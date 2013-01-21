// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

/*
    How to use MSIE browser:

    1. You should include "debug.h" which defines macro ASSERT_EXIT(bool, msb).
    2. Before creating window include "internet_features.h" and call
       SetInternetFeatures(), this gets rid of any restrictions in IE browser.
    3. MainFrame should inherit:
       * public BrowserFrame<MainFrame>,
       * public FullscreenFrame<MainFrame>
    4. MainView member should be named "rootView".
    5. If you call SetFullscreen() you must call ShowTaskbar() in WM_CLOSE
       message, check isFullscreen: if (isFullscreen) ShowTaskbar(true).
*/

#include <comutil.h>
#include <string>

#include "browser_frame_interface.h"
#include "ole_client_site.h"
#include "dochost_ui_handler_dispatch.h"
#include "click_events.h"

// RootFrame can be a main window or a popup window.
template <class RootFrame, bool t_bHasSip = true>
class BrowserFrame
    :
    public BrowserFrameInterface<RootFrame>
{
public:

    OleClientSite<RootFrame> oleClientSite_;
    DocHostUIHandlerDispatch<RootFrame> docHostUIHandlerDispatch_;
    ClickEvents<RootFrame> clickEvents_;
    bool clickEventsAttached_;
    CComBSTR documentUniqueID_;
    _variant_t clickDispatch_;
    RootFrame* rootFrame_;
    // Default: "nohttp" - if url starts with http:// it will
    // be opened in default browser, not in webbrowser control.
    wchar_t allowedUrl_[2084]; 

    BrowserFrame()
        :
        oleClientSite_((BrowserFrameInterface<RootFrame>*)this),
        docHostUIHandlerDispatch_(DocHostUIHandlerDispatch<RootFrame>(
                (IOleClientSite*)&oleClientSite_,
                (BrowserFrameInterface<RootFrame>*)this)),
        clickEvents_(ClickEvents<RootFrame>(static_cast<RootFrame*>(this)))
    {
        rootFrame_ = static_cast<RootFrame*>(this);
        oleClientSite_.dispatch_ = (IDispatch*)&docHostUIHandlerDispatch_;

         // _variant_t declared in <comutil.h>
        clickDispatch_.vt = VT_DISPATCH;
        clickDispatch_.pdispVal = (IDispatch*)&clickEvents_;
        wcsncpy_s(allowedUrl_, _countof(allowedUrl_), L"nohttp", _TRUNCATE);
    }

    void CreateBrowser(const wchar_t* navigateurl)
    {
        // m_hWndClient is of WTL::CFrameWindowImplBase
        rootFrame_->m_hWndClient = rootFrame_->rootView.Create(
                rootFrame_->m_hWnd,
                rootFrame_->rcDefault, (LPCTSTR) navigateurl, (WS_CHILD |
                WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL
                | WS_VSCROLL), WS_EX_CLIENTEDGE);
        ASSERT_EXIT(rootFrame_->m_hWndClient, "rootFrame_->m_hWndClient");

        HRESULT hr;

        CComQIPtr<IOleObject> oleObject;
        int ctrlid = rootFrame_->rootView.GetDlgCtrlID();
        CComQIPtr<IWebBrowser2> webBrowser2;

        hr = rootFrame_->GetDlgControl(ctrlid, IID_IWebBrowser2, 
                                       (void**)&webBrowser2);
        ASSERT_EXIT(SUCCEEDED(hr), 
                "rootFrame_->GetDlgControl(IID_IWebBrowser2) failed");
        ASSERT_EXIT(!!webBrowser2, "webBrowser2 is empty");

        hr = webBrowser2->QueryInterface(IID_IOleObject, (void**)&oleObject);
        ASSERT_EXIT(SUCCEEDED(hr), 
                "webBrowser2->QueryInterface(IID_IOleObject)");
        ASSERT_EXIT(!!oleObject, "oleObject is empty");

        hr = oleObject->SetClientSite(&oleClientSite_);
        ASSERT_EXIT(SUCCEEDED(hr), "oleObject->SetClientSite()");

        hr = rootFrame_->rootView.SetExternalUIHandler(
                &docHostUIHandlerDispatch_);
        ASSERT_EXIT(SUCCEEDED(hr), 
                "rootFrame_->rootView.SetExternalUIHandler()");

        // Do not allow displaying files dragged into the window.
        hr = webBrowser2->put_RegisterAsDropTarget(VARIANT_FALSE);
        ASSERT_EXIT(SUCCEEDED(hr), "webBrowser2->put_RegisterAsDropTarget()");

        /*
        CComQIPtr<IHTMLWindow2> htmlWindow2;
        hr = htmlDocument2->get_parentWindow((IHTMLWindow2**) &htmlWindow2);
        ASSERT_EXIT(SUCCEEDED(hr), 
                "htmlDocument2->get_parentWindow(&htmlWindow2)");

        CComQIPtr<IHTMLWindow3> htmlWindow3;
        hr = htmlWindow2->QueryInterface(IID_IHTMLWindow3, 
                                         (void**) &htmlWindow3);
        ASSERT_EXIT(SUCCEEDED(hr), 
                "htmlWindow2->QueryInterface(IID_IHTMLWindow3)");

        // onbeforeunload (before the connection is made)
        // onunload (after initial connection is made, but before redirecting).
        CComBSTR onClick(TEXT( "onbeforeunload"));
        VARIANT_BOOL result = VARIANT_TRUE;
        hr = htmlWindow3->attachEvent(onClick, &clickEvents_, &result);
        ASSERT_EXIT((result == VARIANT_TRUE), "window3->attachEvent(onClick)");
        */
    }

    bool AttachClickEvents()
    {
        // Attach OnClick event - to catch clicking any external links.

        // Returns whether succeeded to attach click events,
        // it is required for the DOM to be ready, call this
        // function in a timer until it succeeds.

        // After browser navigation these click events need to be
        // re-attached.

        HRESULT hr;

        int ctrlid = rootFrame_->rootView.GetDlgCtrlID();
        CComQIPtr<IWebBrowser2> webBrowser2;

        hr = rootFrame_->GetDlgControl(ctrlid, IID_IWebBrowser2, 
                                      (void**)&webBrowser2);
        ASSERT_EXIT(SUCCEEDED(hr), 
                "rootFrame_->GetDlgControl(IID_IWebBrowser2) failed");
        ASSERT_EXIT(!!webBrowser2, "webBrowser2 is empty");

        VARIANT_BOOL isBusy;
        hr = webBrowser2->get_Busy(&isBusy);
        ASSERT_EXIT(SUCCEEDED(hr), "webBrowser2->get_Busy()");
        if (isBusy == VARIANT_TRUE)
            return false;

        CComQIPtr<IDispatch> dispatch;
        hr = webBrowser2->get_Document(&dispatch);
        ASSERT_EXIT(SUCCEEDED(hr), "webBrowser2->get_Document(&dispatch)");
        if (!dispatch)
            return false;

        CComQIPtr<IHTMLDocument3> htmlDocument3;
        hr = dispatch->QueryInterface(IID_IHTMLDocument3, 
                                      (void**)&htmlDocument3);
        ASSERT_EXIT(SUCCEEDED(hr), "dispatch->QueryInterface(&htmlDocument3)");
        ASSERT_EXIT(!!htmlDocument3, "htmlDocument3 is empty");

        CComQIPtr<IHTMLElement> htmlElement;
        hr = htmlDocument3->get_documentElement(&htmlElement);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlDocument3->get_documentElement()");
        ASSERT_EXIT(!!htmlElement, "htmlElement is empty");

        CComBSTR documentID;
        hr = htmlElement->get_id(&documentID.m_str);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlElement->get_id()");

        if (documentID.Length() && documentID == this->documentUniqueID_) {
            return true;
        } else {
            // Document's identifier changed, browser navigated.
            this->clickEventsAttached_ = false;

            CComBSTR uniqueID;
            hr = htmlDocument3->get_uniqueID(&uniqueID.m_str);
            ASSERT_EXIT(SUCCEEDED(hr), "htmlDocument3->get_uniqueID()");

            hr = htmlElement->put_id(uniqueID.m_str);
            ASSERT_EXIT(SUCCEEDED(hr), "htmlElement->put_id()");

            this->documentUniqueID_.AssignBSTR(uniqueID.m_str);
        }

        if (this->clickEventsAttached_)
            return true;

        CComQIPtr<IHTMLDocument2> htmlDocument2;
        hr = dispatch->QueryInterface(IID_IHTMLDocument2, 
                                      (void**)&htmlDocument2);
        ASSERT_EXIT(SUCCEEDED(hr), "dispatch->QueryInterface(&htmlDocument2)");
        ASSERT_EXIT(!!htmlDocument2, "htmlDocument2 is empty");

        htmlDocument2->put_onclick(clickDispatch_);
        this->clickEventsAttached_ = true;
        return true;
    }

    BOOL OpenWindow(IWebBrowser2* pWebBrowser
                    /*, CString strUrl, CString strName, CString strFeatures*/)
    {
        if(pWebBrowser != NULL)
        {
            CComDispatchDriver pDocDisp;
            if(SUCCEEDED(pWebBrowser->get_Document(&pDocDisp)))
            {   
                CComQIPtr<IHTMLDocument2> pDoc = pDocDisp;
                if(pDoc != NULL)
                {
                    CComPtr<IHTMLWindow2> pWindow;
                    if(SUCCEEDED(pDoc->get_parentWindow(&pWindow)))
                    {
                        CComPtr<IHTMLWindow2> pWindowResult;
                        return TRUE;
                        //return SUCCEEDED(pWindow->open(CComBSTR(strUrl), 
                                //CComBSTR(strName), CComBSTR(strFeatures), 
                                //VARIANT_FALSE, &pWindowResult));
                    }
                }
            }           
        }
        return FALSE;
    }

    virtual HWND GetWindowHandle()
    {
        return rootFrame_->m_hWnd;
    }

    virtual HWND RootView_GetWindowHandle()
    {
        return rootFrame_->rootView.m_hWnd;
    }

    virtual IOleClientSite* GetOleClientSite()
    {
        return &oleClientSite_;
    }

    virtual int RootView_GetDlgCtrlID()
    {
        return rootFrame_->rootView.GetDlgCtrlID();
    }

    virtual HRESULT RootFrame_GetDlgControl(int nID, REFIID iid, void** ppCtrl)
    {
        return rootFrame_->GetDlgControl(nID, iid, ppCtrl);
    }

    virtual bool GetActiveElement(wchar_t* outTag, wchar_t* outTypeAttr)
    {
        CComQIPtr<IWebBrowser2> webBrowser;
        HRESULT hr = rootFrame_->GetDlgControl(
                rootFrame_->rootView.GetDlgCtrlID(), IID_IWebBrowser2, 
                (void**) &webBrowser);
        ASSERT_EXIT(SUCCEEDED(hr), 
                "rootFrame_->GetDlgControl(IID_IWebBrowser2) failed");

        CComQIPtr<IDispatch> dispatch;
        hr = webBrowser->get_Document(&dispatch);
        ASSERT_EXIT(SUCCEEDED(hr), "webBrowser->getDocument()");
        if (dispatch == NULL) {
            outTag = 0;
            return false;
        }

        CComQIPtr<IHTMLDocument2> htmlDocument2;
        hr = dispatch->QueryInterface(IID_IHTMLDocument2, (void**) &htmlDocument2);
        ASSERT_EXIT(SUCCEEDED(hr), "dispatch->QueryInterface(IID_IHTMLDocument2)");
        if (htmlDocument2 == NULL) {
            // Is it possible to return NULL for document?
            outTag = 0;
            return false;
        }

        CComQIPtr<IHTMLElement> htmlElement;
        hr = htmlDocument2->get_activeElement(&htmlElement);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlDocument2->get_ActiveElement()");
        if (htmlElement == NULL) {
            // htmlElement might be NULL when document is not yet loaded.
            outTag = 0;
            return false;
        }

        CComBSTR tag;
        hr = htmlElement->get_tagName(&tag);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlElement->get_tagName()");
        swprintf_s(outTag, 50, L"%s", tag.m_str);

        CComBSTR type(L"type");
        VARIANT attrvalue;
        VariantInit(&attrvalue);
        hr = htmlElement->getAttribute(type, 0 | 2, &attrvalue);
        ASSERT_EXIT(SUCCEEDED(hr), "htmlElement->getAttribute()");

        // Other way of doing this (found example in google):
        // hr = VariantChangeType(&attrvalue, &attrvalue, VARIANT_LOCALBOOL, VT_BSTR);
        // ASSERT_EXIT(SUCCEEDED(hr), "VariantChangeType()");
        // DEBUG_INT(attrvalue.vt); // (body)1==VT_NULL, (input)8==VT_BSTR
        // ASSERT_EXIT((attrvalue.vt == VT_BSTR), "attrvalue.vt != VT_BSTR");

        if (attrvalue.vt == VT_BSTR) {
            swprintf_s(outTypeAttr, 50, L"%s", attrvalue.bstrVal);
        } else {
            swprintf_s(outTypeAttr, 50, L"%s", L"");
        }

        return true;
    }

    int ExternalIdentifier(wchar_t* functionName)
    {
        return 0;
    }

    bool ExternalCall(int functionIdentifier)
    {
        return false;
    }

    void SetAllowedUrl(const wchar_t* inUrl)
    {
        // * Call this function in OnCreate().
        // * Links that do not start with outURL will not be allowed to open
        //   in webbrowser control, instead they will be opened in default
        //   browser by calling "system(start http://...)".
        // * To disallow opening any urls in webbrowser control return here
        //   "nohttp" (this is the default).
        // * If your application is a local http server set it for example to
        //   "http://127.0.0.1:12345/".
        // * If you need more control over allowed urls overwrite
        //   IsUrlAllowed().
        wcsncpy_s(allowedUrl_, _countof(allowedUrl_), inUrl, _TRUNCATE);
    }

    bool IsUrlAllowed(wchar_t* inUrl, int sizeInWords)
    {
        wchar_t* URL_lower = new wchar_t[sizeInWords];
        wcsncpy_s(URL_lower, sizeInWords, inUrl, _TRUNCATE);
        _wcslwr_s(URL_lower, sizeInWords);

        bool ret = false;
        std::wstring URL = URL_lower;

        if (0 == wcscmp(allowedUrl_, L"nohttp")) {
            // Disallow: http://, https:// - case insensitive.
            if (0 == URL.compare(0, wcslen(L"http://"), L"http://")
                || 0 == URL.compare(0, wcslen(L"https://"), L"https://"))
            {
                ret = false;
            } else {
                ret = true;
            }
        } else {
            if (0 == URL.compare(0, wcslen(allowedUrl_), allowedUrl_)) {
                ret = true;
            } else {
                ret = false;
            }
        }

        delete[] URL_lower;

        return ret;
    }
};
