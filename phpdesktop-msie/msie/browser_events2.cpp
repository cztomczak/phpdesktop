// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// DWebBrowserEvents2 interface for WebBrowser control:
// http://msdn.microsoft.com/en-us/library/aa768283(v=vs.85).aspx
// (DWebBrowserEvents2 for MSHTML has different methods).

#include "../defines.h"
#include "browser_events2.h"
#include "browser_window.h"

#include "../log.h"
#include "../main.h"
#include "../settings.h"
#include "../window_utils.h"
#include "../resource.h"

extern HINSTANCE g_hInstance;
extern wchar_t g_windowClassName[256];

HWND CreatePopupWindow(HWND parentHandle) {
    json_value* settings = GetApplicationSettings();    
    bool center_relative_to_parent = 
            (*settings)["popup_window"]["center_relative_to_parent"];

    // Title will be set in BrowserWindow::BrowserWindow().
    // CW_USEDEFAULT cannot be used with WS_POPUP.
    HWND hwnd = CreateWindowEx(0, g_windowClassName, 
            0, WS_OVERLAPPEDWINDOW, 
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
            parentHandle, 0, g_hInstance, 0);
    _ASSERT(hwnd);
    if (center_relative_to_parent) {
        // This won't work properly as real width/height is set later
        // when BrowserEvents2::WindowSetWidth() / WindowSetHeight()
        // are triggered. TODO.
        // CenterWindow(hwnd);
    }
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd); 
    return hwnd;
}

