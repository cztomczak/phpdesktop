// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#define CLICK_EVENTS_TIMER 1

extern CAppModule g_appModule;
extern wchar_t* g_singleInstanceApplicationGuid;

#include "msie/browser_frame.h"
#include "msie/fullscreen_frame.h"
#include "string_utils.h"
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
    DECLARE_FRAME_WND_CLASS(g_singleInstanceApplicationGuid, IDR_MAINFRAME)
    
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

    void SetIconFromSettings() {
        json_value* settings = GetApplicationSettings();
        const char* iconPath = (*settings)["main_window"]["icon"];
        if (iconPath && iconPath[0] != 0) {
            wchar_t iconPathW[4096];
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
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                     BOOL& /*bHandled*/) {
        SetIconFromSettings();

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

        json_value* settings = GetApplicationSettings();
        static long minimum_width = 
                (*settings)["main_window"]["minimum_size"][0];
        static long minimum_height = 
                (*settings)["main_window"]["minimum_size"][1];
        static long maximum_width = 
                (*settings)["main_window"]["maximum_size"][0];
        static long maximum_height = 
                (*settings)["main_window"]["maximum_size"][1];

        if (minimum_width)
            lpMMI->ptMinTrackSize.x = minimum_width;
        if (minimum_height)
            lpMMI->ptMinTrackSize.y = minimum_height;

        if (maximum_width)        
            lpMMI->ptMaxTrackSize.x = maximum_width;
        if (maximum_height)
            lpMMI->ptMaxTrackSize.y = maximum_height;

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
