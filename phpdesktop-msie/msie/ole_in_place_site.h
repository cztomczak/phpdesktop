// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "browser_frame_interface.h"
#include "ole_in_place_frame.h"

template <class TopFrame>
class OleInPlaceSite : public IOleInPlaceSite
{
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;
    IOleInPlaceFrame* oleInPlaceFrame_;

    OleInPlaceSite(BrowserFrameInterface<TopFrame>* inBrowserFrame,
                   IOleInPlaceFrame* inOleInPlaceFrame)
            : browserFrame_(inBrowserFrame),
            oleInPlaceFrame_(inOleInPlaceFrame) {
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
    HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd) {
        ATLASSERT(browserFrame_->GetBrowserHandle());
        *lphwnd = browserFrame_->GetBrowserHandle();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) {
        return S_OK;
    }
    // IOleInPlaceSite
    HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnUIActivate(void) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetWindowContext( 
            /* [out] */ IOleInPlaceFrame **ppFrame,
            /* [out] */ IOleInPlaceUIWindow **ppDoc,
            /* [out] */ LPRECT lprcPosRect,
            /* [out] */ LPRECT lprcClipRect,
            /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo) {
        ATLASSERT(oleInPlaceFrame_);
        ATLASSERT(browserFrame_->GetBrowserHandle());
        *ppFrame = oleInPlaceFrame_;
        *ppDoc = 0;
        GetClientRect(browserFrame_->GetBrowserHandle(), lprcPosRect);
        GetClientRect(browserFrame_->GetBrowserHandle(), lprcClipRect);
        lpFrameInfo->fMDIApp = FALSE;
        lpFrameInfo->hwndFrame = browserFrame_->GetBrowserHandle();
        lpFrameInfo->haccel = 0;
        lpFrameInfo->cAccelEntries = 0;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtent) {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DiscardUndoState(void) {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void) {
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect) {
        if (!lprcPosRect)
            return E_INVALIDARG;
        HRESULT hr;
        CComQIPtr<IOleObject> oleObject;
        CComQIPtr<IWebBrowser2> browser = browserFrame_->GetBrowser();        
        hr = browser->QueryInterface(IID_IOleObject, (void**)&oleObject);
        if (FAILED(hr) || !oleObject) {
            LOG(logWARNING) << "OleInPlaceSite::OnPosRectChange() failed: "
                               "QueryInterface(IOleObject) failed";
            return E_UNEXPECTED;
        }
        CComQIPtr<IOleInPlaceObject> oleInPlaceObject;
        hr = oleObject->QueryInterface(IID_IOleInPlaceObject, 
                                       (void**)&oleInPlaceObject);
        if (FAILED(hr) || !oleInPlaceObject) {
            LOG(logWARNING) << "OleInPlaceSite::OnPosRectChange() failed: "
                               "QueryInterface(IOleInPlaceObject) failed";
            return E_UNEXPECTED;
        }
        hr = oleInPlaceObject->SetObjectRects(lprcPosRect, lprcPosRect);
        if (FAILED(hr)) {
            LOG(logWARNING) << "OleInPlaceSite::OnPosRectChange() failed: "
                               "IOleInPlaceObject->SetObjectRects() failed";
            return E_UNEXPECTED;
        }
        return S_OK;
    }
};
