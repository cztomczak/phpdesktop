// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include <list>

#include "include/cef_client.h"

class Client : public CefClient,
                public CefContextMenuHandler,
                public CefDisplayHandler,
                public CefDownloadHandler,
                public CefDragHandler,
                public CefLifeSpanHandler,
                public CefLoadHandler,
                public CefRequestHandler
{
public:
    Client();
    ~Client();

    // Provide access to the single global instance of this object.
    static Client* GetInstance();

    // CefClient.
    CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override {
        return this;
    }
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
        return this;
    }
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() override {
        return this;
    }
    CefRefPtr<CefDragHandler> GetDragHandler() override {
        return this;
    }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
        return this;
    }
    CefRefPtr<CefLoadHandler> GetLoadHandler() override {
        return this;
    }
    CefRefPtr<CefRequestHandler> GetRequestHandler() override {
        return this;
    }

    // CefContextMenuHandler.
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefContextMenuParams> params,
                                     CefRefPtr<CefMenuModel> model) override;
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params,
                                      int command_id,
                                      EventFlags event_flags) override;

    // CefDownloadHandler.
    virtual bool OnBeforeDownload(
                                CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDownloadItem> download_item,
                                const CefString& suggested_name,
                                CefRefPtr<CefBeforeDownloadCallback> callback) override;

    // CefDragHandler.
    virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefDragData> dragData,
                             DragOperationsMask mask) override;

    // CefLifeSpanHandler.
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // CefRequestHandler.
    virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                                const CefString& origin_url,
                                int64 new_size,
                                CefRefPtr<CefRequestCallback> callback) override;

private:
    // List of existing browser windows. Only accessed on the CEF UI thread.
    typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
    BrowserList browser_list_;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Client);
};
