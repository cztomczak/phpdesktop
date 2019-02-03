// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "client_handler.h"
#include "settings.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

namespace {

ClientHandler* g_instance = NULL;

} // namespace

ClientHandler::ClientHandler()
{
    DCHECK(!g_instance);
    g_instance = this;
    const char* js_dialog_title = (*get_app_settings())["main_window"]["title"];
    dialog_handler_ = new ClientDialogHandlerGtk(js_dialog_title);
}

ClientHandler::~ClientHandler()
{
    g_instance = NULL;
}

// static
ClientHandler* ClientHandler::GetInstance() {
    if (g_instance) {
        return g_instance;
    } else {
        return new ClientHandler();
    }
}

CefRefPtr<CefBrowser> ClientHandler::FindBrowserByXid(::Window xid) {
    BrowserList::iterator bit = browser_list_.begin();
    for (; bit != browser_list_.end(); ++bit) {
        ::Window browser_xid = (*bit)->GetHost()->GetWindowHandle();
        if (browser_xid == xid) {
            return *bit;
        }
    }
    return NULL;
}

void ClientHandler::OnBeforeDownload(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        const CefString& suggested_name,
        CefRefPtr<CefBeforeDownloadCallback> callback) {
    bool enable_downloads = (*get_app_settings())["chrome"]["enable_downloads"];
    if (enable_downloads) {
        LOG(INFO) << "About to download a file: " << suggested_name.ToString();
        callback->Continue(suggested_name, true);
    } else {
        LOG(INFO) << "Tried to download a file, but downloads are disabled";
    }
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    // Add to the list of existing browsers.
    browser_list_.push_back(browser);
    if (browser->IsPopup()) {
        // Set window title for popup, same as main window title.
        // Icon for popup is still missing (todo).
        const char* title = (*get_app_settings())["main_window"]["title"];
        XStoreName(cef_get_xdisplay(), browser->GetHost()->GetWindowHandle(),
                   title);
    }
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

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

bool ClientHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                                   const CefString& origin_url,
                                   int64 new_size,
                                   CefRefPtr<CefRequestCallback> callback) {
    CEF_REQUIRE_IO_THREAD();
    // Allow all requests
    callback->Continue(true);
    return true;
}
