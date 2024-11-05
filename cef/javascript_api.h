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
    JavascriptApi(CefRefPtr<CefBrowser> cefBrowser)
        : cefBrowser_(cefBrowser),
            isFullscreen_(false) 
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
    CefRefPtr<CefBrowser> cefBrowser_;
    bool isFullscreen_;

private:
    // Provide the reference counting implementation for this class.
    IMPLEMENT_REFCOUNTING(JavascriptApi);
};