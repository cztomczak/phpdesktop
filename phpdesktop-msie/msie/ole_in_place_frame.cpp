// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "ole_in_place_frame.h"
#include "browser_window.h"
#include "../log.h"

OleInPlaceFrame::OleInPlaceFrame(BrowserWindow* inBrowserWindow)
    : browserWindow_(inBrowserWindow) {
}
// IUnknown
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::QueryInterface(
        REFIID riid, 
        void** ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(riid, 
                                                             ppvObject);
}
ULONG STDMETHODCALLTYPE OleInPlaceFrame::AddRef(void) {
    return 1;
}
ULONG STDMETHODCALLTYPE OleInPlaceFrame::Release(void) {
    return 1;
}
// IOleWindow
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::GetWindow(
        /* [out] */ HWND FAR* lphwnd) {
    _ASSERT(browserWindow_->GetWindowHandle());
    *lphwnd = browserWindow_->GetWindowHandle();
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::ContextSensitiveHelp(
        /* [in] */ BOOL fEnterMode) {
    return S_OK;
}
// IOleInPlaceUIWindow
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::GetBorder(
        /* [out] */ LPRECT lprectBorder) {
    if (lprectBorder == NULL) 
        return E_INVALIDARG;
    return INPLACE_E_NOTOOLSPACE;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::RequestBorderSpace(
        /* [in] */ LPCBORDERWIDTHS pborderwidths) {
    return INPLACE_E_NOTOOLSPACE;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::SetBorderSpace(
        /* [in] */ LPCBORDERWIDTHS pborderwidths) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::SetActiveObject(
        /* [in] */ IOleInPlaceActiveObject *pActiveObject, 
        /* [in] */ LPCOLESTR pszObjName) {
    return S_OK;
}
// IOleInPlaceFrame
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::InsertMenus(
        /* [in] */ HMENU hmenuShared, 
        /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths) {
    lpMenuWidths->width[0] = 0;
    lpMenuWidths->width[2] = 0;
    lpMenuWidths->width[4] = 0;
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::SetMenu(
        /* [in] */ HMENU hmenuShared, 
        /* [in] */ HOLEMENU holemenu, 
        /* [in] */ HWND hwndActiveObject) {
    if (hmenuShared == NULL || holemenu == NULL 
            || hwndActiveObject == NULL) {
        return E_INVALIDARG;
    }
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::RemoveMenus(
        /* [in] */ HMENU hmenuShared) {
    if (hmenuShared == NULL)
        return E_INVALIDARG;
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::SetStatusText(
        /* [in] */ LPCOLESTR pszStatusText) {
    if (pszStatusText == NULL)
        return E_INVALIDARG;
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::EnableModeless(
        /* [in] */ BOOL fEnable) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleInPlaceFrame::TranslateAccelerator(
        /* [in] */ LPMSG lpmsg, 
        /* [in] */ WORD wID) {
    // Seems like this method is never called.
    if (lpmsg == NULL)
        return E_INVALIDARG;
    return S_FALSE;
}
