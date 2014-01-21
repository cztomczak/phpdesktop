// Copyright (c) 2012-2014 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "client_handler.h"

#include <sstream>
#include <string>

#include "util.h"
#include "include/cef_app.h"
#include "include/cef_runnable.h"

#include "browser_window.h"
#include "../window_utils.h"
#include "../settings.h"
#include "../log.h"
#include "../executable.h"

extern HINSTANCE g_hInstance;
extern wchar_t g_windowClassName[256];
extern std::string g_webServerUrl;

namespace {

ClientHandler* g_instance = NULL;

}  // namespace

ClientHandler::ClientHandler() {
    ASSERT(!g_instance);
    g_instance = this;
}

ClientHandler::~ClientHandler() {
    g_instance = NULL;
}

// static
ClientHandler* ClientHandler::GetInstance() {
    return g_instance;
}

void SetBrowserDpiSettings(CefRefPtr<CefBrowser> cefBrowser) {
    // Setting zoom level immediately after browser was created
    // won't work. We need to wait a moment before we can set it.
    REQUIRE_UI_THREAD();

    json_value* appSettings = GetApplicationSettings();
    if (!(*appSettings)["application"]["dpi_aware"]) {
        return;
    }

    double oldZoomLevel = cefBrowser->GetHost()->GetZoomLevel();
    double newZoomLevel = 0.0;

    // Win7:
    // text size Larger 150% => ppix/ppiy 144
    // text size Medium 125% => ppix/ppiy 120
    // text size Smaller 100% => ppix/ppiy 96
    HWND cefHandle = cefBrowser->GetHost()->GetWindowHandle();
    HDC hdc = GetDC(cefHandle);
    int ppix = GetDeviceCaps(hdc, LOGPIXELSX);
    int ppiy = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(cefHandle, hdc);

    if (ppix > 96) {
        newZoomLevel = (ppix - 96) / 24;
    }

    if (oldZoomLevel != newZoomLevel) {
        cefBrowser->GetHost()->SetZoomLevel(newZoomLevel);
        if (cefBrowser->GetHost()->GetZoomLevel() != oldZoomLevel) {
            // Succes.
            LOG_DEBUG << "DPI, ppix = " << ppix << ", ppiy = " << ppiy;
            LOG_DEBUG << "DPI, browser zoom level = "
                      << cefBrowser->GetHost()->GetZoomLevel();
        }
    } else {
        // This code block running can also be a result of executing
        // SetZoomLevel(), as GetZoomLevel() didn't return the new
        // value that was set. Documentation says that if SetZoomLevel
        // is called on the UI thread, then GetZoomLevel should
        // immediately return the same value that was set. Unfortunately
        // this seems not to be true.
        static bool already_logged = false;
        if (!already_logged) {
            already_logged = true;
            // Success.
            LOG_DEBUG << "DPI, ppix = " << ppix << ", ppiy = " << ppiy;
            LOG_DEBUG << "DPI, browser zoom level = "
                      << cefBrowser->GetHost()->GetZoomLevel();
        }
    }
    // We need to check zooming constantly, during loading of pages.
    // If we set zooming to 2.0 for localhost/ and then it navigates
    // to google.com, then the zomming is back at 0.0 and needs to
    // be set again.
    CefPostDelayedTask(
            TID_UI,
            NewCefRunnableFunction(&SetBrowserDpiSettings, cefBrowser),
            50);
}

