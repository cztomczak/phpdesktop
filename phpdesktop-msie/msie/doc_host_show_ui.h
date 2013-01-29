// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <windows.h>
#include <MsHtmHst.h>
#include "../settings.h"

template <class TopFrame>
class DocHostShowUi : public IDocHostShowUI {
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;
    DocHostShowUi(BrowserFrameInterface<TopFrame>* inBrowserFrame)
            : browserFrame_(inBrowserFrame) {
    }
    HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject) {
        *ppvObject = 0;
        return E_NOTIMPL;
        return browserFrame_->GetOleClientSite()->QueryInterface(riid, 
                                                                 ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef( void) {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release( void) {
        return 1;
    }
    HRESULT STDMETHODCALLTYPE ShowMessage( 
            /* [in] */ HWND hwnd,
            /* [in] */ LPOLESTR lpstrText,
            /* [in] */ LPOLESTR lpstrCaption,
            /* [in] */ DWORD dwType,
            /* [in] */ LPOLESTR lpstrHelpFile,
            /* [in] */ DWORD dwHelpContext,
            /* [out] */ LRESULT *plResult) {
        // window.alert() may be customized here.
        // S_OK Host displayed its UI. MSHTML does not display its message box.
        // S_FALSE Host did not display its UI. MSHTML displays its message box.

        // See also webBrowser2_->put_Silent().
        json_value* settings = GetApplicationSettings();
        bool hide_dialog_boxes = (*settings)["msie"]["hide_dialog_boxes"];
        if (hide_dialog_boxes)
            return S_OK;

        return S_FALSE;
    }    
    HRESULT STDMETHODCALLTYPE ShowHelp( 
            /* [in] */ HWND hwnd,
            /* [in] */ LPOLESTR pszHelpFile,
            /* [in] */ UINT uCommand,
            /* [in] */ DWORD dwData,
            /* [in] */ POINT ptMouse,
            /* [out] */ IDispatch *pDispatchObjectHit) {
        // S_OK Host displayed Help. MSHTML will not display its Help.
        // S_FALSE Host did not display Help. MSHTML will display its Help.
        return S_OK;
    }
};
