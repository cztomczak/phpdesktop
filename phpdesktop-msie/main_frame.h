// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#define CLICK_EVENTS_TIMER 1

extern CAppModule g_appModule;
extern wchar_t* g_singleInstanceApplicationGuid;
extern std::string g_webServerUrl;

#include <string>

#include "fatal_error.h"
#include "msie/browser_frame.h"
#include "popup_frame.h"
#include "settings.h"
#include "string_utils.h"
#include "web_server.h"

class MainFrame :
    public CFrameWindowImpl<MainFrame>,
    //public CUpdateUI<MainFrame>,
    public CMessageFilter,
    //public CIdleHandler,
    public BrowserFrame<MainFrame> {
public:
    DECLARE_FRAME_WND_CLASS(g_singleInstanceApplicationGuid, IDR_MAINFRAME)
    MainFrame() {}

    //BEGIN_UPDATE_UI_MAP(MainFrame)
    //END_UPDATE_UI_MAP()

    virtual BOOL PreTranslateMessage(MSG* msg) {
        if(CFrameWindowImpl<MainFrame>::PreTranslateMessage(msg))
            return TRUE;
        if (0 && msg->message == WM_KEYDOWN) {
            CComQIPtr<IWebBrowser2> browser = GetBrowser();
            if (1 && browser) {
                CComQIPtr<IOleInPlaceActiveObject> activeObject;
                HRESULT hr = browser->QueryInterface(IID_IOleInPlaceActiveObject, 
                                                     (void**)&activeObject);
                if (SUCCEEDED(hr)) {
                    hr = activeObject->TranslateAccelerator(msg);
                    if (SUCCEEDED(hr))
                        return TRUE;
                    else
                        return FALSE;
                } else {
                    static bool logged = false;
                    if (!logged) {
                        LOG(logWARNING) << "MainFrame::PreTranslateMessage() failed: "
                                "QueryInterface(IOleInPlaceActiveObject) failed";
                        logged = true;
                    }
                    return FALSE;
                }
            }
        }
        return FALSE;
    }

    BEGIN_MSG_MAP(MainFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        //CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
        CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
    END_MSG_MAP()

    void SetIconFromSettings() {
        json_value* settings = GetApplicationSettings();
        const char* iconPath = (*settings)["main_window"]["icon"];
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
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                     BOOL& /*bHandled*/) {
        SetIconFromSettings();
        if (!CreateBrowser(Utf8ToWide(g_webServerUrl).c_str())) {
            FatalError(m_hWnd, "Could not create Browser control.\n"
                               "Exiting application.");
        }
        SetTimer(CLICK_EVENTS_TIMER, 10, NULL);
        SetAllowedUrl(Utf8ToWide(g_webServerUrl).c_str());

        CMessageLoop* pLoop = g_appModule.GetMessageLoop();
        _ASSERT(pLoop != NULL);
        pLoop->AddMessageFilter(this);
        // pLoop->AddIdleHandler(this);
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
        CloseBrowser();
        TerminateWebServer();
        bHandled = FALSE;
        return 0;
    }
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, 
                      BOOL& bHandled) {
        CMessageLoop* pLoop = g_appModule.GetMessageLoop();
        _ASSERT(pLoop != NULL);
        pLoop->RemoveMessageFilter(this);
        // pLoop->RemoveIdleHandler(this);
        // When bHandled is false return value doesn't matter.
        bHandled = FALSE;
        return 0;
    }
    bool IsPopup() {
        return false;
    }
    bool IsUsingMetaTitle() {
        return false;
    }
};
