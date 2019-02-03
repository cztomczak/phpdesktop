// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "app.h"
#include "client_handler.h"
#include "utils.h"
#include "mongoose_server.h"
#include "settings.h"
#include "main_message_loop_std.h"

#include "include/base/cef_logging.h"
#include "include/wrapper/cef_helpers.h"

#include "gtk.h"

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <sys/file.h>
#include <errno.h>

// Forwards
void create_browser(::Window xid);

// Globals
std::string g_cgi_env_from_argv = "";

int x11_error_handler(Display* display, XErrorEvent* event) {
    LOG(WARNING) << "X error received: "
                 << "type " << event->type << ", "
                 << "serial " << event->serial << ", "
                 << "error_code " << static_cast<int>(event->error_code) << ", "
                 << "request_code " << static_cast<int>(event->request_code)
                 << ", "
                 << "minor_code " << static_cast<int>(event->minor_code);
    return 0;
}

int x11_io_error_handler(Display* display) {
    return 0;
}

void app_terminate_signal(int signatl) {
    LOG(INFO) << "App terminate signal";
    CefQuitMessageLoop();
}

int main(int argc, char **argv) {
    // Passing ENV variables to PHP using the --cgi-environment
    // command line arg passed to app.
    if (argv) {
        for (int i = 0; i < argc; i++) {
            std::string arg = argv[i];
            size_t pos = arg.find("=");
            if (pos != std::string::npos) {
                std::string name = arg.substr(0, pos);
                std::string value = arg.substr(pos+1, std::string::npos);
                if (name == "--cgi-environment" && value.length()) {
                    g_cgi_env_from_argv.assign(value);
                }
            }
        }
    }

    // Create a copy of |argv| on Linux because Chromium mangles the value
    // internally (see CEF issue #620).
    CefScopedArgArray scoped_arg_array(argc, argv);
    char** argv_copy = scoped_arg_array.array();

    // Provide CEF with command-line arguments.
    CefMainArgs main_args(argc, argv);
    CefRefPtr<CefCommandLine> cmdline = CefCommandLine::CreateCommandLine();
    cmdline->InitFromArgv(main_args.argc, main_args.argv);

    // Log what process type is launching
    if (!cmdline->HasSwitch("type")) {
        // If there is no --type flag then this is main process
        LOG(INFO) << "Launching Browser process (main process)\n";
    } else {
        const std::string& process_type = cmdline->GetSwitchValue("type");
        // Types of subprocesses:
        // - "renderer"
        // - "gpu-process"
        // - "zygote"
        // - "ppapi", "ppapi-broker" (plugins)
        // - "utility"
        // - others
        LOG(INFO) << "Launching " << process_type << " subprocess\n";
        std::string cmdlinestr;
        for (int argi=1; argi < argc; argi++) {
            cmdlinestr.append(argv[argi]).append(" ");
        }
        // cmdline->GetCommandLineString().ToString() is buggy.
        LOG(INFO) << "Subprocess args: " << cmdlinestr;
    }

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
    // that share the same executable. This function checks the command-line
    // and, if this is a sub-process, executes the appropriate logic.
    int exit_code = CefExecuteProcess(main_args, NULL, NULL);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }

    LOG(INFO) << "Executable directory: " << get_executable_dir();

    // If reading settings.json fails exit app immediately
    json_value* app_settings = get_app_settings();
    if (get_app_settings_error().length()) {
        std::string error = get_app_settings_error();
        error.append(".\nApplication will terminate immediately.");
        LOG(ERROR) << error.c_str();
        gtk_init(&argc, &argv_copy);
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new(NULL,
                    GTK_DIALOG_DESTROY_WITH_PARENT,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "%s",
                    error.c_str());
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return 1;
    }

    // Single instance application
    const char* guid = (*app_settings)["application"]["single_instance_guid"];
    if (strlen(guid)) {
        char pid_file[PATH_MAX] = "";
        snprintf(pid_file, PATH_MAX, "/tmp/%s.pid", guid);
        int pid_fp = open(pid_file, O_CREAT | O_RDWR, 0666);
        int pid_lock = flock(pid_fp, LOCK_EX | LOCK_NB);
        if (pid_lock && EWOULDBLOCK == errno) {
            // Another instance is running
            const char* title = (*app_settings)["main_window"]["title"];
            const char message_tmpl[PATH_MAX] = "There is another instance of"
                                                " %s already running.\nOnly"
                                                " one instance is allowed.";
            char message[PATH_MAX];
            snprintf(message, PATH_MAX, message_tmpl, title);
            LOG(INFO) << message;
            gtk_init(&argc, &argv_copy);
            GtkWidget *dialog;
            dialog = gtk_message_dialog_new(NULL,
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "%s",
                        message);
            gtk_window_set_title(GTK_WINDOW(dialog), title);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
            return 1;
        }
    }

    // Start Mongoose server
    mongoose_start();

    // Specify CEF global settings here
    CefSettings cef_settings;
    cef_settings.no_sandbox = true;

    // log_file
    std::string log_file((*app_settings)["chrome"]["log_file"]);
    log_file = get_full_path(log_file);
    CefString(&cef_settings.log_file) = log_file;

    // log_severity
    std::string log_severity((*app_settings)["chrome"]["log_severity"]);
    if (log_severity == "verbose") {
        cef_settings.log_severity = LOGSEVERITY_VERBOSE;
    } else if (log_severity == "info") {
        cef_settings.log_severity = LOGSEVERITY_INFO;
    } else if (log_severity == "warning") {
        cef_settings.log_severity = LOGSEVERITY_WARNING;
    } else if (log_severity == "error") {
        cef_settings.log_severity = LOGSEVERITY_ERROR;
    } else if (log_severity == "disable") {
        cef_settings.log_severity = LOGSEVERITY_DISABLE;
    } else {
        cef_settings.log_severity = LOGSEVERITY_DEFAULT;
    }

    // cache_path
    std::string cache_path((*app_settings)["chrome"]["cache_path"]);
    cache_path = get_full_path(cache_path);
    CefString(&cef_settings.cache_path) = cache_path;

    // remote_debugging_port
    int remote_debugging_port(
            (*app_settings)["chrome"]["remote_debugging_port"]);
    if (remote_debugging_port == 0) {
        remote_debugging_port = random(49152, 65535+1);
        int i = 100;
        while (((i--) > 0)
                && remote_debugging_port == mongoose_get_port_int()) {
            remote_debugging_port = random(49152, 65535+1);
        }
    }
    if (remote_debugging_port > 0) {
        LOG(INFO) << "remote_debugging_port = " << remote_debugging_port;
        cef_settings.remote_debugging_port = remote_debugging_port;
    }

    // App implements application-level callbacks for the browser
    // process.
    CefRefPtr<App> app(new App);

    // Initialize GDK threads before CEF.
    gdk_threads_init();

    scoped_ptr<MainMessageLoop> message_loop;
    message_loop.reset(new MainMessageLoopStd);

    // Log messages created by LOG() macro will be written to debug.log
    // file only after CEF was initialized. Before CEF is initialized
    // all logs are only printed to console.
    LOG(INFO) << "Initialize CEF";
    CefInitialize(main_args, cef_settings, app.get(), NULL);

    // The Chromium sandbox requires that there only be a single thread during
    // initialization. Therefore initialize GTK after CEF.
    gtk_init(&argc, &argv_copy);

    // Install xlib error handlers so that the application won't be terminated
    // on non-fatal errors. X11 errors appearing in debug logs usually can be
    // ignored.
    XSetErrorHandler(x11_error_handler);
    XSetIOErrorHandler(x11_io_error_handler);

    // Install a signal handler so we clean up after ourselves.
    signal(SIGINT, app_terminate_signal);
    signal(SIGTERM, app_terminate_signal);

    // Create Gtk window
    std::string app_icon_path((*app_settings)["main_window"]["icon"]);
    app_icon_path = get_full_path(app_icon_path);
    bool center_on_screen = (*app_settings)["main_window"]["center_on_screen"];
    int default_width = static_cast<long>(
            (*app_settings)["main_window"]["default_size"][0]);
    int default_height = static_cast<long>(
            (*app_settings)["main_window"]["default_size"][1]);
    GtkWidget* gtk_window = create_gtk_window(
            (*app_settings)["main_window"]["title"],
            app_icon_path.c_str(),
            center_on_screen,
            default_width, default_height);

    // Create browser
    ::Window xid = get_window_xid(gtk_window);
    LOG(INFO) << "Top window xid=" << xid;
    create_browser(xid);

    // Run the message loop. This will block until Quit() is called.
    int result = message_loop->Run();

    LOG(INFO) << "Stop Mongoose server";
    mongoose_stop();

    LOG(INFO) << "Shutdown CEF";
    CefShutdown();

    // Release objects in reverse order of creation.
    message_loop.reset();

    return result;
}

void create_browser(::Window xid) {
    // The call to CreateBrowserSync cannot be in the same block scope
    // as the call to CefShutdown otherwise it results in segmentation
    // fault with the stack trace as seen below. Making a call to
    // browser->Release() did not help.
    // ----
    // #0  MaybeSendDestroyedNotification () at
    //     ./../../chrome/browser/profiles/profile.cc:294
    // #1  0x00007ffff34c74b5 in Shutdown () at
    //     ../../cef/libcef/browser/browser_context.cc:81
    // ----
    json_value* app_settings = get_app_settings();
    CefBrowserSettings browser_settings;
    CefWindowInfo window_info;
    int default_width = static_cast<long>(
            (*app_settings)["main_window"]["default_size"][0]);
    int default_height = static_cast<long>(
            (*app_settings)["main_window"]["default_size"][1]);
    CefRect browser_rect(0, 0, default_width, default_height);
    window_info.SetAsChild(xid, browser_rect);
    CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(
        window_info,
        ClientHandler::GetInstance(),
        mongoose_get_url(),
        browser_settings,
        NULL);
    LOG(INFO) << "Browser xid=" << browser->GetHost()->GetWindowHandle();
}