BrowserEvents2::BrowserEvents2(BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow) {
}
HRESULT STDMETHODCALLTYPE BrowserEvents2::QueryInterface( 
        /* [in] */ REFIID riid,
        /* [out] */ void **ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(
            riid, ppvObject);
}    
ULONG STDMETHODCALLTYPE BrowserEvents2::AddRef(void) {
    return 1;
}    
ULONG STDMETHODCALLTYPE BrowserEvents2::Release(void) {
    return 1;
}    
HRESULT STDMETHODCALLTYPE BrowserEvents2::GetTypeInfoCount( 
        /* [out] */ UINT *pctinfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE BrowserEvents2::GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ ITypeInfo **ppTInfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE BrowserEvents2::GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [out] */ DISPID *rgDispId) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE BrowserEvents2::Invoke( 
        /* [in] */ DISPID dispId,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS *pDispParams,
        /* [out] */ VARIANT *pVarResult,
        /* [out] */ EXCEPINFO *pExcepInfo,
        /* [out] */ UINT *puArgErr) {
    
    // When returning a result, you must check whether pVarResult
    // is not NULL and initialize it using VariantInit(). If it's
    // NULL then it doesn't expect a result.
    if (riid != IID_NULL)
        return DISP_E_UNKNOWNINTERFACE;
    pExcepInfo = 0;
    puArgErr = 0;
    HRESULT hr;
    json_value* settings = GetApplicationSettings();
    
    if (dispId == DISPID_NEWWINDOW3) {
        /* When calling window.open() you get an error "Class
           not registered". Before this error appears 
           DWebBrowserEvents2::NewWindow3 event is dispatched, 
           you need to create the popup window in this event
           and assign the dispatch interface of the new popup
           browser to the first parameter of NewWindow3. */
        LOG_DEBUG << "BrowserEvents2::NewWindow3()";            
        if (pDispParams->cArgs != 5) {
            LOG_WARNING << "BrowserEvents2::NewWindow3() failed: "
                    "Wrong number of arguments, expected 5";
            return DISP_E_BADPARAMCOUNT;
        }
        // ppDisp
        _ASSERT(pDispParams->rgvarg[4].vt == (VT_DISPATCH | VT_BYREF));
        // Cancel
        _ASSERT(pDispParams->rgvarg[3].vt == (VT_BOOL | VT_BYREF)); 
        // dwFlags
        _ASSERT(pDispParams->rgvarg[2].vt == VT_I4); 
        // bstrUrlContext
        _ASSERT(pDispParams->rgvarg[1].vt == VT_BSTR); 
        // bstrUrl        
        _ASSERT(pDispParams->rgvarg[0].vt == VT_BSTR); 

        HWND popupHandle = CreatePopupWindow(
                browserWindow_->GetWindowHandle());
        _ASSERT(popupHandle);
        BrowserWindow* browserWindow = GetBrowserWindow(popupHandle);
        if (!browserWindow) {
            LOG_WARNING << "BrowserEvents2::NewWindow3() failed: "
                           "CreatePopupWindow() failed";
            // Cancel parameter. Current navigation should be cancelled.
            *pDispParams->rgvarg[3].pboolVal = VARIANT_TRUE;
            return S_OK;
        }
        const IWebBrowser2Ptr webBrowser2 = browserWindow->GetWebBrowser2();
        IDispatchPtr dispatch;
        hr = webBrowser2->get_Application(&dispatch);
        if (FAILED(hr) || !dispatch) {
            LOG_WARNING << "BrowserEvents2::NewWindow3() failed: "
                           "webBrowser2->get_Application() failed";
            return S_OK;
        }
        
        *pDispParams->rgvarg[4].ppdispVal = dispatch.Detach();
        *pDispParams->rgvarg[3].pboolVal = VARIANT_FALSE;        

        // Following events (DWebBrowserEvents2) will appear  
        // after popup creation, they inform about "features"
        // passed to "window.open", such as width, height and others:
        // DISPID_ONTOOLBAR
        // DISPID_ONADDRESSBAR
        // DISPID_WINDOWSETRESIZABLE
        // DISPID_ONMENUBAR
        // DISPID_ONSTATUSBAR
        // DISPID_ONFULLSCREEN
        // DISPID_CLIENTTOHOSTWINDOW
        // DISPID_WINDOWSETWIDTH
        // DISPID_WINDOWSETHEIGHT
        // DISPID_WINDOWSETTOP
        // DISPID_WINDOWSETLEFT
        // DISPID_NAVIGATECOMPLETE2            
        return S_OK;
    } else if (dispId == DISPID_WINDOWSETWIDTH) {
        _ASSERT(pDispParams->cArgs == 1);
        _ASSERT(pDispParams->rgvarg[0].vt == VT_I4); // nWidth
        long width = pDispParams->rgvarg[0].lVal;
        // LOG_DEBUG << "BrowserEvents2::WindowSetWidth(): width = "
        //           << width;
        browserWindow_->SetWidth(width);
    } else if (dispId == DISPID_WINDOWSETHEIGHT) {
        _ASSERT(pDispParams->cArgs == 1);
        _ASSERT(pDispParams->rgvarg[0].vt == VT_I4); // nHeight
        long height = pDispParams->rgvarg[0].lVal;
        // LOG_DEBUG << "BrowserEvents2::WindowSetHeight(): height = "
        //           << height;
        browserWindow_->SetHeight(height);
    } else if (dispId == DISPID_WINDOWSETTOP) {
        _ASSERT(pDispParams->cArgs == 1);
        _ASSERT(pDispParams->rgvarg[0].vt == VT_I4); // nTop
        long top = pDispParams->rgvarg[0].lVal;
        // LOG_DEBUG << "BrowserEvents2::WindowSetTop(): top = "
        //           << top;
        browserWindow_->SetTop(top);
    } else if (dispId == DISPID_WINDOWSETLEFT) {
        _ASSERT(pDispParams->cArgs == 1);
        _ASSERT(pDispParams->rgvarg[0].vt == VT_I4); // nLeft
        long left = pDispParams->rgvarg[0].lVal;
        // LOG_DEBUG << "BrowserEvents2::WindowSetLeft(): left = "
        //           << left;
        browserWindow_->SetLeft(left);
    } else if (dispId == DISPID_TITLECHANGE) {
        if (browserWindow_->IsPopup()
                && browserWindow_->IsUsingMetaTitle()) {
            _ASSERT(pDispParams->cArgs == 1);
            _ASSERT(pDispParams->rgvarg[0].vt == VT_BSTR); // Text
            BSTR title = pDispParams->rgvarg[0].bstrVal;
            // LOG_DEBUG << "BrowserEvents2::TitleChange(): "
            //              "setting popup title = " << WideToUtf8(title);
            browserWindow_->SetTitle(title);
        }
    } else if (dispId == DISPID_NAVIGATEERROR) {
        LOG_DEBUG << "BrowserEvents2::NavigateError()";
        if (pDispParams->cArgs != 5) {
            LOG_WARNING << "BrowserEvents2::NavigateError() failed: "
                    "Wrong number of arguments, expected 5";
            return DISP_E_BADPARAMCOUNT;
        }
        // pDisp
        _ASSERT(pDispParams->rgvarg[4].vt == VT_DISPATCH); 
        // URL
        _ASSERT(pDispParams->rgvarg[3].vt == (VT_VARIANT | VT_BYREF)); 
        _ASSERT(pDispParams->rgvarg[3].pvarVal->vt == VT_BSTR);
        // TargetFrameName
        _ASSERT(pDispParams->rgvarg[2].vt == (VT_VARIANT | VT_BYREF)); 
        _ASSERT(pDispParams->rgvarg[2].pvarVal->vt == VT_BSTR);
        // StatusCode
        _ASSERT(pDispParams->rgvarg[1].vt == (VT_VARIANT | VT_BYREF));
        _ASSERT(pDispParams->rgvarg[1].pvarVal->vt == VT_I4);
        // Cancel
        _ASSERT(pDispParams->rgvarg[0].vt == (VT_BOOL | VT_BYREF)); 

        const wchar_t* navigateUrl = pDispParams->rgvarg[3].pvarVal->bstrVal;
        int statusCode = pDispParams->rgvarg[1].pvarVal->lVal;
        
        if (browserWindow_->DisplayErrorPage(navigateUrl, statusCode)) {
            *pDispParams->rgvarg[0].pboolVal = VARIANT_TRUE;
            return S_OK;
        } else {
            *pDispParams->rgvarg[0].pboolVal = VARIANT_FALSE;
            return S_OK;
        }
    }
    return S_OK;
}    
