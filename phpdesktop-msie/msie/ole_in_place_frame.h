// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "browser_frame_interface.h"

template <class TopFrame>
class OleInPlaceFrame : public IOleInPlaceFrame
{
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;

    OleInPlaceFrame(BrowserFrameInterface<TopFrame>* inBrowserFrame)
        : browserFrame_(inBrowserFrame) {
    }
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) {
        return browserFrame_->GetOleClientSite()->QueryInterface(riid, 
                                                                 ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }
    // IOleWindow
    HRESULT STDMETHODCALLTYPE GetWindow(
            /* [out] */ HWND FAR* lphwnd) {
        ATLASSERT(browserFrame_->GetBrowserHandle());
        *lphwnd = browserFrame_->GetBrowserHandle();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
            /* [in] */ BOOL fEnterMode) {
        return S_OK;
    }
    // IOleInPlaceUIWindow
    HRESULT STDMETHODCALLTYPE GetBorder(
            /* [out] */ LPRECT lprectBorder) {
        if (lprectBorder == NULL) 
            return E_INVALIDARG;
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE RequestBorderSpace(
            /* [in] */ LPCBORDERWIDTHS pborderwidths) {
        return INPLACE_E_NOTOOLSPACE;
    }
    HRESULT STDMETHODCALLTYPE SetBorderSpace(
            /* [in] */ LPCBORDERWIDTHS pborderwidths) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetActiveObject(
            /* [in] */ IOleInPlaceActiveObject *pActiveObject, 
            /* [in] */ LPCOLESTR pszObjName) {
        return S_OK;
    }
    // IOleInPlaceFrame
    HRESULT STDMETHODCALLTYPE InsertMenus(
            /* [in] */ HMENU hmenuShared, 
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths) {
        lpMenuWidths->width[0] = 0;
        lpMenuWidths->width[2] = 0;
        lpMenuWidths->width[4] = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetMenu(
            /* [in] */ HMENU hmenuShared, 
            /* [in] */ HOLEMENU holemenu, 
            /* [in] */ HWND hwndActiveObject) {
        if (hmenuShared == NULL || holemenu == NULL 
                || hwndActiveObject == NULL) {
            return E_INVALIDARG;
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RemoveMenus(
            /* [in] */ HMENU hmenuShared) {
        if (hmenuShared == NULL)
            return E_INVALIDARG;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetStatusText(
            /* [in] */ LPCOLESTR pszStatusText) {
        if (pszStatusText == NULL)
            return E_INVALIDARG;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE EnableModeless(
            /* [in] */ BOOL fEnable) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(
            /* [in] */ LPMSG lpmsg, 
            /* [in] */ WORD wID) {
        if (lpmsg == NULL)
            return E_INVALIDARG;
        return S_FALSE;
    }
};
