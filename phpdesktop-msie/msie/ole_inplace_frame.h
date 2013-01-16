// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// This interface IS NOT used, see OleClientSite::QueryInterface().
// When using DocHostUIHandlerDispatch() you do not need to
// implement: IOleInPlaceFrame, IOleInPlaceSite, IOleControlSite.

#pragma once

template <class RootFrame>
class OleInPlaceFrame : public IOleInPlaceFrame
{
public:
    IOleClientSite* oleClientSite;
    BrowserFrameInterface<RootFrame>* webFrame;

    OleInPlaceFrame(IOleClientSite* oleClientSite, BrowserFrameInterface<RootFrame>* inWebFrame)
    {
        oleClientSite = oleClientSite;
        webFrame = inWebFrame;
    }
public:

    // IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
    {
        return oleClientSite->QueryInterface(riid, ppvObj);
    }
    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void)
    {
        return 1;
    }

    // IOleWindow

    HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd)
    {
        *lphwnd = webFrame->RootView_GetWindowHandle();
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode)
    {
        return E_NOTIMPL;
    }

    // IOleInPlaceUIWindow

    HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
    {
        return S_OK;
    }

    // IOleInPlaceFrame

    HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable)
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID)
    {
        return E_NOTIMPL;
    }
};
