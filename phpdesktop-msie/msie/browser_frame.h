// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

// Complete documentation on programming IE interfaces:
// http://msdn.microsoft.com/en-us/library/ie/aa752038(v=vs.85).aspx

// WebBrowser customization:
// http://msdn.microsoft.com/en-us/library/aa770041(v=vs.85).aspx

// WebBrowser example C code:
// http://www.codeproject.com/Articles/3365/Embed-an-HTML-control
// Download source zip, see dll/dll.c

// C# WebBrowser control:
// http://code.google.com/p/csexwb2/source/browse/trunk/cEXWB.cs

// Embedding ActiveX (IOle interfaces):
// http://cpansearch.perl.org/src/GMPASSOS/Wx-ActiveX-0.05/activex/wxactivex.cpp

// C++ strings (variant, bstr, others):
// http://www.codeproject.com/Articles/3004/The-Complete-Guide-to-C-Strings

/*
    How to use MSIE browser control:
    1. Before creating window include "internet_features.h" and call
       SetInternetFeatures(), this gets rid of any restrictions in IE browser.
    2. MainFrame should inherit:
       * public BrowserFrame<MainFrame>,
       * public FullscreenFrame<MainFrame> (optional)
    3. MainView member should be named "rootView_".
    4. If you call SetFullscreen() you must call ShowTaskbar() in WM_CLOSE
       message, check isFullscreen: if (isFullscreen) ShowTaskbar(true).
*/

#include <comutil.h>
#include <string>
#include <windows.h>

#include "../settings.h"
#include "browser_frame_interface.h"
#include "ole_client_site.h"
#include "click_events.h"

// TopFrame can be a main window or a popup window.
template <class TopFrame>
class BrowserFrame : public BrowserFrameInterface<TopFrame>
{
public:
    TopFrame* topFrame_;
    OleClientSite<TopFrame> oleClientSite_;
    ExternalDispatch<TopFrame> externalDispatch_;
    ClickEvents<TopFrame> clickEvents_;
    bool clickEventsAttached_;
    CComBSTR documentUniqueID_;
    _variant_t clickDispatch_;    
    wchar_t allowedUrl_[2084];
    CComQIPtr<IWebBrowser2> webBrowser2_;
    DWORD dWebBrowserEvents2Cookie_;    

