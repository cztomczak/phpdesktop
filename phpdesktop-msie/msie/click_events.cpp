// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// ClickEvents is attached to document's onclick event, see
// BrowserWindow::TryAttachClickEvents().
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

#include "../defines.h"
#include "click_events.h"
#include "browser_window.h"

#include "../log.h"

ClickEvents::ClickEvents(BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow) {
}
// IUnknown
HRESULT STDMETHODCALLTYPE ClickEvents::QueryInterface(
        REFIID riid, void** ppvObject) {
    *ppvObject = 0;
    return E_NOTIMPL;
}
ULONG STDMETHODCALLTYPE ClickEvents::AddRef(void) {
    return 1;
}
ULONG STDMETHODCALLTYPE ClickEvents::Release(void) {
    return 1;
}
// IDispatch
HRESULT STDMETHODCALLTYPE ClickEvents::GetTypeInfoCount( 
        /* [out] */ UINT *pctinfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE ClickEvents::GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ ITypeInfo **ppTInfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE ClickEvents::GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [out] */ DISPID *rgDispId) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE ClickEvents::Invoke( 
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

    const IWebBrowser2Ptr webBrowser2 = browserWindow_->GetWebBrowser2();
    IDispatchPtr documentDispatch;
    hr = webBrowser2->get_Document(&documentDispatch);
    if (FAILED(hr) || !documentDispatch) {
        LOG_WARNING << "ClickEvents::Invoke() failed: "
                       "get_Document() failed";
        return S_OK;
    }
    IHTMLDocument2Ptr htmlDocument2;
    hr = documentDispatch->QueryInterface(IID_IHTMLDocument2, 
                                          (void**)&htmlDocument2);
    if (FAILED(hr) || !htmlDocument2) {
        LOG_WARNING << "ClickEvents::Invoke() failed: "
                       "QueryInterface(IHTMLDocument2) failed";
        return S_OK;
    }
    IHTMLWindow2Ptr htmlWindow2;
    hr = htmlDocument2->get_parentWindow(
            static_cast<IHTMLWindow2**>(&htmlWindow2));
    if (FAILED(hr) || !htmlWindow2) {
        LOG_WARNING << "ClickEvents::Invoke() failed: "
                       "IHTMLDocument2->get_parentWindow() failed";
        return S_OK;
    }
    IHTMLEventObjPtr htmlEvent;
    hr = htmlWindow2->get_event(&htmlEvent);
    if (FAILED(hr) || !htmlEvent) {
        LOG_WARNING << "ClickEvents::Invoke() failed: "
                       "IHTMLWindow2->get_event() failed";
        return S_OK;
    }
    IHTMLElementPtr htmlElement;
    hr = htmlEvent->get_srcElement(&htmlElement);
    if (FAILED(hr) || !htmlElement) {
        LOG_WARNING << "ClickEvents::Invoke() failed: "
                       "IHTMLEventObj->get_srcElement() failed";
        return S_OK;
    }
    _bstr_t hrefAttr(L"href");
    VARIANT attrValue;
    VariantInit(&attrValue);
    hr = htmlElement->getAttribute(hrefAttr, 0, &attrValue);
    if (FAILED(hr)) {
        LOG_WARNING << "ClickEvents::Invoke() failed: "
                       "IHTMLElement->getAttribute() failed";
        return S_OK;
    }
    if (attrValue.vt == VT_BSTR && attrValue.bstrVal) {
        // Href attribute found. When not found vt is VT_NULL.
        // Maximum url length in IE is 2084, see:
        // http://support.microsoft.com/kb/208427
        wchar_t href[2084];
        wcsncpy_s(href, _countof(href), attrValue.bstrVal, _TRUNCATE);
        if (!browserWindow_->IsUrlAllowed(href, _countof(href))) {
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
