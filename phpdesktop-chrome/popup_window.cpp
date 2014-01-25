// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "popup_window.h"
#include "settings.h"
#include "window_utils.h"
#include "dpi_aware.h"

extern wchar_t g_windowClassName[256]; // main.cpp
extern HINSTANCE g_hInstance; // main.cpp

HWND CreatePopupWindow(HWND parentHandle) {
    json_value* appSettings = GetApplicationSettings();
    bool center_relative_to_parent =
            (*appSettings)["popup_window"]["center_relative_to_parent"];
    bool dpi_aware = (*appSettings)["application"]["dpi_aware"];
    int default_width = static_cast<long>(\
            (*appSettings)["popup_window"]["default_size"][0]);
    int default_height = static_cast<long>(\
            (*appSettings)["popup_window"]["default_size"][1]);
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    if (default_width && default_height) {
        width = default_width;
        height = default_height;
        // If width or height is CW_USEDEFAULT then don't call the
        // GetDpiAwareWindowSize nor GetCorrectWindowSize, as CW_USEDEFAULT
        // is a virtual number.
        if (dpi_aware) {
            GetDpiAwareWindowSize(&width, &height);
        }
        GetCorrectWindowSize(&width, &height);
    }
    // Title will be set in BrowserWindow::BrowserWindow().
    // CW_USEDEFAULT cannot be used with WS_POPUP.
    HWND hwnd = CreateWindowEx(0, g_windowClassName,
            0, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            parentHandle, 0, g_hInstance, 0);
    _ASSERT(hwnd);
    if (center_relative_to_parent) {
        CenterWindowRelativeToParent(hwnd, parentHandle);
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    return hwnd;
}