    BrowserFrame() 
            : topFrame_(static_cast<TopFrame*>(this)),
            oleClientSite_(topFrame_, this),
            externalDispatch_(topFrame_),
            clickEvents_(topFrame_),
            clickEventsAttached_(false),
            documentUniqueID_(),
            clickDispatch_(),
            // allowedUrl_ (initialized in constructor)
            // webBrowser2_
            dWebBrowserEvents2Cookie_(0) {
        clickDispatch_.vt = VT_DISPATCH;
        clickDispatch_.pdispVal = static_cast<IDispatch*>(&clickEvents_);
        wcsncpy_s(allowedUrl_, _countof(allowedUrl_), L"nohttp", _TRUNCATE);
    }
    bool CreateBrowser(const wchar_t* navigateUrl) {
        HRESULT hr; 
        BOOL b;
        json_value* settings = GetApplicationSettings();

        // Create browser control.
        hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC,
                              IID_IWebBrowser2, (void**)&webBrowser2_);
        if (FAILED(hr)) {
            LOG(logERROR) << "BrowserFrame::CreateBrowser() failed: "
                             "CoCreateInstance(CLSID_WebBrowser) failed";
            return false;
        }
        CComQIPtr<IOleObject> oleObject;
        webBrowser2_->QueryInterface(IID_IOleObject, (void**)&oleObject);
        if (FAILED(hr) || !oleObject) {
            LOG(logERROR) << "BrowserFrame::CreateBrowser() failed: "
                             "QueryInterface(IID_IOleObject) failed";
            return false;
        }
        hr = oleObject->SetClientSite(&oleClientSite_);
        if (FAILED(hr)) {
            LOG(logERROR) << "BrowserFrame::CreateBrowser() failed: "
                             "SetClientSite() failed";
            return false;
        }
        RECT rect;
        b = topFrame_->GetClientRect(&rect);
        if (!b) {
            LOG(logERROR) << "BrowserFrame::CreateBrowser() failed: "
                             "TopFrame->GetClientRect() failed";
            return false;
        }
        hr = oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, 
                               static_cast<IOleClientSite*>(&oleClientSite_), 
                               0, GetWindowHandle(), &rect);
        if (FAILED(hr)) {
            LOG(logERROR) << "BrowserFrame::CreateBrowser() failed: "
                             "DoVerb(OLEIVERB_INPLACEACTIVATE) failed";
            return false;
        }
        bool hide_dialog_boxes = (*settings)["msie"]["hide_dialog_boxes"];
        if (hide_dialog_boxes) {
            hr = webBrowser2_->put_Silent(VARIANT_TRUE);
            if (FAILED(hr)) {
                LOG(logWARNING) << "Hiding dialog boxes failed";
            }
        }
        // Position display area.
        webBrowser2_->put_Left(0);
        webBrowser2_->put_Top(0);
        webBrowser2_->put_Width(rect.right);
        webBrowser2_->put_Height(rect.bottom);
        // Do not allow displaying files dragged into the window.
        hr = webBrowser2_->put_RegisterAsDropTarget(VARIANT_FALSE);
        if (FAILED(hr)) {
            LOG(logWARNING) << "BrowserFrame::CreateBrowser(): "
                            << "put_RegisterAsDropTarget(False) failed";
        }
        // AdviseEvent() takes care of logging errors.
        AdviseEvent(webBrowser2_, DIID_DWebBrowserEvents2,
                    &dWebBrowserEvents2Cookie_);
        // Navigate.
        _variant_t varUrl = navigateUrl;
        webBrowser2_->Navigate2(&varUrl, 0, 0, 0, 0);
        return true;
    }
    void CloseBrowser() {
        UnadviseEvent(webBrowser2_, DIID_DWebBrowserEvents2,
                      dWebBrowserEvents2Cookie_);
        webBrowser2_->Quit();
        webBrowser2_.Release();
    }
    bool AttachClickEvents() {
        // Attach OnClick event - to catch clicking any external
        // links. Returns whether succeeded to attach click events,
        // it is required for the DOM to be ready, call this
        // function in a timer until it succeeds.After browser
        // navigation these click events need to be re-attached.
        HRESULT hr;        
        VARIANT_BOOL isBusy;
        hr = webBrowser2_->get_Busy(&isBusy);
        // This may fail when window is loading/unloading.
        if (FAILED(hr) || isBusy == VARIANT_TRUE) {
            return false;
        }
        CComQIPtr<IDispatch> dispatch;
        hr = webBrowser2_->get_Document(&dispatch);
        // This may fail when window is loading.
        if (FAILED(hr) || !dispatch) {
            return false;
        }
        CComQIPtr<IHTMLDocument3> htmlDocument3;
        hr = dispatch->QueryInterface(IID_IHTMLDocument3,
                                      (void**)&htmlDocument3);
        if (FAILED(hr) || !htmlDocument3) {
            LOG(logWARNING) << "BrowserFrame::AttachClickEvents() failed "
                               "QueryInterface(IHTMLDocument3) failed";
            return false;
        }
        CComQIPtr<IHTMLElement> htmlElement;
        hr = htmlDocument3->get_documentElement(&htmlElement);
        if (FAILED(hr) || !htmlElement) {
            LOG(logWARNING) << "BrowserFrame::AttachClickEvents() failed "
                               "get_documentElement() failed";
            return false;
        }
        CComBSTR documentID;
        hr = htmlElement->get_id(&documentID.m_str);
        if (FAILED(hr)) {
            LOG(logWARNING) << "BrowserFrame::AttachClickEvents() failed "
                               "htmlElement->get_id() failed";
            return false;
        }
        if (documentID.Length() && documentID == this->documentUniqueID_) {
            return true;
        } else {
            // Document's identifier changed, browser navigated.
            this->clickEventsAttached_ = false;
            CComBSTR uniqueID;
            hr = htmlDocument3->get_uniqueID(&uniqueID.m_str);
            if (FAILED(hr)) {
                LOG(logWARNING) << "BrowserFrame::AttachClickEvents() failed "
                                   "htmlDocument3->get_uniqueID() failed";
                return false;
            }
            hr = htmlElement->put_id(uniqueID.m_str);
            if (FAILED(hr)) {
                LOG(logWARNING) << "BrowserFrame::AttachClickEvents() failed "
                                   "htmlElement->put_id() failed";
                return false;
            }
            this->documentUniqueID_.AssignBSTR(uniqueID.m_str);
        }
        if (this->clickEventsAttached_) {
            return true;
        }
        CComQIPtr<IHTMLDocument2> htmlDocument2;
        hr = dispatch->QueryInterface(IID_IHTMLDocument2,
                                      (void**)&htmlDocument2);
        if (FAILED(hr) || !htmlDocument2) {
            LOG(logWARNING) << "BrowserFrame::AttachClickEvents() failed "
                               "QueryInterface(IHTMLDocument2)";
            return false;
        }
        hr = htmlDocument2->put_onclick(clickDispatch_);
        if (FAILED(hr)) {
            LOG(logWARNING) << "BrowserFrame::AttachClickEvents() failed "
                               "htmlDocument2->put_onclick() failed";
            return false;
        }
        this->clickEventsAttached_ = true;
        return true;
    }
    bool AdviseEvent(CComQIPtr<IWebBrowser2> webBrowser2, REFIID riid,
                     DWORD* adviseCookie) {
        CComQIPtr<IConnectionPointContainer> connectionPointContainer;
        HRESULT hr = webBrowser2->QueryInterface(IID_IConnectionPointContainer,
                (void**)&connectionPointContainer);
        if (FAILED(hr) || !connectionPointContainer) {
            LOG(logWARNING) << "BrowserFrame::AdviseEvent() failed: "
                    "QueryInterface(IConnectionPointContainer) failed";
            return false;
        }
        CComQIPtr<IConnectionPoint> connectionPoint;
        hr = connectionPointContainer->FindConnectionPoint(riid,
                &connectionPoint);
        if (FAILED(hr) || !connectionPoint) {
            LOG(logWARNING) << "BrowserFrame::AdviseEvent() failed: "
                             "FindConnectionPoint() failed";
            return false;
        }
        CComPtr<IUnknown> unknown;
        hr = oleClientSite_.QueryInterface(IID_IUnknown, (void**)&unknown);
        if (FAILED(hr) || !unknown) {
            LOG(logWARNING) << "BrowserFrame::AdviseEvent() failed: "
                               "QueryInterface(IUnknown) failed";
            return false;
        }
        hr = connectionPoint->Advise(unknown, adviseCookie);
        if (FAILED(hr) && !(*adviseCookie)) {
            LOG(logWARNING) << "BrowserFrame::AdviseEvent() failed: "
                               "connectionPoint->Advise() failed";
            return false;
        }
        return true;
    }
    bool UnadviseEvent(CComQIPtr<IWebBrowser2> webBrowser2, REFIID riid,
                     DWORD adviseCookie) {
        CComQIPtr<IConnectionPointContainer> connectionPointContainer;
        HRESULT hr = webBrowser2->QueryInterface(IID_IConnectionPointContainer,
                (void**)&connectionPointContainer);
        if (FAILED(hr) || !connectionPointContainer) {
            LOG(logDEBUG) << "BrowserFrame::UnaviseEvent() failed: "
                    << "QueryInterface(IConnectionPointContainer) failed";
            return false;
        }
        CComQIPtr<IConnectionPoint> connectionPoint;
        hr = connectionPointContainer->FindConnectionPoint(riid,
                &connectionPoint);
        if (FAILED(hr) || !connectionPoint) {
            LOG(logDEBUG) << "BrowserFrame::UnadviseEvent() failed: "
                    << "FindConnectionPoint() failed";
            return false;
        }
        hr = connectionPoint->Unadvise(adviseCookie);
        if (FAILED(hr)) {
            LOG(logDEBUG) << "BrowserFrame::UnadviseEvent() failed: "
                    << "Unadvise() failed";
            return false;
        }
        return true;
    }
    HWND GetWindowHandle() {
        _ASSERT(topFrame_->m_hWnd);
        return topFrame_->m_hWnd;
    }
    IOleClientSite* GetOleClientSite() {
        return static_cast<IOleClientSite*>(&oleClientSite_);
    }
    CComQIPtr<IWebBrowser2> GetBrowser() {
        _ASSERT(webBrowser2_);
        return webBrowser2_;
    }
    bool GetActiveHtmlElement(wchar_t* outTag, int outTagSize,
                              wchar_t* outType, int outTypeSize) {
        _ASSERT(outTagSize);
        _ASSERT(outTypeSize);
        // In case this function fails & returns false.
        if (outTagSize) outTag[0] = 0; else outTag = 0;
        if (outTypeSize) outType[0] = 0; else outType = 0;
        HRESULT hr;
        int ires;

        CComQIPtr<IDispatch> dispatch;
        hr = webBrowser2_->get_Document(&dispatch);
        if (FAILED(hr) || !dispatch) {
            LOG(logWARNING) << "BrowserFrame::GetActiveHtmlElement() failed "
                               "get_Document() failed";
            return false;
        }
        CComQIPtr<IHTMLDocument2> htmlDocument2;
        hr = dispatch->QueryInterface(IID_IHTMLDocument2, 
                                      (void**)&htmlDocument2);
        if (FAILED(hr) || !htmlDocument2) {
            LOG(logWARNING) << "BrowserFrame::GetActiveHtmlElement() failed "
                               "QueryInterface(IHTMLDocument2) failed";
            return false;
        }
        CComQIPtr<IHTMLElement> htmlElement;
        hr = htmlDocument2->get_activeElement(&htmlElement);
        if (FAILED(hr) || !htmlElement) {
            LOG(logWARNING) << "BrowserFrame::GetActiveHtmlElement() failed "
                               "get_activeElement() failed";
            return false;
        }
        CComBSTR tag;
        hr = htmlElement->get_tagName(&tag);
        if (FAILED(hr)) {
            LOG(logWARNING) << "BrowserFrame::GetActiveHtmlElement() failed "
                               "get_tagName() failed";
            return false;
        }
        ires = swprintf_s(outTag, outTagSize, L"%s", tag.m_str);
        if (-1 == ires)
            return false;
        CComBSTR type(L"type");
        VARIANT attrvalue;
        VariantInit(&attrvalue);
        hr = htmlElement->getAttribute(type, 0 | 2, &attrvalue);
        if (FAILED(hr)) {
            LOG(logWARNING) << "BrowserFrame::GetActiveHtmlElement() failed"
                               "getAttribute() failed";
            return false;
        }
        if (attrvalue.vt == VT_BSTR) {
            ires = swprintf_s(outType, outTypeSize, L"%s", attrvalue.bstrVal);
            if (-1 == ires)
                return false;
        } else {
            ires = swprintf_s(outType, outTypeSize, L"%s", L"");
            if (-1 == ires)
                return false;
        }
        return true;
    }
    int ExternalIdentifier(wchar_t* function) {
        LOG(logDEBUG) << "BrowserFrame::ExternalIdentifier(): function = "
                << WideToUtf8(function);
        return 0;
    }
    bool ExternalCall(int functionIdentifier) {
        return false;
    }
    void SetAllowedUrl(const wchar_t* inUrl) {
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
    bool IsUrlAllowed(wchar_t* inUrl, int sizeInWords) {
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
    void SetWidth(long width) {
        RECT rect;
        topFrame_->GetWindowRect(&rect);
        int height = rect.bottom - rect.top;
        topFrame_->SetWindowPos(HWND_TOP, 0, 0, width, height,
                SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
    }
    void SetHeight(long height) {
        RECT rect;
        topFrame_->GetWindowRect(&rect);
        int width = rect.right - rect.left;
        topFrame_->SetWindowPos(HWND_TOP, 0, 0, width, height,
                SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
    }
    void SetTop(long top) {
        RECT rect;
        topFrame_->GetWindowRect(&rect);
        topFrame_->SetWindowPos(HWND_TOP, top, rect.left, 0, 0,
                SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }
    void SetLeft(long left) {
        RECT rect;
        topFrame_->GetWindowRect(&rect);
        topFrame_->SetWindowPos(HWND_TOP, rect.top, left, 0, 0,
                SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }
    void SetTitle(const wchar_t* title) {
        topFrame_->SetWindowText(title);
    }
    virtual bool IsPopup() = 0;
    virtual bool IsUsingMetaTitle() = 0;
};
