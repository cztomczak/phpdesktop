// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <windows.h>
#include <MsHtmHst.h>
class BrowserWindow;

class DocHostUiHandler : public IDocHostUIHandler {
private:
    BrowserWindow* browserWindow_;
    IDispatch* externalDispatch_;
public:
    DocHostUiHandler(BrowserWindow* inBrowserWindow,
                     IDispatch* inExternalDispatch);
    HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    HRESULT STDMETHODCALLTYPE ShowContextMenu( 
            /* [in] */ DWORD dwID,
            /* [in] */ POINT *ppt,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch *pdispReserved);
    HRESULT STDMETHODCALLTYPE GetHostInfo( 
            /* [out][in] */ DOCHOSTUIINFO *pInfo);
    HRESULT STDMETHODCALLTYPE ShowUI( 
            /* [in] */ DWORD dwID,
            /* [in] */ IOleInPlaceActiveObject *pActiveObject,
            /* [in] */ IOleCommandTarget *pCommandTarget,
            /* [in] */ IOleInPlaceFrame *pFrame,
            /* [in] */ IOleInPlaceUIWindow *pDoc);
    HRESULT STDMETHODCALLTYPE HideUI(void);
    HRESULT STDMETHODCALLTYPE UpdateUI(void);
    HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable);
    HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
            /* [in] */ BOOL fActivate);
    HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( 
            /* [in] */ BOOL fActivate);
    HRESULT STDMETHODCALLTYPE ResizeBorder( 
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IOleInPlaceUIWindow *pUIWindow,
            /* [in] */ BOOL fRameWindow);
    HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ LPMSG lpMsg,
            /* [in] */ const GUID *pguidCmdGroup,
            /* [in] */ DWORD nCmdID);
    HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
            /* [out] */ LPOLESTR *pchKey,
            /* [in] */ DWORD dw);
    HRESULT STDMETHODCALLTYPE GetDropTarget( 
            /* [in] */ IDropTarget *pDropTarget,
            /* [out] */ IDropTarget **ppDropTarget);
    HRESULT STDMETHODCALLTYPE GetExternal( 
            /* [out] */ IDispatch **ppDispatch);
    HRESULT STDMETHODCALLTYPE TranslateUrl( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ OLECHAR *pchURLIn,
            /* [out] */ OLECHAR **ppchURLOut);
    HRESULT STDMETHODCALLTYPE FilterDataObject( 
            /* [in] */ IDataObject *pDO,
            /* [out] */ IDataObject **ppDORet);
};
