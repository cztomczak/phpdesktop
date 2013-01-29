// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

template <class TopFrame>
class BrowserFrameInterface
{
public:
    virtual HWND GetWindowHandle() = 0;
    virtual IOleClientSite* GetOleClientSite() = 0;
    virtual CComQIPtr<IWebBrowser2> GetBrowser() = 0;
    virtual bool GetActiveHtmlElement(wchar_t* outTag, int outTagSize,
                                      wchar_t* outType, int outTypeSize) = 0;
    virtual void SetWidth(long width) = 0;
    virtual void SetHeight(long height) = 0;
    virtual void SetTop(long top) = 0;
    virtual void SetLeft(long left) = 0;
    virtual void SetTitle(const wchar_t* title) = 0;
    virtual bool IsPopup() = 0;
    virtual bool IsUsingMetaTitle() = 0;
};