HWND CreatePopupWindow(HWND parentHandle) {
    json_value* appSettings = GetApplicationSettings();
    bool center_relative_to_parent =
            (*appSettings)["popup_window"]["center_relative_to_parent"];

    // Title will be set in BrowserWindow::BrowserWindow().
    // CW_USEDEFAULT cannot be used with WS_POPUP.
    HWND hwnd = CreateWindowEx(0, g_windowClassName,
            0, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            parentHandle, 0, g_hInstance, 0);
    _ASSERT(hwnd);
    if (center_relative_to_parent) {
        // This won't work properly as real width/height is set later
        // when BrowserEvents2::WindowSetWidth() / WindowSetHeight()
        // are triggered. TODO.
        // CenterWindow(hwnd);
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
    return hwnd;
}

// ----------------------------------------------------------------------------
// CefDisplayHandler methods
// ----------------------------------------------------------------------------

///
// Called when the page title changes.
///
void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> cefBrowser,
                                  const CefString& cefTitle) {
    REQUIRE_UI_THREAD();
    json_value* appSettings = GetApplicationSettings();
    HWND cefHandle = cefBrowser->GetHost()->GetWindowHandle();
    BrowserWindow* browser = GetBrowserWindow(cefHandle);
    if (browser && browser->IsPopup()) {
        if (browser->IsUsingMetaTitle()) {
            std::string ipAddress = (*appSettings)["web_server"]["listen_on"][0];
            if (cefTitle.empty() || cefTitle.ToString().find(ipAddress) == 0) {
                // Use main window title if no title provided in popup.
                // If there is not meta title, then CEF sets url as a title.
                std::string main_window_title = (*appSettings)["main_window"]["title"];
                if (main_window_title.empty())
                    main_window_title = GetExecutableName();
                browser->SetTitle(Utf8ToWide(main_window_title).c_str());
            } else {
                browser->SetTitle(std::wstring(cefTitle).c_str());
            }
        } else {
            browser->SetTitleFromSettings();
        }
    }
}

// ----------------------------------------------------------------------------
// CefLifeSpanHandler methods
// ----------------------------------------------------------------------------

///
// Called after a new browser is created.
///
void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> cefBrowser) {
    REQUIRE_UI_THREAD();

    HWND cefHandle = cefBrowser->GetHost()->GetWindowHandle();
    BrowserWindow* phpBrowser = GetBrowserWindow(cefHandle);
    if (phpBrowser) {
        ASSERT(!phpBrowser->GetCefBrowser().get());
        if (!phpBrowser->GetCefBrowser().get()) {
            LOG_DEBUG << "SetCefBrowser() called in "
                            "ClientHandler::OnAfterCreated()";
            phpBrowser->SetCefBrowser(cefBrowser);
        }
    } else {
        // CEF popup browser.
        BrowserWindow* phpBrowser = new BrowserWindow(cefHandle, true);
        StoreBrowserWindow(cefHandle, phpBrowser);
        phpBrowser->SetCefBrowser(cefBrowser);
        phpBrowser->SetIconFromSettings();
        phpBrowser->SetTitleFromSettings();
        phpBrowser->SetFocus();
    }

    SetBrowserDpiSettings(cefBrowser);

    // Add to the list of existing browsers.
    browser_list_.push_back(cefBrowser);
}

///
// Called just before a browser is destroyed. Release all references to the
// browser object and do not attempt to execute any methods on the browser
// object after this callback returns. If this is a modal window and a custom
// modal loop implementation was provided in RunModal() this callback should
// be used to exit the custom modal loop. See DoClose() documentation for
// additional usage information.
///
void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    REQUIRE_UI_THREAD();

    RemoveBrowserWindow(browser->GetHost()->GetWindowHandle());

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

///
// Called on the IO thread before a new popup window is created. The |browser|
// and |frame| parameters represent the source of the popup request. The
// |target_url| and |target_frame_name| values may be empty if none were
// specified with the request. The |popupFeatures| structure contains
// information about the requested popup window. To allow creation of the
// popup window optionally modify |windowInfo|, |client|, |settings| and
// |no_javascript_access| and return false. To cancel creation of the popup
// window return true. The |client| and |settings| values will default to the
// source browser's values. The |no_javascript_access| value indicates whether
// the new browser window should be scriptable and in the same process as the
// source browser.
/*--cef(optional_param=target_url,optional_param=target_frame_name)--*/
bool ClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            const CefString& target_url,
                            const CefString& target_frame_name,
                            const CefPopupFeatures& popupFeatures,
                            CefWindowInfo& windowInfo,
                            CefRefPtr<CefClient>& client,
                            CefBrowserSettings& settings,
                            bool* no_javascript_access) {
    json_value* appSettings = GetApplicationSettings();
    bool external_navigation = (*appSettings)["chrome"]["external_navigation"];
    if (target_url.ToString().find(g_webServerUrl) == 0) {
        // Allow to create.
        return false; 
    } else {
        if (external_navigation) {
            // Allow to create.
            return false; 
        } else {
            // Open in system default browser.
            ShellExecute(0, L"open", target_url.ToWString().c_str(), 0, 0, SW_SHOWNORMAL);
            return true;
        }
    }
}

