// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "client.h"
#include "settings.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

namespace {
    Client* g_Client = NULL;
}

Client::Client()
{
    DCHECK(!g_Client);
    g_Client = this;
}

Client::~Client()
{
    g_Client = NULL;
}

// static
Client* Client::GetInstance() {
    if (g_Client) {
        return g_Client;
    } else {
        return new Client();
    }
}

// CefContextMenuHandler.

#define _MENU_ID_DEVTOOLS                         MENU_ID_USER_FIRST + 1
#define _MENU_ID_RELOAD_PAGE                      MENU_ID_USER_FIRST + 2

void Client::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefContextMenuParams> params,
                                CefRefPtr<CefMenuModel> model)
{
    CEF_REQUIRE_UI_THREAD();

    json_value* settings = Settings();
    bool enable_menu = (*settings)["chrome"]["context_menu"]["enable_menu"];
    bool navigation = (*settings)["chrome"]["context_menu"]["navigation"];
    bool print = (*settings)["chrome"]["context_menu"]["print"];
    bool view_source = (*settings)["chrome"]["context_menu"]["view_source"];
    bool devtools = (*settings)["chrome"]["context_menu"]["devtools"];

    if (!enable_menu) {
        model->Clear();
        return;
    }

    if (!navigation) {
        model->Remove(MENU_ID_BACK);
        model->Remove(MENU_ID_FORWARD);
        // Remote separator.
        model->RemoveAt(0);
    }
    if (!print) {
        model->Remove(MENU_ID_PRINT);
    }
    if (!view_source) {
        model->Remove(MENU_ID_VIEW_SOURCE);
    }

    if (!params->IsEditable() && params->GetSelectionText().empty()
            && (params->GetPageUrl().length() || params->GetFrameUrl().length())) {
        if (navigation) {
            model->InsertItemAt(2, _MENU_ID_RELOAD_PAGE, "Reload");
        }
        if (devtools) {
            model->AddSeparator();
            model->AddItem(_MENU_ID_DEVTOOLS, "Show Developer Tools");
        }
    }
}

bool Client::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags)
{
    CEF_REQUIRE_UI_THREAD();
    if (command_id == _MENU_ID_RELOAD_PAGE) {
        browser->ReloadIgnoreCache();
        return true;
    } else if (command_id == _MENU_ID_DEVTOOLS) {
        CefWindowInfo window_info;
        CefBrowserSettings settings;
        CefPoint inspect;
        browser->GetHost()->ShowDevTools(window_info, NULL, settings, inspect);
        return true;
    }
    return false;
}

// CefDownloadHandler.

void Client::OnBeforeDownload(  CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDownloadItem> download_item,
                                const CefString& suggested_name,
                                CefRefPtr<CefBeforeDownloadCallback> callback)
{
    bool enable_downloads = (*Settings())["chrome"]["enable_downloads"];
    if (enable_downloads) {
        LOG(INFO) << "About to download a file: " << suggested_name.ToString();
        callback->Continue(suggested_name, true);
    } else {
        // To cancel just do nothing.
        LOG(INFO) << "Tried to download a file, but downloads are disabled";
    }
    return true;
}

// CefDragHandler.

bool Client::OnDragEnter(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDragData> dragData,
                                DragOperationsMask mask)
{
    return false;
}

// CefLifeSpanHandler

void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();
    // Add to the list of existing browsers.
    browser_list_.push_back(browser);
}

void Client::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Cookies are not flushed to disk when closing app immediately.
    // Need to call FlushStore manually when browser is closing.
    browser->GetHost()->GetRequestContext()->GetDefaultCookieManager(NULL)
            ->FlushStore(NULL);

    // Remove from the list of existing browsers.
    BrowserList::iterator bit = browser_list_.begin();
    for (; bit != browser_list_.end(); ++bit) {
        if ((*bit)->IsSame(browser)) {
            browser_list_.erase(bit);
            break;
        }
    }

    if (browser_list_.empty()) {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}

// CefRequestHandler.

bool Client::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                            const CefString& origin_url,
                            int64 new_size,
                            CefRefPtr<CefRequestCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();
    // Allow all requests
    callback->Continue(true);
    return true;
}
