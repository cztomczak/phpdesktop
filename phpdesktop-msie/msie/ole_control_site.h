// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// This interface IS NOT used, see OleClientSite::QueryInterface().
// When using DocHostUIHandlerDispatch() you do not need to
// implement: IOleInPlaceFrame, IOleInPlaceSite, IOleControlSite.

#pragma once

OleControlSite : IOleControlSite {
public:
    virtual HRESULT STDMETHODCALLTYPE OnControlInfoChanged( void) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE LockInPlaceActive( 
        /* [in] */ BOOL fLock) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE GetExtendedControl( 
        /* [out] */ __RPC__deref_out_opt IDispatch **ppDisp) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE TransformCoords( 
        /* [out][in] */ __RPC__inout POINTL *pPtlHimetric,
        /* [out][in] */ __RPC__inout POINTF *pPtfContainer,
        /* [in] */ DWORD dwFlags) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
        /* [in] */ __RPC__in MSG *pMsg,
        /* [in] */ DWORD grfModifiers) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE OnFocus( 
        /* [in] */ BOOL fGotFocus) = 0;
    
    virtual HRESULT STDMETHODCALLTYPE ShowPropertyFrame( void) = 0;
    
};
