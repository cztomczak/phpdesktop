// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

// DWebBrowserEvents2 interface for WebBrowser control:
// http://msdn.microsoft.com/en-us/library/aa768283(v=vs.85).aspx
// (DWebBrowserEvents2 for MSHTML has different methods).

// DISPID_NEWWINDOW3 and others.
#include <ExDispid.h>
#include "browser_frame_interface.h"

template <class TopFrame>
class BrowserEvents2 : public DWebBrowserEvents2
{
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;
public:
    BrowserEvents2(BrowserFrameInterface<TopFrame>* inBrowserFrame)
            : browserFrame_(inBrowserFrame) {
    }
    HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObject) {
        return browserFrame_->GetOleClientSite()->QueryInterface(
                riid, ppvObject);
    }    
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }    
    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }    
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo) {
        *pctinfo = 0;
        return E_NOTIMPL;
    }    
    HRESULT STDMETHODCALLTYPE GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo) {
        *ppTInfo = 0;
        return E_NOTIMPL;
    }    
    HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [out] */ DISPID *rgDispId) {
        *rgDispId = 0;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE Invoke( 
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
            return ResultFromScode(DISP_E_UNKNOWNINTERFACE);
        pExcepInfo = 0;
        puArgErr = 0;
        HRESULT hr;
        
        if (dispId == DISPID_NEWWINDOW3) {
            /* When calling window.open() you get an error "Class
               not registered". Before this error appears 
               DWebBrowserEvents2::NewWindow3 event is dispatched, 
               you need to create the popup window in this event
               and assign the dispatch interface of the new popup
               browser to the first parameter of NewWindow3. */
            LOG(logDEBUG) << "BrowserEvents2::NewWindow3()";            
            PopupFrame* popupFrame = new PopupFrame();
            if (popupFrame->CreateEx(browserFrame_->GetWindowHandle(), NULL)
                    == NULL) {
                LOG(logWARNING) << "popupFrame->CreateEx() failed";
                // Cancel parameter, current navigation should be cancelled.
                *pDispParams->rgvarg[3].pboolVal = VARIANT_TRUE;
                return S_OK;
            }
            popupFrame->ShowWindow(SW_SHOWNORMAL);
            CComQIPtr<IWebBrowser2> webBrowser2 = popupFrame->GetBrowser();
            CComPtr<IDispatch> dispatch;
            hr = webBrowser2->get_Application(&dispatch);
            if (FAILED(hr) || !dispatch) {
                LOG(logWARNING) << "BrowserEvents2::NewWindow3() failed: "
                    "webBrowser2->get_Application() failed";
                return S_OK;
            }
            *pDispParams->rgvarg[3].pboolVal = VARIANT_FALSE;
            *pDispParams->rgvarg[4].ppdispVal = dispatch.Detach();
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
            long width = pDispParams->rgvarg[0].lVal;
            LOG(logDEBUG) << "BrowserEvents2::WindowSetWidth(): width = "
                          << width;
            browserFrame_->SetWidth(width);
        } else if (dispId == DISPID_WINDOWSETHEIGHT) {
            long height = pDispParams->rgvarg[0].lVal;
            LOG(logDEBUG) << "BrowserEvents2::WindowSetHeight(): height = "
                          << height;
            browserFrame_->SetHeight(height);
        } else if (dispId == DISPID_WINDOWSETTOP) {
            long top = pDispParams->rgvarg[0].lVal;
            LOG(logDEBUG) << "BrowserEvents2::WindowSetTop(): top = "
                          << top;
            browserFrame_->SetTop(top);
        } else if (dispId == DISPID_WINDOWSETLEFT) {
            long left = pDispParams->rgvarg[0].lVal;
            LOG(logDEBUG) << "BrowserEvents2::WindowSetLeft(): left = "
                          << left;
            browserFrame_->SetLeft(left);
        } else if (dispId == DISPID_TITLECHANGE) {
            if (browserFrame_->IsPopup()
                    && browserFrame_->IsUsingMetaTitle()) {
                _ASSERT(pDispParams->rgvarg[0].vt == VT_BSTR);
                BSTR title = pDispParams->rgvarg[0].bstrVal;
                LOG(logDEBUG) << "BrowserEvents2::TitleChange(): "
                                 "setting popup title = " << title;
                browserFrame_->SetTitle(title);
            }
        }        
        return S_OK;
    }    
};
