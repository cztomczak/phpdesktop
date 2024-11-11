// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "devtools.h"
#include "browser_window.h"

#include "../logger.h"
#include "../string_utils.h"
#include "../popup_window.h"
#include "../settings.h"

bool ShowDevTools(CefRefPtr<CefBrowser> browser)
{
    LOGGER_DEBUG << "Show DevTools";

    json_value* settings = GetApplicationSettings();
    std::string runtime_style = (*settings)["chrome"]["runtime_style"];

    CefWindowInfo window_info;
    CefBrowserSettings browser_settings;

    if (runtime_style == "alloy") {
        LOGGER_INFO << "Runtime style: alloy";
        window_info.runtime_style = CEF_RUNTIME_STYLE_ALLOY;
    } else if (runtime_style == "chrome") {
        LOGGER_INFO << "Runtime style: chrome";
        window_info.runtime_style = CEF_RUNTIME_STYLE_CHROME;
    } else {
        LOGGER_INFO << "Invalid runtime style in settings.json: " << runtime_style;
        window_info.runtime_style = CEF_RUNTIME_STYLE_ALLOY;
    }

    HWND popup_handle = CreatePopupWindow(browser->GetHost()->GetWindowHandle());
    if (!popup_handle) {
        LOGGER_ERROR << "Failed to create popup window";
        return false;
    }
    window_info.SetAsPopup(popup_handle, "DevTools");

    browser->GetHost()->ShowDevTools(window_info, nullptr,
                                     browser_settings, CefPoint());

    BrowserWindow* browser_window = GetBrowserWindow(popup_handle);
    if (browser_window) {
        browser_window->SetTitle(L"DevTools");
    }

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
    LOGGER_INFO << "DevTools url: " << devtools_url;
    if (devtools_url.empty()) {
        LOGGER_WARNING << "GetDevToolsURL() returned an empty string. "
                        "Make sure you've set the remote-debugging-port switch";
        return false;
    }
    CefWindowInfo windowInfo;
    CefBrowserSettings browser_settings;
    BrowserWindow* phpBrowser = GetBrowserWindow(cefBrowser->GetHost()->GetWindowHandle());
    if (!phpBrowser) {
        LOGGER_ERROR << "GetBrowserWindow() failed in ClientHandler::OnKeyEvent";
        return false;
    }
    HWND popupHandle = CreatePopupWindow(phpBrowser->GetWindowHandle());
    if (!popupHandle) {
        LOGGER_ERROR << "CreatePopupWindow() failed in ClientHandler::OnKeyEvent";
        return false;
    }
    RECT rect;
    GetWindowRect(popupHandle, &rect);
    windowInfo.SetAsChild(popupHandle, rect);
    bool created = CefBrowserHost::CreateBrowser(
            windowInfo, cefBrowser->GetHost()->GetClient(), devtools_url,
            browser_settings, NULL);
    if (!created) {
        LOGGER_ERROR << "CreateBrowser() failed in ClientHandler::OnKeyEvent";
        return false;
    }
    return true;
}
*/
