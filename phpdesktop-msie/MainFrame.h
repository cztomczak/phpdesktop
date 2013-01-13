// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "winmain.h"
#include "WebBrowser/WebBrowserFrame.h"
#include "WebBrowser/FullScreenFrame.h"

class MainFrame :
	public CFrameWindowImpl<MainFrame>,
	public CUpdateUI<MainFrame>,
	public CMessageFilter,
	public CIdleHandler,
	public WebBrowserFrame<MainFrame>,
	public FullScreenFrame<MainFrame>
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	MainView rootview;	
	MainFrame() {}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<MainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return rootview.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	
	BEGIN_UPDATE_UI_MAP(MainFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(MainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		CHAIN_MSG_MAP(CUpdateUI<MainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<MainFrame>)
	END_MSG_MAP()


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{

		wchar_t* httpaddr = GetHttpAddressPort();
		ASSERT_EXIT((wcslen(httpaddr) > 10), "httpaddr length <= 10");
		CreateBrowser(httpaddr);		

		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		return 0;
	}

	LRESULT OnGetMinMaxInfo(UINT, WPARAM, LPARAM lParam, BOOL&)
	{
		// load size structure with lParam values
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

		// change the values in the size structure to desired values
		lpMMI->ptMinTrackSize.x = 600; // min width
		lpMMI->ptMinTrackSize.y = 600; // min height
		//lpMMI->ptMaxTrackSize.x = 0; // max width, cannot set to 0!
		//lpMMI->ptMaxTrackSize.y = 0; // max height

		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		TerminatePHPShell();
		if (isfullscreen) {
			ShowTaskBar(true);
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);
		bHandled = FALSE;
		return 1;
	}

};
