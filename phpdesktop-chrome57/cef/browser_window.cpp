// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "browser_window.h"

#include <map>
#include <string>

#include "../executable.h"
#include "../log.h"
#include "../settings.h"
#include "../string_utils.h"
#include "../fatal_error.h"
#include "../file_utils.h"
#include "../window_utils.h"
#include "../resource.h"
#include "../web_server.h"

std::map<HWND, BrowserWindow*> g_browserWindows;
extern wchar_t g_windowClassName[256];
extern HINSTANCE g_hInstance; // main.cpp

BrowserWindow* GetBrowserWindow(HWND hwnd) {
    std::map<HWND, BrowserWindow*>::iterator it;
    it = g_browserWindows.find(hwnd);
    if (it != g_browserWindows.end()) {
        return it->second;
    }
    /*
    // Chrome 39 update - disabling code. Not sure why it
    // was needed earlier.
    // This would return parent browser for popup browsers,
    // which would not be an expected behavior. Commenting
    // out as it causes app hang when closing popup and then
    // main window. See RemoveBrowserWindow in OnBeforeClose
    // and WM_DESTROY.
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner) {
        // hwnd is CEF host handle.
        // This condition is for popups.
        it = g_browserWindows.find(owner);
        if (it != g_browserWindows.end()) {
            return it->second;
        }
    }
    */
    HWND parent = GetParent(hwnd);
    if (parent) {
        // This condition is for main window.
        it = g_browserWindows.find(parent);
        if (it != g_browserWindows.end()) {
            return it->second;
        }
    }
    // GetBrowserWindow() may fail during window creation, so log
    // severity is only DEBUG.
    LOG_DEBUG << "GetBrowserWindow(): not found, hwnd = " << (int)hwnd;
    return NULL;
}
void StoreBrowserWindow(HWND hwnd, BrowserWindow* browser) {
    LOG_DEBUG << "StoreBrowserWindow(): hwnd = " << (int)hwnd;
    std::map<HWND, BrowserWindow*>::iterator it;
    it = g_browserWindows.find(hwnd);
    if (it == g_browserWindows.end()) {
        g_browserWindows[hwnd] = browser;
    } else {
        LOG_WARNING << "StoreBrowserWindow() failed: already stored";
    }
}
void RemoveBrowserWindow(HWND hwnd) {
    LOG_DEBUG << "RemoveBrowserWindow(): hwnd = " << (int)hwnd;
    BrowserWindow* browser = GetBrowserWindow(hwnd);
    if (!browser) {
        LOG_WARNING << "RemoveBrowserWindow() failed: "
                    << "GetBrowserWindow() failed";
        return;
    }
    std::map<HWND, BrowserWindow*>::iterator it;
    it = g_browserWindows.find(browser->GetWindowHandle());
    if (it != g_browserWindows.end()) {
        // BrowserWindow* browser = it->second;
        g_browserWindows.erase(it);
        delete browser;
    } else {
        LOG_WARNING << "RemoveBrowserWindow() failed: not found";
    }
}

int CountBrowserWindows()
{
    return g_browserWindows.size();
}

