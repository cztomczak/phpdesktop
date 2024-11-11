// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "javascript_api.h"
#include "../version.h"
#include "../logger.h"

bool JavascriptApi::Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) {
    LOGGER_DEBUG << "V8 execute phpdesktop object method: " << name;
    if (name == "GetVersion") {
        retval = CefV8Value::CreateString(GetPhpDesktopVersion());
        return true;
    } else if (name == "ToggleFullscreen") {
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("ToggleFullscreen");
        if (message->IsValid()) {
            // Browser object must be obtained using current context, cannot use previously stored object.
            CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
            context->GetBrowser()->GetMainFrame()->SendProcessMessage(PID_BROWSER, message);
            isFullscreen_ = !(isFullscreen_);
        } else {
            LOGGER_ERROR << "V8 could not create process message: ToggleFullscreen";
        }
        return true;
    } else if (name == "IsFullscreen") {
        retval = CefV8Value::CreateBool(isFullscreen_);
        return true;
    }
    // Function does not exist.
    return false;
}
