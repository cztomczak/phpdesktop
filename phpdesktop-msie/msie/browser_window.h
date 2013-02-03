// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <crtdbg.h> // _ASSERT macro.
#include <windows.h>
#include <ExDisp.h>
#include <memory> // shared_ptr

#include "com_utils.h"
#include "click_events.h"
#include "ole_client_site.h"
#include "external_dispatch.h"

class BrowserWindow;
BrowserWindow* GetBrowserWindow(HWND hwnd);
void StoreBrowserWindow(HWND hwnd, BrowserWindow* browser);
void RemoveBrowserWindow(HWND hwnd);

class BrowserWindow {
private:
    HWND windowHandle_;
    HWND parentHandle_;
    std::tr1::shared_ptr<OleClientSite> oleClientSite_;
    std::tr1::shared_ptr<ExternalDispatch> externalDispatch_;
    std::tr1::shared_ptr<ClickEvents> clickEvents_;
    bool clickEventsAttached_;
    _bstr_t documentUniqueID_;
    _variant_t clickDispatch_;    
    wchar_t allowedUrl_[2084];
    IWebBrowser2Ptr webBrowser2_;
    IOleObjectPtr oleObject_;
    IOleInPlaceActiveObjectPtr oleInPlaceActiveObject_;
    DWORD dWebBrowserEvents2Cookie_;
    bool isResizing_;
    bool focusedAfterCreation_;
public:
    BrowserWindow(HWND inWindowHandle);
    ~BrowserWindow();
    bool CreateBrowserControl(const wchar_t* navigateUrl);
    void CloseBrowserControl();
    bool DetachClickEvents();
    bool TryAttachClickEvents();
    bool TrySetFocusAfterCreation();
    bool AdviseEvent(IWebBrowser2Ptr webBrowser2, REFIID riid,
                     DWORD* adviseCookie);
    bool UnadviseEvent(IWebBrowser2Ptr webBrowser2, REFIID riid,
                     DWORD adviseCookie);
    HWND GetWindowHandle();
    IOleClientSite* GetOleClientSite();
    const IWebBrowser2Ptr GetWebBrowser2();
    bool GetActiveHtmlElement(wchar_t* outTag, int outTagSize,
                              wchar_t* outType, int outTypeSize);
    int ExternalIdentifier(wchar_t* function);
    bool ExternalCall(int functionIdentifier);
    void SetAllowedUrl(const wchar_t* inUrl);
    bool IsUrlAllowed(wchar_t* inUrl, int sizeInWords);
    void SetWidth(long width);
    void SetHeight(long height);
    void SetTop(long top);
    void SetLeft(long left);
    void SetTitle(const wchar_t* title);
    bool IsPopup();
    bool IsUsingMetaTitle();
    void OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void SetTitleFromSettings();
    void SetIconFromSettings();
    HWND GetShellBrowserHandle();
    bool SetFocus();
    bool TranslateAccelerator(MSG* msg);
};


