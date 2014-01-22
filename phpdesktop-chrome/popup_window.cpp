// Copyright (c) 2012-2014 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "popup_window.h"
#include "settings.h"

extern wchar_t g_windowClassName[256]; // main.cpp
extern HINSTANCE g_hInstance; // main.cpp

HWND CreatePopupWindow(HWND parentHandle) {
    json_value* appSettings = GetApplicationSettings();
    bool center_relative_to_parent =
            (*appSettings)["popup_window"]["center_relative_to_parent"];

    // Title will be set in BrowserWindow::BrowserWindow().
    // CW_USEDEFAULT cannot be used with WS_POPUP.
    HWND hwnd = CreateWindowEx(0, g_windowClassName,
            0, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            parentHandle, 0, g_hInstance, 0);
    _ASSERT(hwnd);
    if (center_relative_to_parent) {
        // This won't work properly as real width/height is set later
        // when BrowserEvents2::WindowSetWidth() / WindowSetHeight()
        // are triggered. TODO.
        // CenterWindow(hwnd);
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    return hwnd;
}