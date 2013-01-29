// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <OCIdl.h>
#include "browser_frame_interface.h"

template <class TopFrame>
class OleControlSite : public IOleControlSite {
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;

    OleControlSite(BrowserFrameInterface<TopFrame>* inBrowserFrame)
            : browserFrame_(inBrowserFrame) {
    }
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) {
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
    // IOleControlSite
    HRESULT STDMETHODCALLTYPE OnControlInfoChanged(void) {
        return S_OK;
    }    
    HRESULT STDMETHODCALLTYPE LockInPlaceActive( 
            /* [in] */ BOOL fLock) {
        return S_OK;
    }    
    HRESULT STDMETHODCALLTYPE GetExtendedControl( 
            /* [out] */ IDispatch **ppDisp) {
        *ppDisp = 0;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE TransformCoords( 
            /* [out][in] */ POINTL *pPtlHimetric,
            /* [out][in] */ POINTF *pPtfContainer,
            /* [in] */ DWORD dwFlags) {
        return E_NOTIMPL;
    }    
    HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ MSG *pMsg,
            /* [in] */ DWORD grfModifiers) {
        return S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE OnFocus( 
            /* [in] */ BOOL fGotFocus) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ShowPropertyFrame(void) {
        return E_NOTIMPL;
    }
};
