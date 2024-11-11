// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include "include/cef_v8.h"
#include "fullscreen.h"

class JavascriptApi : public CefV8Handler
{
public:
    JavascriptApi()
        : isFullscreen_(false)
    {
    }

    virtual bool Execute(const CefString& name,
                        CefRefPtr<CefV8Value> object,
                        const CefV8ValueList& arguments,
                        CefRefPtr<CefV8Value>& retval,
                        CefString& exception) override;

    void SetIsFullscreen(bool isFullscreen) {
        isFullscreen_ = isFullscreen;
    }

protected:
    // Can't store CefBrowser object in CefV8Handler as it won't work properly when sending process message.
    // Issue reported here: https://magpcss.org/ceforum/viewtopic.php?f=6&t=20040

    bool isFullscreen_;

private:
    // Provide the reference counting implementation for this class.
    IMPLEMENT_REFCOUNTING(JavascriptApi);
};