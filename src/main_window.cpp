// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "main_window.h"
#include "settings.h"
#include "dpi_aware.h"
#include "window_utils.h"
#include "resource.h"
#include "main.h"

extern wchar_t g_windowClassName[256]; // main.cpp

HWND CreateMainWindow(HINSTANCE hInstance, int nCmdShow, std::string title) {
    json_value* appSettings = GetApplicationSettings();
    int default_width = static_cast<long>((*appSettings)["main_window"]["default_size"][0]);
    int default_height = static_cast<long>((*appSettings)["main_window"]["default_size"][1]);
    bool disable_maximize_button =
            (*appSettings)["main_window"]["disable_maximize_button"];
    bool center_on_screen = (*appSettings)["main_window"]["center_on_screen"];
    bool dpi_aware = (*appSettings)["application"]["dpi_aware"];
    bool start_maximized = (*appSettings)["main_window"]["start_maximized"];
    bool always_on_top = (*appSettings)["main_window"]["always_on_top"];

    if (default_width && default_height) {
        if (dpi_aware) {
            GetDpiAwareWindowSize(&default_width, &default_height);
        }
        GetCorrectWindowSize(&default_width, &default_height);
    } else {
        default_width = CW_USEDEFAULT;
        default_height = CW_USEDEFAULT;
    }

    WNDCLASSEX wc = {0};
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
    if (center_on_screen) {
        CenterWindow(hwnd);
    }
    if (always_on_top) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    if (start_maximized) {
        ShowWindow(hwnd, SW_MAXIMIZE);
    } else {
        ShowWindow(hwnd, nCmdShow);
    }
    UpdateWindow(hwnd);
    return hwnd;
}