BrowserWindow::BrowserWindow(HWND inWindowHandle, bool isPopup)
        : windowHandle_(inWindowHandle),
            isPopup_(isPopup),
            cefBrowser_(NULL),
            fullscreen_(NULL)
{
    _ASSERT(windowHandle_);

    SetTitleFromSettings();
    SetIconFromSettings();

    if (IsPopup()) {
        LOG_DEBUG << "BrowserWindow::BrowserWindow() created for Popup";
    } else {
        if (!CreateBrowserControl(Utf8ToWide(GetWebServerUrl()).c_str())) {
            FatalError(windowHandle_, "Could not create Browser control.\n"
                    "Exiting application.");
        }
    }
}
BrowserWindow::~BrowserWindow() {
}
CefRefPtr<CefBrowser> BrowserWindow::GetCefBrowser() {
    return cefBrowser_;
}
Fullscreen* BrowserWindow::GetFullscreenObject() {
    return fullscreen_.get();
}
void BrowserWindow::SetCefBrowser(CefRefPtr<CefBrowser> cefBrowser) {
    // Called from ClientHandler::OnAfterCreated().
    _ASSERT(!cefBrowser_);
    if (cefBrowser_) {
        LOG_ERROR << "BrowserWindow::SetCefBrowser() called, "
                  << "but it is already set";
        return;
    }
    cefBrowser_ = cefBrowser;
    fullscreen_.reset(new Fullscreen(cefBrowser));
    json_value* appSettings = GetApplicationSettings();
    if (!IsPopup()) {
        bool start_fullscreen = (*appSettings)["main_window"]["start_fullscreen"];
        if (start_fullscreen) {
            fullscreen_->ToggleFullscreen();
            CefRefPtr<CefProcessMessage> message = \
                    CefProcessMessage::Create("SetIsFullscreen");
            message->GetArgumentList()->SetBool(0, fullscreen_->IsFullscreen());
            cefBrowser->SendProcessMessage(PID_RENDERER, message);
        }
    }

    // OnSize was called from WM_SIZE, but cefBrowser_ was not yet
    // set, so the window wasn't yet positioned correctly.
    this->OnSize();
}
bool BrowserWindow::CreateBrowserControl(const wchar_t* navigateUrl) {
    LOG_DEBUG << "BrowserWindow::CreateBrowserControl()";
    // This is called only for the main window.
    // Popup cef browsers are created internally by CEF,
    // see OnBeforePopup, OnAfterCreated.
    RECT rect;
    BOOL b = GetWindowRect(windowHandle_, &rect);
    if (!b) {
        LOG_ERROR << "GetWindowRect() failed in "
                     "BrowserWindow::CreateBrowserControl()";
    }

    // Information used when creating the native window.
    CefWindowInfo window_info;
    window_info.SetAsChild(windowHandle_, rect);
    // SimpleHandler implements browser-level callbacks.
    CefRefPtr<ClientHandler> handler(new ClientHandler());
    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;
    // Create the first browser window.
    CefBrowserHost::CreateBrowser(
            window_info, handler.get(),
            GetWebServerUrl(), browser_settings, NULL);

    return true;
}
HWND BrowserWindow::GetWindowHandle() {
    _ASSERT(windowHandle_);
    return windowHandle_;
}
void BrowserWindow::SetTitle(const wchar_t* title) {
    BOOL b = SetWindowText(windowHandle_, title);
    _ASSERT(b);
}
bool BrowserWindow::IsPopup() {
    return isPopup_;
}
bool BrowserWindow::IsUsingMetaTitle() {
    if (IsPopup()) {
        json_value* settings = GetApplicationSettings();
        std::string fixed_title = (*settings)["popup_window"]["fixed_title"];
        return fixed_title.empty();
    }
    return false;
}
void BrowserWindow::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!IsPopup()) {
        json_value* settings = GetApplicationSettings();
        static long minimum_width =
                (*settings)["main_window"]["minimum_size"][0];
        static long minimum_height =
                (*settings)["main_window"]["minimum_size"][1];
        static long maximum_width =
                (*settings)["main_window"]["maximum_size"][0];
        static long maximum_height =
                (*settings)["main_window"]["maximum_size"][1];
        MINMAXINFO* pMMI = (MINMAXINFO*)lParam;
        if (minimum_width)
            pMMI->ptMinTrackSize.x = minimum_width;
        if (minimum_height)
            pMMI->ptMinTrackSize.y = minimum_height;
        if (maximum_width)
            pMMI->ptMaxTrackSize.x = maximum_width;
        if (maximum_height)
            pMMI->ptMaxTrackSize.y = maximum_height;
    }
}
void BrowserWindow::OnSize() {
    if (cefBrowser_) {
        HWND browserHandle = cefBrowser_->GetHost()->GetWindowHandle();
        _ASSERT(browserHandle);
        RECT rect;
        GetClientRect(windowHandle_, &rect);
        HDWP hdwp = BeginDeferWindowPos(2);
        hdwp = DeferWindowPos(hdwp, browserHandle, NULL,
                rect.left, rect.top,
                rect.right - rect.left,
                rect.bottom - rect.top,
                SWP_NOZORDER);
        EndDeferWindowPos(hdwp);
    } else {
        LOG_DEBUG << "BrowserWindow::OnSize(): "
                       "CefBrowser object not yet created";
    }
}
void BrowserWindow::SetTitleFromSettings() {
    if (IsPopup()) {
        json_value* settings = GetApplicationSettings();
        std::wstring popup_title = (*settings)["popup_window"]["fixed_title"];
        if (popup_title.empty())
            popup_title = (*settings)["main_window"]["title"];
        if (popup_title.empty())
            popup_title = Utf8ToWide(GetExecutableName());
        SetTitle(popup_title.c_str());
    }
    // Main window title is set in CreateMainWindow().
}
void BrowserWindow::SetIconFromSettings() {
    json_value* settings = GetApplicationSettings();
    const char* iconPath;
    if (IsPopup())
        iconPath = (*settings)["popup_window"]["icon"];
    else
        iconPath = (*settings)["main_window"]["icon"];
    if (iconPath && iconPath[0] != 0) {
        wchar_t iconPathW[MAX_PATH];
        Utf8ToWide(iconPath, iconPathW, _countof(iconPathW));
        int bigX = GetSystemMetrics(SM_CXICON);
        int bigY = GetSystemMetrics(SM_CYICON);
        HANDLE bigIcon = LoadImage(0, iconPathW, IMAGE_ICON, bigX, bigY,
                                   LR_LOADFROMFILE);
        if (bigIcon) {
            SendMessage(windowHandle_, WM_SETICON, ICON_BIG, (LPARAM)bigIcon);
        } else {
            LOG_WARNING << "Setting icon from settings file failed "
                           "(ICON_BIG)";
        }
        int smallX = GetSystemMetrics(SM_CXSMICON);
        int smallY = GetSystemMetrics(SM_CYSMICON);
        HANDLE smallIcon = LoadImage(0, iconPathW, IMAGE_ICON, smallX,
                                     smallY, LR_LOADFROMFILE);
        if (smallIcon) {
            SendMessage(windowHandle_, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);
        } else {
            LOG_WARNING << "Setting icon from settings file failed "
                           "(ICON_SMALL)";
        }
    } else if (IsPopup()) {
        // CEF did not set icon for the popup window, even though the opener
        // window had an icon set.
        HICON smallIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDR_MAINWINDOW));
        if (smallIcon) {
            SendMessage(windowHandle_, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);
        } else {
            LOG_WARNING << "LoadIcon(IDR_MAINWINDOW) failed "
                        << "in BrowserWindow::SetIconFromSettings()";
        }
    }
}
bool BrowserWindow::SetFocus() {
    // Calling SetFocus() on shellBrowser handle does not work.
    if (cefBrowser_) {
        cefBrowser_->GetHost()->SetFocus(true);
    }
    return true;
}
