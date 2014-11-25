// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include "include/cef_app.h"
#include <map>
#include "javascript_api.h"
#include "../log.h"

class App : public CefApp,
            public CefBrowserProcessHandler,
            public CefRenderProcessHandler {
public:
    App(){}

    // CefApp methods:
    virtual void OnBeforeCommandLineProcessing(
            const CefString& process_type,
            CefRefPtr<CefCommandLine> command_line) OVERRIDE;
  
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
            OVERRIDE { return this; }
  
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
            OVERRIDE { return this; }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;

    // CefRenderProcessHandler methods:
    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message)
                                        OVERRIDE;
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefV8Context> context) OVERRIDE;
    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;

protected:
    // There is one instance of JavascriptApi for each browser.
    // Many frames may share the same JavascriptApi object.
    CefRefPtr<JavascriptApi> GetJavascriptApi(CefRefPtr<CefBrowser> browser) {
        std::map<int, CefRefPtr<JavascriptApi> >::iterator it;
        it = javascriptApiMap_.find(browser->GetIdentifier());
        if (it != javascriptApiMap_.end()) {
            return it->second;
        }
        LOG_ERROR << "GetJavascriptApi() failed, api not found";
        return NULL;
    }
    void StoreJavascriptApi(CefRefPtr<CefBrowser> browser, 
            CefRefPtr<JavascriptApi> javascriptApi) {
        std::map<int, CefRefPtr<JavascriptApi> >::iterator it;
        it = javascriptApiMap_.find(browser->GetIdentifier());
        if (it == javascriptApiMap_.end()) {
            javascriptApiMap_[browser->GetIdentifier()] = javascriptApi;
        } else {
            LOG_ERROR << "StoreJavascriptApi() failed, api already exists";
        }
    }
    void RemoveJavascriptApi(CefRefPtr<CefBrowser> browser) {
        std::map<int, CefRefPtr<JavascriptApi> >::iterator it;
        it = javascriptApiMap_.find(browser->GetIdentifier());
        if (it != javascriptApiMap_.end()) {
            javascriptApiMap_.erase(it);
        } else {
            LOG_ERROR << "RemoveJavascriptApi() failed, api not found";
        }
    }

protected:
    std::map<int, CefRefPtr<JavascriptApi> > javascriptApiMap_;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(App);
};
