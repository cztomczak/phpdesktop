// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#define CLICK_EVENTS_TIMER 1

extern CAppModule g_appModule;

#include "msie/browser_frame.h"
#include "msie/fullscreen_frame.h"
#include "web_server.h"

class MainView : public CWindowImpl<MainView, CAxWindow> {
  public:
    DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())
    
    BOOL PreTranslateMessage(MSG* pMsg) {
        if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
           (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
            return FALSE;
        return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
    }

    BEGIN_MSG_MAP(MainView)
    END_MSG_MAP()
};

class MainFrame :
    public CFrameWindowImpl<MainFrame>,
    public CUpdateUI<MainFrame>,
    public CMessageFilter,
    public CIdleHandler,
    public BrowserFrame<MainFrame>,
    public FullscreenFrame<MainFrame> {
  public:
    DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
    
    MainView rootView;
    MainFrame() {}

    virtual BOOL PreTranslateMessage(MSG* pMsg) {
        if(CFrameWindowImpl<MainFrame>::PreTranslateMessage(pMsg))
            return TRUE;
        return rootView.PreTranslateMessage(pMsg);
    }
    virtual BOOL OnIdle() {
        return FALSE;
    }

    BEGIN_UPDATE_UI_MAP(MainFrame)
    END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP(MainFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
        CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                     BOOL& /*bHandled*/) {
        CreateBrowser(L"http://127.0.0.1:54007/");
        // CreateBrowser(L"c:\\phpdesktop\\phpdesktop-src\\phpdesktop-msie\\Release\\www\\test.html");

        this->SetTimer(CLICK_EVENTS_TIMER, 10, NULL);
        this->SetAllowedURL(L"http://127.0.0.1:54007/");

        CMessageLoop* pLoop = g_appModule.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
        pLoop->AddIdleHandler(this);
        return 0;
    }
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        // Need to re-attach after each browser navigation.
        this->AttachClickEvents();
        return 0;
    }
    LRESULT OnGetMinMaxInfo(UINT, WPARAM, LPARAM lParam, BOOL&) {
        // Load size structure with lParam values
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

        // Change the values in the size structure to desired values
        lpMMI->ptMinTrackSize.x = 800; // min width
        lpMMI->ptMinTrackSize.y = 600; // min height
        
        // lpMMI->ptMaxTrackSize.x = 0; // max width, cannot set to 0!
        // lpMMI->ptMaxTrackSize.y = 0; // max height

        return 0;
    }
    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
                    BOOL& bHandled) {
        TerminateWebServer();
        if (isfullscreen) {
            ShowTaskBar(true);
        }
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                      BOOL& bHandled) {
        CMessageLoop* pLoop = g_appModule.GetMessageLoop();
        ATLASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
        pLoop->RemoveIdleHandler(this);
        bHandled = FALSE;
        return 1;
    }
};
