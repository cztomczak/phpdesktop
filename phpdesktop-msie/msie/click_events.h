// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

// ClickEvents is attached to document's onclick event, see
// BrowserFrame::AttachClickEvents().
//
// Do not allow navigating to external websites using the
// main browser window, subframes are allowed to navigate to
// external pages.
//
// ClickEvents - control over clicked links.
// SubmitEvents - control submitted forms.
// Use of window.location cannot be controlled.
//
// TODO: Another protection against external navigation in main
// browser window would be through IHTMLWindow3.attachEvent("onunload"),
// combined with DWebBrowserEvents2::BeforeNavigate2(), we should
// only allow BeforeNavigate2() when it's not preceded with onunload event.

template <class TopFrame>
class ClickEvents : public IDispatch {
public:
    TopFrame* topFrame_;

    ClickEvents(TopFrame* inTopFrame)
        : topFrame_(inTopFrame) {
    }
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) {
        *ppvObject = 0;
        return E_NOTIMPL;
        return topFrame_->GetOleClientSite()->QueryInterface(riid, ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }
    // IDispatch
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

        CComQIPtr<IWebBrowser2> webBrowser2 = topFrame_->GetBrowser();
        CComQIPtr<IDispatch> documentDispatch;
        hr = webBrowser2->get_Document(&documentDispatch);
        if (FAILED(hr) || !documentDispatch) {
            LOG(logWARNING) << "ClickEvents::Invoke() failed: "
                               "get_Document() failed";
            return S_OK;
        }
        CComQIPtr<IHTMLDocument2> htmlDocument2;
        hr = documentDispatch->QueryInterface(IID_IHTMLDocument2, 
                                              (void**)&htmlDocument2);
        if (FAILED(hr) || !htmlDocument2) {
            LOG(logWARNING) << "ClickEvents::Invoke() failed: "
                               "QueryInterface(IHTMLDocument2) failed";
            return S_OK;
        }
        CComQIPtr<IHTMLWindow2> htmlWindow2;
        hr = htmlDocument2->get_parentWindow(
                static_cast<IHTMLWindow2**>(&htmlWindow2));
        if (FAILED(hr) || !htmlWindow2) {
            LOG(logWARNING) << "ClickEvents::Invoke() failed: "
                               "IHTMLDocument2->get_parentWindow() failed";
            return S_OK;
        }
        CComQIPtr<IHTMLEventObj> htmlEvent;
        hr = htmlWindow2->get_event(&htmlEvent);
        if (FAILED(hr) || !htmlEvent) {
            LOG(logWARNING) << "ClickEvents::Invoke() failed: "
                               "IHTMLWindow2->get_event() failed";
            return S_OK;
        }
        CComQIPtr<IHTMLElement> htmlElement;
        hr = htmlEvent->get_srcElement(&htmlElement);
        if (FAILED(hr) || !htmlElement) {
            LOG(logWARNING) << "ClickEvents::Invoke() failed: "
                               "IHTMLEventObj->get_srcElement() failed";
            return S_OK;
        }
        CComBSTR hrefAttr(L"href");
        VARIANT attrValue;
        VariantInit(&attrValue);
        hr = htmlElement->getAttribute(hrefAttr, 0, &attrValue);
        if (FAILED(hr)) {
            LOG(logWARNING) << "ClickEvents::Invoke() failed: "
                               "IHTMLElement->getAttribute() failed";
            return S_OK;
        }
        if (attrValue.vt == VT_BSTR && attrValue.bstrVal) {
            // Href attribute found. When not found vt is VT_NULL.
            // Maximum url length in IE is 2084, see:
            // http://support.microsoft.com/kb/208427
            wchar_t href[2084];
            wcsncpy_s(href, _countof(href), attrValue.bstrVal, _TRUNCATE);
            if (!topFrame_->IsUrlAllowed(href, _countof(href))) {
                VARIANT eventReturn;
                VariantInit(&eventReturn);
                eventReturn.vt = VT_BOOL;
                eventReturn.boolVal = VARIANT_FALSE;
                htmlEvent->put_returnValue(eventReturn);
                ShellExecute(0, L"open", href, 0, 0, SW_SHOWNORMAL);
            }
        }
        return S_OK;
    }
};
