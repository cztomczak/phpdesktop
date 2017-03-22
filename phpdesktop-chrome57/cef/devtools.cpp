// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "devtools.h"
#include "browser_window.h"

#include "../log.h"
#include "../string_utils.h"
#include "../popup_window.h"

bool ShowDevTools(CefRefPtr<CefBrowser> browser) {
    CefWindowInfo windowInfo;
    CefBrowserSettings settings;

    #if defined(OS_WIN)
    windowInfo.SetAsPopup(browser->GetHost()->GetWindowHandle(), "DevTools");
    #endif

    browser->GetHost()->ShowDevTools(windowInfo, browser->GetHost()->GetClient(),
                                     settings, CefPoint());
    return true;
}

/*
bool ShowDevTools(CefRefPtr<CefBrowser> cefBrowser) {
    std::string devtools_url = cefBrowser->GetHost()->GetDevToolsURL(true);
    // Example url returned:
    //     http://localhost:54008/devtools/devtools.html?ws=localhost:54008
    //     /devtools/page/1538ed984a2a4a90e5ed941c7d142a12
    // Let's replace "localhost" with "127.0.0.1", using the ip address
    // is more reliable.
    devtools_url = ReplaceString(devtools_url, "localhost:", "127.0.0.1:");
    LOG_INFO << "DevTools url: " << devtools_url;
    if (devtools_url.empty()) {
        LOG_WARNING << "GetDevToolsURL() returned an empty string. "
                        "Make sure you've set the remote-debugging-port switch";
        return false;
    }
    CefWindowInfo windowInfo;
    CefBrowserSettings browser_settings;
    BrowserWindow* phpBrowser = GetBrowserWindow(cefBrowser->GetHost()->GetWindowHandle());
    if (!phpBrowser) {
        LOG_ERROR << "GetBrowserWindow() failed in ClientHandler::OnKeyEvent";
        return false;
    }
    HWND popupHandle = CreatePopupWindow(phpBrowser->GetWindowHandle());
    if (!popupHandle) {
        LOG_ERROR << "CreatePopupWindow() failed in ClientHandler::OnKeyEvent";
        return false;
    }
    RECT rect;
    GetWindowRect(popupHandle, &rect);
    windowInfo.SetAsChild(popupHandle, rect);
    bool created = CefBrowserHost::CreateBrowser(
            windowInfo, cefBrowser->GetHost()->GetClient(), devtools_url,
            browser_settings, NULL);
    if (!created) {
        LOG_ERROR << "CreateBrowser() failed in ClientHandler::OnKeyEvent";
        return false;
    }
    return true;
}
*/
