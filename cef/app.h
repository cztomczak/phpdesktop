// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include "include/cef_app.h"
#include <map>
#include "javascript_api.h"
#include "../logger.h"

class App : public CefApp,
            public CefBrowserProcessHandler,
            public CefRenderProcessHandler {
public:
    App() noexcept {}

    // CefApp methods:
    virtual void OnBeforeCommandLineProcessing(
            const CefString& process_type,
            CefRefPtr<CefCommandLine> command_line) override;
  
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
            override { return this; }
  
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
            override { return this; }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

    // CefRenderProcessHandler methods:
    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message)
                                        override;
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame,
            CefRefPtr<CefV8Context> context) override;
    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefDictionaryValue> extra_info) override;
    virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;

protected:
    // There is one instance of JavascriptApi for each browser.
    // Many frames may share the same JavascriptApi object.
    CefRefPtr<JavascriptApi> GetJavascriptApi(CefRefPtr<CefBrowser> browser) {
        std::map<int, CefRefPtr<JavascriptApi> >::iterator it;
        it = javascriptApiMap_.find(browser->GetIdentifier());
        if (it != javascriptApiMap_.end()) {
            return it->second;
        }
        LOGGER_ERROR << "GetJavascriptApi() failed, api not found";
        return nullptr;
    }
    void StoreJavascriptApi(CefRefPtr<CefBrowser> browser, 
            CefRefPtr<JavascriptApi> javascriptApi) {
        std::map<int, CefRefPtr<JavascriptApi> >::iterator it;
        it = javascriptApiMap_.find(browser->GetIdentifier());
        if (it == javascriptApiMap_.end()) {
            javascriptApiMap_[browser->GetIdentifier()] = javascriptApi;
        } else {
            LOGGER_ERROR << "StoreJavascriptApi() failed, api already exists";
        }
    }
    void RemoveJavascriptApi(CefRefPtr<CefBrowser> browser) {
        std::map<int, CefRefPtr<JavascriptApi> >::iterator it;
        it = javascriptApiMap_.find(browser->GetIdentifier());
        if (it != javascriptApiMap_.end()) {
            javascriptApiMap_.erase(it);
        } else {
            LOGGER_ERROR << "RemoveJavascriptApi() failed, api not found";
        }
    }

protected:
    std::map<int, CefRefPtr<JavascriptApi> > javascriptApiMap_;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(App);
};
