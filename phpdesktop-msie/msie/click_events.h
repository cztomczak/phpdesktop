// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

// Do not allow navigating to external websites using the
// main browser window, subframes inside main window are allowed.

// ClickEvents - control over clicked links.
// SubmitEvents - control submitted forms.
// Use of window.location cannot be controlled.

// Make sure that events from IHTMLWindow3.attachEvent("onclick")
// and IHTMLWindow3.attachEvent("onclick") are happening in current
// document in main window, and not in subframes, as it is allowed
// in subframes to navigate external resources.

// Another protection against external navigation in main
// browser window will be through IHTMLWindow3.attachEvent("onunload")
// combined with DWebBrowserEvents2::BeforeNavigate2() - we will
// only allow BeforeNavigate2() when it's not preceded with onunload event.

// Post the complete solution here:
// http://stackoverflow.com/questions/2925279/iwebbrowser2-how-to-force-links

/*
    DWebBrowserEvents2::BeforeNavigate2()
    DWebBrowserEvents2::NavigateError()
    DWebBrowserEvents2::NewWindow3()
    DWebBrowserEvents2::DocumentComplete()

    IHTMLWindow2::put_onunload(VARIANT p)

    Get IHTMLWindow2:
    1. IWebBrowser2::get_document()
    2. IHTMLDocument2::get_parentWindow()

    IHTMLWindow3::attachEvent(BSTR event, IDispatch *pDisp,
                              VARIANT_BOOL *pfResult)
    IHTMLWindow3::attachEvent(onClick)
    IHTMLWindow3::attachEvent(onSubmit)

    Get IHTMLWindow3:
    1. IWebBrowser2->getdocument
    2. IHTMLDocument2->get_parentWindow
    3. HTMLWindow2->QueryInterface(IHTMLWindow3)

    IHTMLWindow3::attachEvent example:

        CComBSTR onScrollName( TEXT( "onscroll" ) );
        VARIANT_BOOL result = VARIANT_TRUE;
        hr = window3->attachEvent( onScrollName, events, &result );
        if ( result != VARIANT_TRUE ) {}

        class Events : public IDispatch{
            GetTypeInfoCount : E_NOTIMPL
            GetTypeInfo : E_NOTIMPL
            GetIDsOfNames : E_NOTIMPL

            STDMETHODIMP CIE4Events::Invoke(DISPID dispidMember, REFIID riid,
            LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
            VARIANT* pvarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
            {
                if ( !pDispParams ) {
                return E_INVALIDARG;
                }
                if ( _browserControl != NULL &&
                pDispParams->cArgs == 1 && pDispParams->rgvarg [0].vt ==
                VT_DISPATCH ) {

                ComPtrQI <IHTMLEventObj > event( ComPtrQI <IDispatch> (
                pDispParams->rgvarg [0].pdispVal ) );

                if ( event ) {

                event IHTMLEventObj::get_srcElement(IHTMLElement **p);

                event IHTMLEventObj::put_returnValue(VARIANT v)
                false   Default action of the event on the source object is
                        canceled.
            }
        }
*/

template <class RootFrame>
class ClickEvents : public IDispatch
{
public:
    RootFrame* rootFrame_;

    ClickEvents(RootFrame* inRootFrame) {
        rootFrame_ = inRootFrame;
    }

    //
    // IUnknown
    //

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj) {
        *ppvObj = 0;
        return E_NOTIMPL;
    }

    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }

    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }

    //
    // IDispatch
    //

    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo) {
        *pctinfo = 0;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID  lcid,
            ITypeInfo **ppTInfo) {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames,
            UINT cNames, LCID lcid, DISPID *rgDispId) {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid,
            LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
            VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) {
        UINT            uArgErr;
        VARIANT     varResultDummy;

        if (riid != IID_NULL)
            return ResultFromScode(DISP_E_UNKNOWNINTERFACE);

        if (puArgErr == NULL)
            puArgErr = &uArgErr;

        if (pVarResult == NULL)
            pVarResult = &varResultDummy;

        VariantInit(pVarResult);

        if (!pDispParams)
            return E_INVALIDARG;

        HRESULT hr;

        CComQIPtr<IWebBrowser2> webBrowser2;
        hr = rootFrame_->GetDlgControl(rootFrame_->rootView.GetDlgCtrlID(),
                IID_IWebBrowser2, (void**) &webBrowser2);
        ASSERT_EXIT(SUCCEEDED(hr),
                "rootFrame_->GetDlgControl(IID_IWebBrowser2) failed");

        CComQIPtr<IDispatch> dispatch;
        hr = webBrowser2->get_Document(&dispatch);
        ASSERT_EXIT(SUCCEEDED(hr), "webBrowser2->get_Document(&dispatch)");

        CComQIPtr<IHTMLDocument2> htmlDocument2;
        hr = dispatch->QueryInterface(IID_IHTMLDocument2,
                (void**) &htmlDocument2);
        ASSERT_EXIT(SUCCEEDED(hr), "dispatch->QueryInterface(&htmlDocument2)");

        CComQIPtr<IHTMLWindow2> htmlWindow2;
        hr = htmlDocument2->get_parentWindow((IHTMLWindow2**) &htmlWindow2);
        ASSERT_EXIT(SUCCEEDED(hr),
                "htmlDocument2->get_parentWindow(&htmlWindow2)");

        CComQIPtr<IHTMLEventObj> htmlEvent;
        hr = htmlWindow2->get_event(&htmlEvent);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlWindow2->get_event(&htmlEvent)");

        CComQIPtr<IHTMLElement> htmlElement;
        hr = htmlEvent->get_srcElement(&htmlElement);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlEvent->get_srcElement(&htmlElement)");

        CComBSTR hrefAttr(L"href");
        VARIANT attrValue;
        VariantInit(&attrValue);
        // Second param == 0 => by default case insensitive
        hr = htmlElement->getAttribute(hrefAttr, 0, &attrValue);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlElement->getAttribute()");

        if (attrValue.vt == VT_BSTR && attrValue.bstrVal) {
            // Href attribute found, this is VT_NULL when not found,
            // when this check was missing a crash happened on Windows 7
            // while clicking on something else than a link.

            // Maximum url length in IE is 2084, see:
            // http://support.microsoft.com/kb/208427
            wchar_t href[2084];
            wcsncpy_s(href, _countof(href), attrValue.bstrVal, _TRUNCATE);

            if (!rootFrame_->IsUrlAllowed(href, 2084)) {
                VARIANT variant;
                variant.vt = VT_BOOL;
                variant.boolVal = VARIANT_FALSE;
                htmlEvent->put_returnValue(variant);
                ShellExecute(0, L"open", href, 0, 0, SW_SHOWNORMAL);
            }
        }

        return S_OK;
    }
};
