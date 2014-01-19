// Copyright (c) 2012-2014 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <crtdbg.h> // _ASSERT macro.
#include <windows.h>
#include <ExDisp.h>
#include <memory> // shared_ptr

#include "app.h"
#include "client_handler.h"

class BrowserWindow;
BrowserWindow* GetBrowserWindow(HWND hwnd);
void StoreBrowserWindow(HWND hwnd, BrowserWindow* browser);
void RemoveBrowserWindow(HWND hwnd);

class BrowserWindow {
private:
    HWND windowHandle_;
    bool isPopup_;
	CefRefPtr<CefBrowser> cefBrowser_; // may be empty, always check using .get()
public:
    BrowserWindow(HWND inWindowHandle, bool isPopup);
    ~BrowserWindow();
    CefRefPtr<CefBrowser> GetCefBrowser();
    void SetCefBrowser(CefRefPtr<CefBrowser> cefBrowser);
    bool CreateBrowserControl(const wchar_t* navigateUrl);
    void CloseBrowserControl();
    HWND GetWindowHandle();
    void SetTitle(const wchar_t* title);
    bool IsPopup();
    bool IsUsingMetaTitle();
    void OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSize();
    void SetTitleFromSettings();
    void SetIconFromSettings();
    bool SetFocus();
};
