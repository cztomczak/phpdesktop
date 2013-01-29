// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "browser_frame_interface.h"

template <class TopFrame>
class HtmlOmWindowServices : public IHTMLOMWindowServices
{
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;

    HtmlOmWindowServices(BrowserFrameInterface<TopFrame>* inBrowserFrame) 
            : browserFrame_(inBrowserFrame) {
    }
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) {
        *ppvObject = 0;
        return E_NOTIMPL;
        return browserFrame_->GetOleClientSite()->QueryInterface(riid, 
                                                                 ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }
    // IHTMLOMWindowServices
    HRESULT STDMETHODCALLTYPE moveTo(LONG x, LONG y) {
        SetWindowPos(browserFrame_->GetWindowHandle(), NULL, (int)x, (int)y, 
                     0, 0, SWP_NOSIZE | SWP_NOZORDER);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE moveBy(LONG x, LONG y) {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE resizeTo(LONG x, LONG y) {
        SetWindowPos(browserFrame_->GetWindowHandle(), NULL, 0, 0, 
                     (int)x, (int)y, SWP_NOMOVE | SWP_NOZORDER);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE resizeBy(LONG x, LONG y) {
        return E_NOTIMPL;
    }
};
