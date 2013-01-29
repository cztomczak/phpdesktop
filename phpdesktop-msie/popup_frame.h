// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#define CLICK_EVENTS_TIMER 1

extern CAppModule g_appModule;

#include "msie/browser_frame.h"
#include "settings.h"
#include "string_utils.h"
#include "web_server.h"

class PopupView : public CWindowImpl<PopupView, CAxWindow> {
  public:
    DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())    
    BOOL PreTranslateMessage(MSG* pMsg) {
        if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
           (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
            return FALSE;
        return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
    }
    BEGIN_MSG_MAP(PopupView)
    END_MSG_MAP()
};

class PopupFrame :
    public CFrameWindowImpl<PopupFrame>,
    public CUpdateUI<PopupFrame>,
    public CMessageFilter,
    public CIdleHandler,
    public BrowserFrame<PopupFrame> {
  public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_POPUPFRAME)
    PopupView topView_;
    PopupFrame() {}

    virtual BOOL PreTranslateMessage(MSG* pMsg) {
        if(CFrameWindowImpl<PopupFrame>::PreTranslateMessage(pMsg))
            return TRUE;
        return topView_.PreTranslateMessage(pMsg);
    }
    virtual BOOL OnIdle() {
        return FALSE;
    }

    BEGIN_UPDATE_UI_MAP(PopupFrame)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(PopupFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        CHAIN_MSG_MAP(CUpdateUI<PopupFrame>)
        CHAIN_MSG_MAP(CFrameWindowImpl<PopupFrame>)
    END_MSG_MAP()

    void SetIconFromSettings() {
        json_value* settings = GetApplicationSettings();
        const char* iconPath = (*settings)["popup_window"]["icon"];
        if (iconPath && iconPath[0] != 0) {
            wchar_t iconPathW[MAX_PATH];
            Utf8ToWide(iconPath, iconPathW, _countof(iconPathW));

            int bigX = GetSystemMetrics(SM_CXICON);
            int bigY = GetSystemMetrics(SM_CYICON);
            HANDLE bigIcon = LoadImage(0, iconPathW, IMAGE_ICON, bigX, bigY, 
                                       LR_LOADFROMFILE);
            if (bigIcon)
                SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)bigIcon);
            else
                LOG(logWARNING) << "Setting icon from settings file failed "
                                   "(ICON_BIG)";
            int smallX = GetSystemMetrics(SM_CXSMICON);
            int smallY = GetSystemMetrics(SM_CYSMICON);
            HANDLE smallIcon = LoadImage(0, iconPathW, IMAGE_ICON, smallX, 
                                         smallY, LR_LOADFROMFILE);
            if (smallIcon)
                SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);
            else
                LOG(logWARNING) << "Setting icon from settings file failed "
                                   "(ICON_SMALL)";
        }
    }
    void SetTitleFromSettings() {
        // Popup title.
        json_value* settings = GetApplicationSettings();
        std::wstring popup_title = (*settings)["popup_window"]["fixed_title"];
        if (popup_title.empty())
            popup_title = (*settings)["main_window"]["title"];
        if (popup_title.empty())
            popup_title = Utf8ToWide(GetExecutableName());
        SetTitle(popup_title.c_str());
    }
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                     BOOL& /*bHandled*/) {
        SetIconFromSettings();
        SetTitleFromSettings();
        if (!CreateBrowser(L"about:blank")) {
            LOG(logERROR) << "PopupFrame::OnCreate() failed: "
                    "BrowserFrame::CreateBrowser() failed";
            SendMessage(WM_CLOSE, 0, 0);
            return 0;
        }
        SetTimer(CLICK_EVENTS_TIMER, 10, NULL);
        SetAllowedUrl(Utf8ToWide(g_webServerUrl).c_str());

        CMessageLoop* pLoop = g_appModule.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
        // pLoop->AddIdleHandler(this);
        return 0;
    }
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        // Need to re-attach after each browser navigation.
        this->AttachClickEvents();
        return 0;
    }
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
                    BOOL& bHandled) {
        CloseBrowser();
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                      BOOL& bHandled) {
        CMessageLoop* pLoop = g_appModule.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
        // pLoop->RemoveIdleHandler(this);
        // When bHandled is false return value doesn't matter.
        // WM_DESTROY: should return zero if it processes this message.        
        bHandled = FALSE;
        return 0;
    }
    virtual void OnFinalMessage(HWND hwnd) {
        // PopupFrame() is created using "new", cleaning up memory.
		delete this;
	}
    bool IsPopup() {
        return true;
    }
    bool IsUsingMetaTitle() {
        json_value* settings = GetApplicationSettings();
        std::string fixed_title = (*settings)["popup_window"]["fixed_title"];
        return fixed_title.empty();
    }
};
