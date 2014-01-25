// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <windows.h>
#include <MsHtmHst.h>
class BrowserWindow;

class DocHostShowUi : public IDocHostShowUI {
private:
    BrowserWindow* browserWindow_;
public:
    DocHostShowUi(BrowserWindow* inBrowserWindow);
    HRESULT STDMETHODCALLTYPE QueryInterface(
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    HRESULT STDMETHODCALLTYPE ShowMessage(
            /* [in] */ HWND hwnd,
            /* [in] */ LPOLESTR lpstrText,
            /* [in] */ LPOLESTR lpstrCaption,
            /* [in] */ DWORD dwType,
            /* [in] */ LPOLESTR lpstrHelpFile,
            /* [in] */ DWORD dwHelpContext,
            /* [out] */ LRESULT *plResult);
    HRESULT STDMETHODCALLTYPE ShowHelp(
            /* [in] */ HWND hwnd,
            /* [in] */ LPOLESTR pszHelpFile,
            /* [in] */ UINT uCommand,
            /* [in] */ DWORD dwData,
            /* [in] */ POINT ptMouse,
            /* [out] */ IDispatch *pDispatchObjectHit);
};
