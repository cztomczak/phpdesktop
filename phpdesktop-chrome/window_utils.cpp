// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include "log.h"

void CenterWindow(HWND wnd) {
   RECT r,r1;
   GetWindowRect(wnd,&r);
   GetWindowRect(GetDesktopWindow(),&r1);
   MoveWindow(wnd,((r1.right-r1.left)-(r.right-r.left))/2,
      ((r1.bottom-r1.top)-(r.bottom-r.top))/2,
      (r.right-r.left), (r.bottom-r.top),0);
}

bool CenterWindowRelativeToParent(HWND window, HWND parent) {
    RECT rect, parentRect;
    int width, height;
    int screenWidth, screenHeight;
    int x, y;
    GetWindowRect(window, &rect);
    GetWindowRect(parent, &parentRect);
    width  = rect.right  - rect.left;
    height = rect.bottom - rect.top;
    x = ((parentRect.right - parentRect.left) - width) / 2 + parentRect.left;
    y = ((parentRect.bottom - parentRect.top) - height) / 2 + parentRect.top;
    screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (x < 0) { x = 0; }
    if (y < 0) { y = 0; }
    if (x + width > screenWidth)  {
        x = screenWidth  - width;
    }
    if (y + height > screenHeight) {
        y = screenHeight - height;
    }
    // Move it a bit so that popup window does not overlay the parent
    // window completely, when it is of the same size or a bit smaller.
    if (x >= parentRect.left && x < parentRect.left + 48) {
        x = parentRect.left + 48;
    }
    if (y >= parentRect.top && y < parentRect.top + 32) {
        y = parentRect.top + 32;
    }
    MoveWindow(window, x, y, width, height, TRUE);
    return true;
}

void GetCorrectWindowSize(int* width, int* height) {
    int max_width = GetSystemMetrics(SM_CXMAXIMIZED) - 96;
    int max_height = GetSystemMetrics(SM_CYMAXIMIZED) - 64;
    LOG_DEBUG << "Window max width/height = " << max_width << "/" << max_height;
    bool max_size_exceeded = (*width > max_width \
            || *height > max_height);
    if (*width > max_width) {
        *width = max_width;
    }
    if (*height > max_height) {
        *height = max_height;
    }
    if (max_size_exceeded) {
        LOG_DEBUG << "Window max size exceeded, new width/height = "
                    << *width << "/" << *height;
    }
}
