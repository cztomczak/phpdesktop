// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "client_handler.h"

#include <sstream>
#include <string>

#include "util.h"
#include "include/cef_app.h"
#include "browser_window.h"
#include "devtools.h"

#include "../settings.h"
#include "../log.h"
#include "../executable.h"
#include "../string_utils.h"
#include "../dpi_aware.h"
#include "../window_utils.h"
#include "../web_server.h"

extern HINSTANCE g_hInstance;
extern wchar_t g_windowClassName[256];
extern std::map<HWND, BrowserWindow*> g_browserWindows; // browser_window.cpp

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

// ----------------------------------------------------------------------------
// CefClient methods
// ----------------------------------------------------------------------------

///
// Called when a new message is received from a different process. Return true
// if the message was handled or false otherwise. Do not keep a reference to
// or attempt to access the message outside of this callback.
///
/*--cef()--*/
bool ClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                    CefProcessId source_process,
                                    CefRefPtr<CefProcessMessage> message) {
    LOG_DEBUG << "browser[" << browser->GetIdentifier() << "] "
              << "OnProcessMessageReceived: " << message->GetName().ToString();
    if (message->GetName() == "ToggleFullscreen") {
        BrowserWindow* browserWindow = GetBrowserWindow(\
                browser->GetHost()->GetWindowHandle());
        if (browserWindow && browserWindow->GetFullscreenObject()) {
            browserWindow->GetFullscreenObject()->ToggleFullscreen();
        }
        return true;
    }
    LOG_ERROR << "Unhandled message in OnProcessMessageReceived";
    return false;
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
            if (cefTitle.empty() 
                    || cefTitle.ToString().find(GetWebServerIpAddress()) == 0) {
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
    LOG_DEBUG << "ClientHandler::OnAfterCreated()";
    json_value* appSettings = GetApplicationSettings();
    bool center_relative_to_parent = \
            (*appSettings)["popup_window"]["center_relative_to_parent"];
    HWND cefHandle = cefBrowser->GetHost()->GetWindowHandle();
    BrowserWindow* phpBrowser = GetBrowserWindow(cefHandle);
    if (phpBrowser) {
        // This block of code gets called for Main window & Devtools window.
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
        if (center_relative_to_parent) {
            // It would be best to provide x,y coordinates in OnBeforePopup,
            // but setting windowInfo.x and windowInfo.y there doesn't work.
            // These coordinates are probably used only when calling SetAsChild().
            // So we have to center it after the window was created.
            HWND openerHandle = cefBrowser->GetHost()->GetOpenerWindowHandle();
            BrowserWindow* openerPhpBrowser = GetBrowserWindow(openerHandle);
            if (openerPhpBrowser) {
                openerHandle = openerPhpBrowser->GetWindowHandle();
            }
            LOG_DEBUG << "Centering popup window relative to its parent";
            CenterWindowRelativeToParent(cefHandle, openerHandle);
        }
    }
    SetBrowserDpiSettings(cefBrowser);
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
    if (g_browserWindows.empty()) {
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
    LOG_DEBUG << "ClientHandler::OnBeforePopup()";
    // OnBeforePopup does not get called for the DevTools popup window.
    // The devtools window is created using CreatePopupWindow
    // ----
    // This popup window will be created internally by CEF.
    // Its parent hwnd won't be the current browser. To get
    // the browser that opened the popup call CefBrowserHost
    // GetOpenerWindowHandle().
    json_value* appSettings = GetApplicationSettings();
    bool external_navigation = (*appSettings)["chrome"]["external_navigation"];
    bool dpi_aware = (*appSettings)["application"]["dpi_aware"];
    // windowInfo.width and windowInfo.height will be set when there
    // was specified width/height for a popup. If it wasn't then these
    // values will be like 1073741824 (some garbage as memory wasn't set).
    // Although we can set it to something normal and it will be used
    // for the window size.
    int max_width = GetSystemMetrics(SM_CXMAXIMIZED);
    int max_height = GetSystemMetrics(SM_CYMAXIMIZED);
    if (windowInfo.width > max_width || windowInfo.height > max_height
            || windowInfo.width <= 0 || windowInfo.height <= 0) {
        // Use default size for a popup only when no size was provided.
        int default_width = static_cast<long>(\
                (*appSettings)["popup_window"]["default_size"][0]);
        int default_height = static_cast<long>(\
                (*appSettings)["popup_window"]["default_size"][1]);
        if (default_width && default_height) {
            LOG_INFO << "Setting default size for a popup window "
                     << default_width << "/" << default_height;
            windowInfo.width = default_width;
            windowInfo.height = default_height;
        }
    }
    if (dpi_aware) {
        GetDpiAwareWindowSize(&windowInfo.width, &windowInfo.height);
    }
    GetCorrectWindowSize(&windowInfo.width, &windowInfo.height);
    if (target_url.ToString().find(GetWebServerUrl()) == 0) {
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

#define MY_MENU_ID_DEVTOOLS MENU_ID_USER_FIRST + 1
#define MY_MENU_ID_RELOAD_PAGE MENU_ID_USER_FIRST + 2

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
    json_value* appSettings = GetApplicationSettings();
    bool reload_page_F5 = (*appSettings)["chrome"]["reload_page_F5"];
    bool devtools_F12 = (*appSettings)["chrome"]["devtools_F12"];

    model->Remove(MENU_ID_VIEW_SOURCE);
    if (!params->IsEditable() && params->GetSelectionText().empty()
            && (params->GetPageUrl().length()
                    || params->GetFrameUrl().length())) {
        if (reload_page_F5) {
            model->AddItem(MY_MENU_ID_RELOAD_PAGE, "Reload page");
        }
        if (devtools_F12) {
            model->AddSeparator();
            model->AddItem(MY_MENU_ID_DEVTOOLS, "Show Developer Tools");
        }
    }
}

///
// Called to execute a command selected from the context menu. Return true if
// the command was handled or false for the default implementation. See
// cef_menu_id_t for the command ids that have default implementations. All
// user-defined command ids should be between MENU_ID_USER_FIRST and
// MENU_ID_USER_LAST. |params| will have the same values as what was passed to
// OnBeforeContextMenu(). Do not keep a reference to |params| outside of this
// callback.
///
/*--cef()--*/
bool ClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefContextMenuParams> params,
                                int command_id,
                                EventFlags event_flags) {
    if (command_id == MY_MENU_ID_RELOAD_PAGE) {
        browser->ReloadIgnoreCache();
        return true;
    } else if (command_id == MY_MENU_ID_DEVTOOLS) {
        ShowDevTools(browser);
        return true;
    }
    return false;
}

///
// Called when the context menu is dismissed irregardless of whether the menu
// was empty or a command was selected.
///
/*--cef()--*/
void ClientHandler::OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame) {
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
    if (newUrl.ToString().find(GetWebServerUrl()) == 0) {
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

// ----------------------------------------------------------------------------
// CefKeyboardHandler methods
// ----------------------------------------------------------------------------

///
// Called after the renderer and JavaScript in the page has had a chance to
// handle the event. |event| contains information about the keyboard event.
// |os_event| is the operating system event message, if any. Return true if
// the keyboard event was handled or false otherwise.
///
/*--cef()--*/
bool ClientHandler::OnKeyEvent(CefRefPtr<CefBrowser> cefBrowser,
                        const CefKeyEvent& event,
                        CefEventHandle os_event) {
    REQUIRE_UI_THREAD();

    json_value* appSettings = GetApplicationSettings();
    bool reload_page_F5 = (*appSettings)["chrome"]["reload_page_F5"];
    bool devtools_F12 = (*appSettings)["chrome"]["devtools_F12"];

    if (reload_page_F5 && event.windows_key_code == VK_F5
            && event.type == KEYEVENT_RAWKEYDOWN) {
        LOG_DEBUG << "F5 pressed, reloading page";
        cefBrowser->ReloadIgnoreCache();
        return false;
    } else if (devtools_F12 && event.windows_key_code == VK_F12
            && event.type == KEYEVENT_RAWKEYDOWN) {
        LOG_DEBUG << "F12 pressed, opening developer tools";
        if (!ShowDevTools(cefBrowser)) {
            return false;
        }
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// CefDownloadHandler methods
// ----------------------------------------------------------------------------

///
// Called before a download begins. |suggested_name| is the suggested name for
// the download file. By default the download will be canceled. Execute
// |callback| either asynchronously or in this method to continue the download
// if desired. Do not keep a reference to |download_item| outside of this
// method.
///
/*--cef()--*/
void ClientHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefDownloadItem> download_item,
                        const CefString& suggested_name,
                        CefRefPtr<CefBeforeDownloadCallback> callback) {
    json_value* appSettings = GetApplicationSettings();
    bool enable_downloads = (*appSettings)["chrome"]["enable_downloads"];
    if (enable_downloads) {
        LOG_INFO << "About to download a file: " << suggested_name.ToString();
        callback->Continue(suggested_name, true);
    } else {
        LOG_INFO << "Tried to download a file, but downloads are disabled";
    }
}

///
// Called when a download's status or progress information has been updated.
// This may be called multiple times before and after OnBeforeDownload().
// Execute |callback| either asynchronously or in this method to cancel the
// download if desired. Do not keep a reference to |download_item| outside of
// this method.
///
/*--cef()--*/
void ClientHandler::OnDownloadUpdated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        CefRefPtr<CefDownloadItemCallback> callback) {
    if (download_item->IsComplete()) {
        LOG_INFO << "Download completed, saved to: " << download_item->GetFullPath().ToString();
    } else if (download_item->IsCanceled()) {
        LOG_INFO << "Download was cancelled";
    }
}
