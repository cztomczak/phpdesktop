// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <OleIdl.h>
class BrowserWindow;

class OleInPlaceFrame : public IOleInPlaceFrame {
private:
    BrowserWindow* browserWindow_;
public:
    OleInPlaceFrame(BrowserWindow* inBrowserWindow);
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    // IOleWindow
    HRESULT STDMETHODCALLTYPE GetWindow(
            /* [out] */ HWND FAR* lphwnd);
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(
            /* [in] */ BOOL fEnterMode);
    // IOleInPlaceUIWindow
    HRESULT STDMETHODCALLTYPE GetBorder(
            /* [out] */ LPRECT lprectBorder);
    HRESULT STDMETHODCALLTYPE RequestBorderSpace(
            /* [in] */ LPCBORDERWIDTHS pborderwidths);
    HRESULT STDMETHODCALLTYPE SetBorderSpace(
            /* [in] */ LPCBORDERWIDTHS pborderwidths);
    HRESULT STDMETHODCALLTYPE SetActiveObject(
            /* [in] */ IOleInPlaceActiveObject *pActiveObject,
            /* [in] */ LPCOLESTR pszObjName);
    // IOleInPlaceFrame
    HRESULT STDMETHODCALLTYPE InsertMenus(
            /* [in] */ HMENU hmenuShared,
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);
    HRESULT STDMETHODCALLTYPE SetMenu(
            /* [in] */ HMENU hmenuShared,
            /* [in] */ HOLEMENU holemenu,
            /* [in] */ HWND hwndActiveObject);
    HRESULT STDMETHODCALLTYPE RemoveMenus(
            /* [in] */ HMENU hmenuShared);
    HRESULT STDMETHODCALLTYPE SetStatusText(
            /* [in] */ LPCOLESTR pszStatusText);
    HRESULT STDMETHODCALLTYPE EnableModeless(
            /* [in] */ BOOL fEnable);
    HRESULT STDMETHODCALLTYPE TranslateAccelerator(
            /* [in] */ LPMSG lpmsg,
            /* [in] */ WORD wID);
};
