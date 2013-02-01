// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "ole_control_site.h"
#include "browser_window.h"

OleControlSite::OleControlSite(BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow) {
}
// IUnknown
HRESULT STDMETHODCALLTYPE OleControlSite::QueryInterface(
        REFIID riid, 
        void** ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(riid, ppvObject);
}
ULONG STDMETHODCALLTYPE OleControlSite::AddRef(void) {
    return 1;
}
ULONG STDMETHODCALLTYPE OleControlSite::Release(void) {
    return 1;
}
// IOleControlSite
HRESULT STDMETHODCALLTYPE OleControlSite::OnControlInfoChanged(void) {
    return S_OK;
}    
HRESULT STDMETHODCALLTYPE OleControlSite::LockInPlaceActive( 
        /* [in] */ BOOL fLock) {
    return S_OK;
}    
HRESULT STDMETHODCALLTYPE OleControlSite::GetExtendedControl( 
        /* [out] */ IDispatch **ppDisp) {
    *ppDisp = 0;
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleControlSite::TransformCoords( 
        /* [out][in] */ POINTL *pPtlHimetric,
        /* [out][in] */ POINTF *pPtfContainer,
        /* [in] */ DWORD dwFlags) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE OleControlSite::TranslateAccelerator( 
        /* [in] */ MSG *pMsg,
        /* [in] */ DWORD grfModifiers) {
    return S_FALSE;
}
HRESULT STDMETHODCALLTYPE OleControlSite::OnFocus( 
        /* [in] */ BOOL fGotFocus) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleControlSite::ShowPropertyFrame(void) {
    return E_NOTIMPL;
}
