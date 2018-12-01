#include "app.h"
#include "client_handler.h"

#include "include/base/cef_logging.h"
#include "include/wrapper/cef_helpers.h"

#include <X11/Xlib.h>
#include <gtk/gtk.h>

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cerrno>


int XErrorHandlerImpl(Display* display, XErrorEvent* event) {
    LOG(WARNING) << "X error received: "
                 << "type " << event->type << ", "
                 << "serial " << event->serial << ", "
                 << "error_code " << static_cast<int>(event->error_code) << ", "
                 << "request_code " << static_cast<int>(event->request_code)
                 << ", "
                 << "minor_code " << static_cast<int>(event->minor_code);
    return 0;
}

int XIOErrorHandlerImpl(Display* display) {
    return 0;
}

int main(int argc, char **argv) {
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

    // Install xlib error handlers so that the application won't be terminated
    // on non-fatal errors. X11 errors appearing in debug logs usually can be
    // ignored.
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);

    // Specify CEF global settings here.
    CefSettings settings;
    settings.no_sandbox = true;
    CefString( &settings.log_file ) = "debug.log";
    settings.log_severity = LOGSEVERITY_INFO;

    // App implements application-level callbacks for the browser
    // process.
    CefRefPtr<App> app(new App);

    // Initialize GDK threads before CEF.
    // gdk_threads_init();

    LOG(INFO) << "Initialize CEF";
    // Log messages created by LOG() macro will be written to debug.log
    // file only after CEF was initialized. Before CEF is initialized
    // all logs are only printed to console.
    CefInitialize(main_args, settings, app.get(), NULL);

    // The Chromium sandbox requires that there only be a single thread during
    // initialization. Therefore initialize GTK after CEF.
    gtk_init(&argc, &argv_copy);

     // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    // Create browser
    CefWindowInfo window_info;
    CefRect browser_rect(0, 0, 800, 600);
    window_info.SetAsChild(0, browser_rect);
    CefBrowserHost::CreateBrowserSync(
        window_info,
        ClientHandler::GetInstance(),
        "https://www.google.com/",
        browser_settings,
        NULL);

    CefRunMessageLoop();
    
    LOG(INFO) << "Shutdown CEF";
    CefShutdown();

    return 0;
}
