// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include <list>
#include <X11/Xlib.h>

#include "include/cef_client.h"

class ClientHandler : public CefClient,
                      public CefDisplayHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler,
                      public CefRequestHandler
{
public:
    ClientHandler();
    ~ClientHandler();

    // Provide access to the single global instance of this object.
    static ClientHandler* GetInstance();

    virtual CefRefPtr<CefBrowser> FindBrowserByXid(::Window xid);

    // CefClient methods:
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
        return this;
    }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
        return this;
    }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override {
        return this;
    }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override {
        return this;
    }

    // CefLifeSpanHandler
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefRequestHandler
    virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                                const CefString& origin_url,
                                int64 new_size,
                                CefRefPtr<CefRequestCallback> callback);

private:
    // List of existing browser windows. Only accessed on the CEF UI thread.
    typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
    BrowserList browser_list_;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(ClientHandler);
};
