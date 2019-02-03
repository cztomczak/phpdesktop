// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "client_handler.h"
#include "settings.h"
#include "gtk.h"

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

// CefContextMenuHandler

#define _MENU_ID_DEVTOOLS                         MENU_ID_USER_FIRST + 1
#define _MENU_ID_RELOAD_PAGE                      MENU_ID_USER_FIRST + 2

void ClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefContextMenuParams> params,
                                        CefRefPtr<CefMenuModel> model) {
    CEF_REQUIRE_UI_THREAD();

    json_value* settings = get_app_settings();
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
            && (params->GetPageUrl().length()
                    || params->GetFrameUrl().length())) {
        if (navigation) {
            model->InsertItemAt(2, _MENU_ID_RELOAD_PAGE, "Reload");
        }
        if (devtools) {
            model->AddSeparator();
            model->AddItem(_MENU_ID_DEVTOOLS, "Show Developer Tools");
        }
    }
}

bool ClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefContextMenuParams> params,
                                         int command_id,
                                         EventFlags event_flags) {
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

// CefDownloadHandler

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

// CefDragHandler

bool ClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDragData> dragData,
                                DragOperationsMask mask) {
    bool external_drag = (*get_app_settings())["chrome"]["external_drag"];
    if (external_drag) {
        return false;
    } else {
        return true;
    }
}

// CefLifeSpanHandler

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    // Add to the list of existing browsers.
    browser_list_.push_back(browser);
    if (browser->IsPopup()) {
        // Set window title for popup, same as main window title.
        // Icon for popup is still missing.
        const char* title = (*get_app_settings())["main_window"]["title"];
        XStoreName(cef_get_xdisplay(), browser->GetHost()->GetWindowHandle(),
                   title);
    }
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();

    // Cookies are not flushed to disk when closing app immediately.
    // Need to call FlushStore manually when browser is closing.
    browser->GetHost()->GetRequestContext()->GetDefaultCookieManager(NULL)
            ->FlushStore(NULL);

    // Currently if main window is closed app other popups will be
    // closed too and app terminates. However when "window.close" is
    // executed in main window (and a popup browser lives) then
    // it will close browser, but it won't destroy main window,
    // leaving a gray main window (and a popup running in background).
    // So to resolve this, detect that browser embedded in main window
    // is closing and destroy its GTK window, so that app terminates.
    if (!browser->IsPopup() && browser_list_.size() > 1) {
        GtkWidget* main_window = get_main_window();
        if (main_window) {
            LOG(INFO) << "Force destroy GTK window in OnBeforeClose";
            gtk_widget_destroy(main_window);
        }
    }

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

// CefRequestHandler

bool ClientHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                                   const CefString& origin_url,
                                   int64 new_size,
                                   CefRefPtr<CefRequestCallback> callback) {
    CEF_REQUIRE_IO_THREAD();
    // Allow all requests
    callback->Continue(true);
    return true;
}
