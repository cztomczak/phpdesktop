// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once
#include "../defines.h"
#include "include/cef_browser.h"

class Fullscreen {
public:
    Fullscreen(CefRefPtr<CefBrowser> cefBrowser)
        : cefBrowser_(cefBrowser),
            isFullscreen_(false),
            isMaximized_(false),
            gwlStyle_(0),
            gwlExStyle_(0),
            windowRect_(RECT())
    {
    }
    void ToggleFullscreen();
    bool IsFullscreen();
protected:
    CefRefPtr<CefBrowser> cefBrowser_;
    bool isFullscreen_;
    bool isMaximized_;
    LONG gwlStyle_;
    LONG gwlExStyle_;
    RECT windowRect_;
};
