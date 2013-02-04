// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"

#pragma comment(linker, "/manifestdependency:\"type='win32' "\
    "name='Microsoft.Windows.Common-Controls' version='6.0.0.0' "\
    "processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#include <crtdbg.h> // _ASSERT() macro
#include "resource.h"
#include <iostream>

#include "executable.h"
#include "fatal_error.h"
#include "file_utils.h"
#include "log.h"
#include "msie/internet_features.h"
#include "msie/browser_window.h"
#include "settings.h"
#include "single_instance_application.h"
#include "string_utils.h"
#include "web_server.h"
#include "window_utils.h"

#define BROWSER_GENERIC_TIMER 1

SingleInstanceApplication g_singleInstanceApplication;
wchar_t* g_singleInstanceApplicationGuid = 0;
wchar_t g_windowClassName[256] = L"";
int g_windowCount = 0;
HINSTANCE g_hInstance = 0;

HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, std::string title);
void InitLogging(bool show_console, std::string log_level, 
                 std::string log_file);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, 
                            LPARAM lParam) {
    BrowserWindow* browser = 0;
    UINT_PTR timer = 0;
    BOOL b = 0;
    switch (uMsg) {
        case WM_SIZE:
            browser = GetBrowserWindow(hwnd);
            if (browser) {
                browser->OnResize(uMsg, wParam, lParam);
            } else {
                LOG_WARNING << "WindowProc(): event WM_SIZE: "
                               "could not fetch BrowserWindow";
                return 1;
            }
            return 0;
        case WM_CREATE:
            g_windowCount++;
            browser = new BrowserWindow(hwnd);
            StoreBrowserWindow(hwnd, browser);
            timer = SetTimer(hwnd, BROWSER_GENERIC_TIMER, 50, 0);
            if (!timer) {
                LOG_WARNING << "WindowProc(): event WM_CREATE: "
                               "SetTimer() failed";
            }
            return 0;
        case WM_DESTROY:
            g_windowCount--;
            b = KillTimer(hwnd, BROWSER_GENERIC_TIMER);
            _ASSERT(b);
            RemoveBrowserWindow(hwnd);
            if (g_windowCount <= 0) {
                TerminateWebServer();
                PostQuitMessage(0);
            }
            return 1;
        case WM_TIMER:
            if (wParam == BROWSER_GENERIC_TIMER) {
                browser = GetBrowserWindow(hwnd);
                if (browser) {
                    browser->OnTimer(uMsg, wParam, lParam);
                } else {
                    LOG_WARNING << "WindowProc(): event WM_TIMER failed: "
                                   "could not fetch BrowserWindow";
                    return 1;
                }
                return 0;
            }
            return 1;
        case WM_GETMINMAXINFO:
            browser = GetBrowserWindow(hwnd);
            if (browser) {
                browser->OnGetMinMaxInfo(uMsg, wParam, lParam);
            } else {
                // GetMinMaxInfo may fail during window creation, so
                // log severity is only DEBUG.
                LOG_DEBUG << "WindowProc(): event WM_GETMINMAXINFO: "
                             "could not fetch BrowserWindow";
                return 1;
            }
            return 0;
        case WM_SETFOCUS:
            browser = GetBrowserWindow(hwnd);
            if (browser) {
                browser->SetFocus();
            } else {
                LOG_DEBUG << "WindowProc(): event WM_SETFOCUS: "
                             "could not fetch BrowserWindow";
                return 1;
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
bool ProcessKeyboardMessage(MSG* msg) {
    if (msg->message == WM_KEYDOWN 
            || msg->message == WM_KEYUP
            || msg->message == WM_SYSKEYDOWN 
            || msg->message == WM_SYSKEYUP) {
        HWND root = GetAncestor(msg->hwnd, GA_ROOT);
        BrowserWindow* browser = GetBrowserWindow(root);
        if (browser) {
            if (browser->TranslateAccelerator(msg))
                return true;
        } else {
            LOG_DEBUG << "ProcessKeyboardMessage(): could not fetch BrowserWindow";
        }
    }
    return false;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPTSTR lpstrCmdLine, int nCmdShow) {
    g_hInstance = hInstance;
    json_value* settings = GetApplicationSettings();
    // Debugging options.    
    bool show_console = (*settings)["debugging"]["show_console"];
    std::string log_level = (*settings)["debugging"]["log_level"];
    std::string log_file = (*settings)["debugging"]["log_file"];
    if (log_file.length()) {
        log_file = GetExecutableDirectory() + "\\" + log_file;
        log_file = GetRealPath(log_file);
    }

    InitLogging(show_console, log_level, log_file);
    LOG_INFO << "--------------------------------------------------------";
    LOG_INFO << "Started application";    
    
    if (log_file.length())
        LOG_INFO << "Logging to: " << log_file;
    else
        LOG_INFO << "No logging file set";
    LOG_INFO << "Log level = "
             << FILELog::ToString(FILELog::ReportingLevel());

    // Main window title option.
    std::string main_window_title = (*settings)["main_window"]["title"];
    if (main_window_title.empty())
        main_window_title = GetExecutableName();

    // Single instance guid option.
    const char* single_instance_guid = 
            (*settings)["application"]["single_instance_guid"];
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
        FatalError(NULL, "Could not start internal web-server.\n"
                   "Exiting application.");
    }

    HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED 
                                | COINIT_DISABLE_OLE1DDE);
    _ASSERT(SUCCEEDED(hr));

    SetInternetFeatures();
    CreateMainWindow(hInstance, nCmdShow, main_window_title);
    
    MSG msg;
    int ret;
    while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
        if (ret == -1) {
            LOG_ERROR << "WinMain.GetMessage() returned -1";
            _ASSERT(false);
            break;
        } else {
            if (!ProcessKeyboardMessage(&msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    CoUninitialize();
    LOG_INFO << "Ended application";
    LOG_INFO << "--------------------------------------------------------";

    return ret;
}
HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, std::string title) {
    json_value* settings = GetApplicationSettings();    
    long default_width = (*settings)["main_window"]["default_size"][0];
    long default_height = (*settings)["main_window"]["default_size"][1];
    bool disable_maximize_button = 
            (*settings)["main_window"]["disable_maximize_button"];
    bool center_on_screen = (*settings)["main_window"]["center_on_screen"];
    if (!default_width || !default_height) {
        default_width = CW_USEDEFAULT;
        default_height = CW_USEDEFAULT;
    }

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_MAINWINDOW));
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = g_windowClassName;
    
    ATOM atom = RegisterClassEx(&wc);
    _ASSERT(atom);

    HWND hwnd = CreateWindowEx(0, g_windowClassName,
            Utf8ToWide(title).c_str(), WS_OVERLAPPEDWINDOW, 
            CW_USEDEFAULT, CW_USEDEFAULT, default_width, default_height, 
            HWND_DESKTOP, 0, hInstance, 0);
    _ASSERT(hwnd);
    if (disable_maximize_button) {
        int style = GetWindowLong(hwnd, GWL_STYLE);
        _ASSERT(style);
        int ret = SetWindowLong(hwnd, GWL_STYLE, style &~WS_MAXIMIZEBOX);
        _ASSERT(ret);
    }
    if (center_on_screen)
        CenterWindow(hwnd);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return hwnd;
}
void InitLogging(bool show_console, std::string log_level, 
                 std::string log_file) {
    if (show_console) {
        AllocConsole();
        FILE* freopen_file;
        freopen_s(&freopen_file, "CONIN$", "rb", stdin);
        freopen_s(&freopen_file, "CONOUT$", "wb", stdout);
        freopen_s(&freopen_file, "CONOUT$", "wb", stderr);
    }
    
    if (log_level.length())
        FILELog::ReportingLevel() = FILELog::FromString(log_level);
    else
        FILELog::ReportingLevel() = logINFO;

    if (log_file.length()) {
        FILE* pFile;
        if (0 == _wfopen_s(&pFile, Utf8ToWide(log_file).c_str(), L"a"))
            Output2FILE::Stream() = pFile;
        else
            LOG_INFO << "Opening log file for writing failed";
    }
}
