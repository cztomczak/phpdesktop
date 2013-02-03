// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "ole_in_place_site.h"
#include "browser_window.h"
#include "ole_in_place_frame.h"
#include "../log.h"

OleInPlaceSite::OleInPlaceSite(BrowserWindow* inBrowserWindow,
                               IOleInPlaceFrame* inOleInPlaceFrame)
        : browserWindow_(inBrowserWindow),
        oleInPlaceFrame_(inOleInPlaceFrame) {
}
// IUnknown
HRESULT STDMETHODCALLTYPE OleInPlaceSite::QueryInterface(
        REFIID riid, 
        void** ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(riid, ppvObject);
}
ULONG STDMETHODCALLTYPE OleInPlaceSite::AddRef(void) {
    return 1;
}
ULONG STDMETHODCALLTYPE OleInPlaceSite::Release(void) {
    return 1;
}
// IOleWindow
HRESULT STDMETHODCALLTYPE OleInPlaceSite::GetWindow(HWND FAR* lphwnd) {
    if (!lphwnd)
        return E_FAIL;
    _ASSERT(browserWindow_->GetWindowHandle());
    *lphwnd = browserWindow_->GetWindowHandle();
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::ContextSensitiveHelp(
        BOOL fEnterMode) {
    return S_OK;
}
// IOleInPlaceSite
HRESULT STDMETHODCALLTYPE OleInPlaceSite::CanInPlaceActivate(void) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::OnInPlaceActivate(void) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::OnUIActivate(void) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::GetWindowContext( 
        /* [out] */ IOleInPlaceFrame **ppFrame,
        /* [out] */ IOleInPlaceUIWindow **ppDoc,
        /* [out] */ LPRECT lprcPosRect,
        /* [out] */ LPRECT lprcClipRect,
        /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo) {
    _ASSERT(oleInPlaceFrame_);
    _ASSERT(browserWindow_->GetWindowHandle());
    *ppFrame = oleInPlaceFrame_;
    *ppDoc = 0;
    BOOL b = GetClientRect(browserWindow_->GetWindowHandle(), lprcPosRect);
    _ASSERT(b);
    b = GetClientRect(browserWindow_->GetWindowHandle(), lprcClipRect);
    _ASSERT(b);
    if(lpFrameInfo->cb == sizeof(OLEINPLACEFRAMEINFO)) {
        lpFrameInfo->fMDIApp = FALSE;
        lpFrameInfo->hwndFrame = browserWindow_->GetWindowHandle();
        lpFrameInfo->haccel = 0;
        lpFrameInfo->cAccelEntries = 0;
    }
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::Scroll(SIZE scrollExtent) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::OnUIDeactivate(BOOL fUndoable) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::OnInPlaceDeactivate(void) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::DiscardUndoState(void) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::DeactivateAndUndo(void) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleInPlaceSite::OnPosRectChange(
        LPCRECT lprcPosRect) {
    if (!lprcPosRect)
        return E_INVALIDARG;
    HRESULT hr;
    IOleObjectPtr oleObject;
    const IWebBrowser2Ptr webBrowser2 = 
            browserWindow_->GetWebBrowser2();        
    hr = webBrowser2->QueryInterface(IID_IOleObject, (void**)&oleObject);
    if (FAILED(hr) || !oleObject) {
        LOG_WARNING << "OleInPlaceSite::OnPosRectChange() failed: "
                       "QueryInterface(IOleObject) failed";
        return E_UNEXPECTED;
    }
    IOleInPlaceObjectPtr oleInPlaceObject;
    hr = oleObject->QueryInterface(IID_IOleInPlaceObject, 
                                   (void**)&oleInPlaceObject);
    if (FAILED(hr) || !oleInPlaceObject) {
        LOG_WARNING << "OleInPlaceSite::OnPosRectChange() failed: "
                       "QueryInterface(IOleInPlaceObject) failed";
        return E_UNEXPECTED;
    }
    hr = oleInPlaceObject->SetObjectRects(lprcPosRect, lprcPosRect);
    if (FAILED(hr)) {
        LOG_WARNING << "OleInPlaceSite::OnPosRectChange() failed: "
                       "IOleInPlaceObject->SetObjectRects() failed";
        return E_UNEXPECTED;
    }
    return S_OK;
}
