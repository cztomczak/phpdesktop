// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#pragma once

#include <OCIdl.h>
class BrowserWindow;

class OleControlSite : public IOleControlSite {
private:
    BrowserWindow* browserWindow_;
public:
    OleControlSite(BrowserWindow* inBrowserWindow);
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    // IOleControlSite
    HRESULT STDMETHODCALLTYPE OnControlInfoChanged(void);
    HRESULT STDMETHODCALLTYPE LockInPlaceActive( 
            /* [in] */ BOOL fLock);
    HRESULT STDMETHODCALLTYPE GetExtendedControl( 
            /* [out] */ IDispatch **ppDisp);
    HRESULT STDMETHODCALLTYPE TransformCoords( 
            /* [out][in] */ POINTL *pPtlHimetric,
            /* [out][in] */ POINTF *pPtfContainer,
            /* [in] */ DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ MSG *pMsg,
            /* [in] */ DWORD grfModifiers);
    HRESULT STDMETHODCALLTYPE OnFocus( 
            /* [in] */ BOOL fGotFocus);
    HRESULT STDMETHODCALLTYPE ShowPropertyFrame(void);
};
