// Copyright (c) 2012-2014 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <Windows.h>

#pragma comment(linker, "/manifestdependency:\"type='win32' "\
    "name='Microsoft.Windows.Common-Controls' version='6.0.0.0' "\
    "processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#include <crtdbg.h> // _ASSERT() macro
#include "resource.h"

#include "main_window.h"
#include "logging.h"
#include "executable.h"
#include "fatal_error.h"
#include "file_utils.h"
#include "log.h"
#include "cef/browser_window.h"
#include "settings.h"
#include "single_instance_application.h"
#include "string_utils.h"
#include "web_server.h"
// #include "php_server.h"
#include "cef/app.h"

SingleInstanceApplication g_singleInstanceApplication;
wchar_t* g_singleInstanceApplicationGuid = 0;
wchar_t g_windowClassName[256] = L"";
int g_windowCount = 0;
HINSTANCE g_hInstance = 0;
extern std::string g_webServerUrl;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
    BrowserWindow* browser = 0;
    BOOL b = 0;
    WORD childEvent = 0;
    HWND childHandle = 0;
    HWND shellBrowserHandle = 0;

    switch (uMsg) {
        case WM_SIZE:
            browser = GetBrowserWindow(hwnd);
            if (browser) {
                browser->OnSize();
            } else {
                LOG_WARNING << "WindowProc(): event WM_SIZE: "
                               "could not fetch BrowserWindow";
            }
            break;
        case WM_CREATE:
            g_windowCount++;
            if (GetWindow(hwnd, GW_OWNER)) {
                browser = new BrowserWindow(hwnd, true);
            } else {
                browser = new BrowserWindow(hwnd, false);
            }
            StoreBrowserWindow(hwnd, browser);
            return 0;
        case WM_DESTROY:
            g_windowCount--;
            LOG_DEBUG << "WM_DESTROY";
            RemoveBrowserWindow(hwnd);
            if (g_windowCount <= 0) {
                StopWebServer();
#ifdef DEBUG
                // Debugging mongoose, see InitializeLogging().
                printf("----------------------------------------");
                printf("----------------------------------------\n");
#endif
                PostQuitMessage(0);
            }
            break;
        case WM_GETMINMAXINFO:
            browser = GetBrowserWindow(hwnd);
            if (browser) {
                browser->OnGetMinMaxInfo(uMsg, wParam, lParam);
                return 0;
            } else {
                // GetMinMaxInfo may fail during window creation, so
                // log severity is only DEBUG.
                LOG_DEBUG << "WindowProc(): event WM_GETMINMAXINFO: "
                             "could not fetch BrowserWindow";
            }
            break;
        case WM_SETFOCUS:
            browser = GetBrowserWindow(hwnd);
            if (browser) {
                browser->SetFocus();
                return 0;
            } else {
                LOG_DEBUG << "WindowProc(): event WM_SETFOCUS: "
                             "could not fetch BrowserWindow";
            }
            break;
        case WM_ERASEBKGND:
            browser = GetBrowserWindow(hwnd);
            if (browser && browser->GetCefBrowser().get()) {
                CefWindowHandle hwnd = \
                        browser->GetCefBrowser()->GetHost()->GetWindowHandle();
                if (hwnd) {
                    // Dont erase the background if the browser window has been loaded
                    // (this avoids flashing)
                    return 1;
                }
            }
            break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPTSTR lpstrCmdLine, int nCmdShow) {
    g_hInstance = hInstance;
    json_value* appSettings = GetApplicationSettings();
    
    // Debugging options.
    bool show_console = (*appSettings)["debugging"]["show_console"];
    bool subprocess_show_console = (*appSettings)["debugging"]["subprocess_show_console"];
    std::string log_level = (*appSettings)["debugging"]["log_level"];
    std::string log_file = (*appSettings)["debugging"]["log_file"];
    if (log_file.length() && log_file.find(":") == std::string::npos) {
        log_file = GetExecutableDirectory() + "\\" + log_file;
        log_file = GetRealPath(log_file);
    }
    
    // Initialize logging.
    if (std::wstring(lpstrCmdLine).find(L"--type=") != std::string::npos) {
        // This is a subprocess.
        InitializeLogging(subprocess_show_console, log_level, log_file);
    } else {
        // Main browser process.
        InitializeLogging(show_console, log_level, log_file);
    }
    
    // CEF subprocesses.
    CefMainArgs main_args(hInstance);
    CefRefPtr<App> app(new App);    
    int exit_code = CefExecuteProcess(main_args, app.get());
    if (exit_code >= 0) {
        ShutdownLogging();
        return exit_code;
    }

    LOG_INFO << "--------------------------------------------------------";
    LOG_INFO << "Started application";

    if (log_file.length())
        LOG_INFO << "Logging to: " << log_file;
    else
        LOG_INFO << "No logging file set";
    LOG_INFO << "Log level = "
             << FILELog::ToString(FILELog::ReportingLevel());

    // Main window title option.
    std::string main_window_title = (*appSettings)["main_window"]["title"];
    if (main_window_title.empty())
        main_window_title = GetExecutableName();

    // Single instance guid option.
    const char* single_instance_guid =
            (*appSettings)["application"]["single_instance_guid"];
    if (single_instance_guid && single_instance_guid[0] != 0) {
        int guidSize = strlen(single_instance_guid) + 1;
        g_singleInstanceApplicationGuid = new wchar_t[guidSize];
        Utf8ToWide(single_instance_guid, g_singleInstanceApplicationGuid,
                   guidSize);
    }
    if (g_singleInstanceApplicationGuid
            && g_singleInstanceApplicationGuid[0] != 0) {
        g_singleInstanceApplication.Initialize(
                g_singleInstanceApplicationGuid);
	    if (g_singleInstanceApplication.IsRunning()) {
            HWND hwnd = FindWindow(g_singleInstanceApplicationGuid, NULL);
            if (hwnd) {
                if (IsIconic(hwnd))
                    ShowWindow(hwnd, SW_RESTORE);
                SetForegroundWindow(hwnd);
                return 0;
            }
        }
    }

    // Window class name.
    if (g_singleInstanceApplicationGuid) {
        swprintf_s(g_windowClassName, _countof(g_windowClassName), L"%s",
                   g_singleInstanceApplicationGuid);
    } else {
        swprintf_s(g_windowClassName, _countof(g_windowClassName), L"%s",
                   Utf8ToWide(GetExecutableName()).c_str());
    }

    if (!StartWebServer()) {
        FatalError(NULL, "Could not start internal web server.\n"
                   "Exiting application.");
    }

    CefSettings cef_settings;

    // log_file
    std::string chrome_log_file = (*appSettings)["chrome"]["log_file"];
    if (chrome_log_file.length() && chrome_log_file.find(":") == std::string::npos) {
        chrome_log_file = GetExecutableDirectory() + "\\" + chrome_log_file;
        chrome_log_file = GetRealPath(chrome_log_file);
    }
    CefString(&cef_settings.log_file) = chrome_log_file;
    
    // log_severity
    std::string chrome_log_severity = (*appSettings)["chrome"]["log_severity"];
    cef_log_severity_t log_severity = LOGSEVERITY_DEFAULT;
    if (chrome_log_severity == "verbose") {
        log_severity = LOGSEVERITY_VERBOSE;
    } else if (chrome_log_severity == "info") {
        log_severity = LOGSEVERITY_INFO;
    } else if (chrome_log_severity == "warning") {
        log_severity = LOGSEVERITY_WARNING;
    } else if (chrome_log_severity == "error") {
        log_severity = LOGSEVERITY_ERROR;
    } else if (chrome_log_severity == "error-report") {
        log_severity = LOGSEVERITY_ERROR_REPORT;
    } else if (chrome_log_severity == "disable") {
        log_severity = LOGSEVERITY_DISABLE;
    }
    cef_settings.log_severity = log_severity;
    
    // cache_path
    std::string cache_path = (*appSettings)["chrome"]["cache_path"];
    if (cache_path.length() && cache_path.find(":") == std::string::npos) {
        cache_path = GetExecutableDirectory() + "\\" + cache_path;
        cache_path = GetRealPath(cache_path);
    }
    CefString(&cef_settings.cache_path) = cache_path;

    CefInitialize(main_args, cef_settings, app.get());
    CreateMainWindow(hInstance, nCmdShow, main_window_title);
    CefRunMessageLoop();
    CefShutdown();

    LOG_INFO << "Ended application";
    LOG_INFO << "--------------------------------------------------------";

    ShutdownLogging();

    return 0;
}
