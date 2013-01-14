// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

template <class RootFrame>
class BrowserFrameInterface
{
public:
    virtual HWND GetWindowHandle() = 0;
    virtual IOleClientSite* GetOleClientSite() = 0;
    virtual bool GetActiveElement(wchar_t* tag, wchar_t* typeattr) = 0;

    virtual HRESULT RootFrame_GetDlgControl(int nID, REFIID iid, void** ppCtrl) = 0;

    virtual HWND RootView_GetWindowHandle() = 0;
    virtual int RootView_GetDlgCtrlID() = 0;
};
