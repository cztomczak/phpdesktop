// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "fullscreen.h"
#include <Windows.h>
#include "browser_window.h"
#include "../log.h"

void Fullscreen::ToggleFullscreen() {
    // Original code in chromium > fullscreen_handler.cc > FullscreenHandler::SetFullscreenImpl:
    // http://src.chromium.org/viewvc/chrome/trunk/src/ui/views/win/fullscreen_handler.cc
    BrowserWindow* browserWindow = GetBrowserWindow(
            cefBrowser_->GetHost()->GetWindowHandle());
    if (!browserWindow) {
        LOG_ERROR << "GetBrowserWindow() failed in ToggleFullscreen()";
        return;
    }
    HWND hwnd = browserWindow->GetWindowHandle();
    if (!isFullscreen_) {
        isMaximized_ = (IsZoomed(hwnd) != 0);
        if (isMaximized_) {
            SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
        gwlStyle_ = GetWindowLong(hwnd, GWL_STYLE);
        gwlExStyle_ = GetWindowLong(hwnd, GWL_EXSTYLE);
        RECT rect;
        GetWindowRect(hwnd, &rect);
        windowRect_ = rect;
        int removeStyle, removeExStyle;
        removeStyle = WS_CAPTION | WS_THICKFRAME;
        removeExStyle = (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE
                | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        SetWindowLong(hwnd, GWL_STYLE, gwlStyle_ & ~(removeStyle));
        SetWindowLong(hwnd, GWL_EXSTYLE, gwlExStyle_ & ~(removeExStyle));
        HMONITOR monitor;
        MONITORINFO monitorInfo = {};
        monitorInfo.cbSize = sizeof(monitorInfo);
        // MONITOR_DEFAULTTONULL, MONITOR_DEFAULTTOPRIMARY, 
        // MONITOR_DEFAULTTONEAREST
        monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        GetMonitorInfo(monitor, &monitorInfo);
        int left, top, right, bottom;
        left = monitorInfo.rcMonitor.left;
        top = monitorInfo.rcMonitor.top;
        right = monitorInfo.rcMonitor.right;
        bottom = monitorInfo.rcMonitor.bottom;
        SetWindowPos(hwnd, NULL,
                left, top, right-left, bottom-top,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    } else {
        SetWindowLong(hwnd, GWL_STYLE, gwlStyle_);
        SetWindowLong(hwnd, GWL_EXSTYLE, gwlExStyle_);
        RECT rect = windowRect_;
        SetWindowPos(hwnd, NULL, rect.left, rect.top,
                rect.right - rect.left, rect.bottom - rect.top,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        if (isMaximized_) {
            SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
    }
    isFullscreen_ = !(isFullscreen_);
}

bool Fullscreen::IsFullscreen() {
    return isFullscreen_;
}
