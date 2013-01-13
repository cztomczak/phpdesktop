// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

class WebBrowserFrameInterface
{
public:
	virtual HWND GetHWND() = 0;
	virtual HWND View_GetHWND() = 0;
	virtual IOleClientSite* GetOleClientSite() = 0;
	virtual int View_GetDlgCtrlID() = 0;
	virtual HRESULT Self_GetDlgControl(int nID, REFIID iid, void** ppCtrl) = 0;
	virtual bool GetActiveElement(wchar_t* tag, wchar_t* typeattr) = 0;
};