// ----------------------------------------------------------------------------
// CefLoadHandler methods
// ----------------------------------------------------------------------------

///
// Called when the resource load for a navigation fails or is canceled.
// |errorCode| is the error code number, |errorText| is the error text and
// |failedUrl| is the URL that failed to load. See net\base\net_error_list.h
// for complete descriptions of the error codes.
///
void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
    REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
            "<h2>Failed to load URL " << std::string(failedUrl) <<
            " with error " << std::string(errorText) << " (" << errorCode <<
            ").</h2></body></html>";
    frame->LoadString(ss.str(), failedUrl);
}

///
// Called when the loading state has changed. This callback will be executed
// twice -- once when loading is initiated either programmatically or by user
// action, and once when loading is terminated due to completion, cancellation
// of failure.
///
/*--cef()--*/
void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> cefBrowser,
                                bool isLoading,
                                bool canGoBack,
                                bool canGoForward) {
}

// ----------------------------------------------------------------------------
// CefContextMenuHandler methods
// ----------------------------------------------------------------------------

///
// Called before a context menu is displayed. |params| provides information
// about the context menu state. |model| initially contains the default
// context menu. The |model| can be cleared to show no context menu or
// modified to show a custom menu. Do not keep references to |params| or
// |model| outside of this callback.
///
/*--cef()--*/
void ClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefContextMenuParams> params,
                                CefRefPtr<CefMenuModel> model) {
    model->Remove(MENU_ID_VIEW_SOURCE);
}

// ----------------------------------------------------------------------------
// CefDragHandler methods
// ----------------------------------------------------------------------------

///
// Called when an external drag event enters the browser window. |dragData|
// contains the drag event data and |mask| represents the type of drag
// operation. Return false for default drag handling behavior or true to
// cancel the drag event.
///
bool ClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefDragData> dragData,
                       DragOperationsMask mask) {
    json_value* appSettings = GetApplicationSettings();
    bool external_drag = (*appSettings)["chrome"]["external_drag"];
    if (external_drag) {
        return false;
    } else {
        return true;
    }
}

// ----------------------------------------------------------------------------
// CefRequestHandler methods
// ----------------------------------------------------------------------------

///
// Called on the UI thread before browser navigation. Return true to cancel
// the navigation or false to allow the navigation to proceed. The |request|
// object cannot be modified in this callback.
// CefLoadHandler::OnLoadingStateChange will be called twice in all cases.
// If the navigation is allowed CefLoadHandler::OnLoadStart and
// CefLoadHandler::OnLoadEnd will be called. If the navigation is canceled
// CefLoadHandler::OnLoadError will be called with an |errorCode| value of
// ERR_ABORTED.
///
/*--cef()--*/
bool ClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            bool is_redirect) {
    REQUIRE_UI_THREAD();
    // See also OnBeforePopup.
    json_value* appSettings = GetApplicationSettings();
    bool external_navigation = (*appSettings)["chrome"]["external_navigation"];
    CefString newUrl = request->GetURL();
    if (newUrl.ToString().find(g_webServerUrl) == 0) {
        // Allow to open in phpdesktop browser.
        return false;
    } else {
        if (external_navigation) {
            // Allow to open in phpdesktop browser.
            return false;
        } else {
            // Open in system default browser.
            ShellExecute(0, L"open", newUrl.ToWString().c_str(), 0, 0, SW_SHOWNORMAL);
            return true;
        }
    }
}