// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

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

#include "../defines.h"
#include "browser_window.h"

#include <map>
#include <string>

#include "../executable.h"
#include "../log.h"
#include "../settings.h"
#include "../string_utils.h"
#include "../fatal_error.h"
#include "../file_utils.h"
#include "../window_utils.h"

std::map<HWND, BrowserWindow*> g_browserWindows;
extern std::string g_webServerUrl;
extern wchar_t g_windowClassName[256];

BrowserWindow* GetBrowserWindow(HWND hwnd) {
    std::map<HWND, BrowserWindow*>::iterator it;
    it = g_browserWindows.find(hwnd);
    if (it != g_browserWindows.end()) {
        return it->second;
    }
    // GetBrowserWindow() may fail during window creation, so log
    // severity is only DEBUG.
    LOG_DEBUG << "GetBrowserWindow(): not found, hwnd = " << (int)hwnd;
    return NULL;
}
void StoreBrowserWindow(HWND hwnd, BrowserWindow* browser) {
    LOG_DEBUG << "StoreBrowserWindow(): hwnd = " << (int)hwnd;
    std::map<HWND, BrowserWindow*>::iterator it;
    it = g_browserWindows.find(hwnd);
    if (it == g_browserWindows.end()) {
        g_browserWindows[hwnd] = browser;
    } else {
        LOG_WARNING << "StoreBrowserWindow() failed: already stored";
    }
}
void RemoveBrowserWindow(HWND hwnd) {
    LOG_DEBUG << "RemoveBrowserWindow(): hwnd = " << (int)hwnd;
    std::map<HWND, BrowserWindow*>::iterator it;
    it = g_browserWindows.find(hwnd);
    if (it != g_browserWindows.end()) {
        BrowserWindow* browser = it->second;
        g_browserWindows.erase(it);
        delete browser;
    } else {
        LOG_WARNING << "RemoveBrowserWindow() failed: not found";
    }
}

BrowserWindow::BrowserWindow(HWND inWindowHandle) 
        : windowHandle_(inWindowHandle),
          parentHandle_(0),
          oleClientSite_(), // initialized in constructor
          externalDispatch_(), // initialized in constructor
          clickEvents_(), // initialized in constructor
          clickEventsAttached_(false),
          documentUniqueID_(),
          clickDispatch_(),
          // allowedUrl_ (initialized in constructor)
          // webBrowser2_
          dWebBrowserEvents2Cookie_(0),
          isResizing_(false),
          focusedAfterCreation_(false) {

    // Passing "this" in member initialization throws a warning,
    // the solution is to initialize these classes in constructor.
    oleClientSite_.reset(new OleClientSite(this));
    externalDispatch_.reset(new ExternalDispatch(this));
    clickEvents_.reset(new ClickEvents(this));
    
    _ASSERT(oleClientSite_);
    _ASSERT(externalDispatch_);
    _ASSERT(clickEvents_);
        
    parentHandle_ = GetParentWindow(windowHandle_);
    LOG_DEBUG << "BrowserWindow(): parentHandle = " << (int)parentHandle_;
    
    clickDispatch_.vt = VT_DISPATCH;
    clickDispatch_.pdispVal = static_cast<IDispatch*>(clickEvents_.get());
    wcsncpy_s(allowedUrl_, _countof(allowedUrl_), L"nohttp", _TRUNCATE);

    _ASSERT(windowHandle_);
    SetTitleFromSettings();
    SetIconFromSettings();
    SetAllowedUrl(Utf8ToWide(g_webServerUrl).c_str());
    if (IsPopup()) {
        if (!CreateBrowserControl(0)) {
            LOG_ERROR << "BrowserWindow::CreateBrowserControl() "
                         "failed";
            _ASSERT(false);
            SendMessage(windowHandle_, WM_CLOSE, 0, 0);
            return;
        }
    } else {
        if (!CreateBrowserControl(Utf8ToWide(g_webServerUrl).c_str())) {
            FatalError(windowHandle_, "Could not create Browser control.\n"
                    "Exiting application.");
        }
    }
}
BrowserWindow::~BrowserWindow() {
    CloseBrowserControl();
}
bool BrowserWindow::CreateBrowserControl(const wchar_t* navigateUrl) {
    // navigateUrl might be NULL, if so Navigate2() won't be called.
    HRESULT hr; 
    BOOL b;
    json_value* settings = GetApplicationSettings();

    // Create browser control.
    hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC_SERVER,
                          IID_IOleObject, (void**)&oleObject_);
    if (FAILED(hr) || !oleObject_) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "CoCreateInstance(CLSID_WebBrowser) failed";
        _ASSERT(false);
        return false;
    }

    hr = oleObject_->SetClientSite(oleClientSite_.get());
    if (FAILED(hr)) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "SetClientSite() failed";
        _ASSERT(false);
        return false;
    }

    RECT rect;
    b = GetClientRect(windowHandle_, &rect);
    if (!b) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "GetClientRect() failed";
        _ASSERT(false);
        return false;
    }

    hr = oleObject_->SetHostNames(g_windowClassName, 0);
    if (FAILED(hr)) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: ";
                     "IOleObject->SetHostNames() failed";
        _ASSERT(false);
        return false;
    }

    hr = OleSetContainedObject(static_cast<IUnknown*>(oleObject_), TRUE);
    if (FAILED(hr)) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "OleSetContaintedObject() failed";
        _ASSERT(false);
        return false;
    }
    
    hr = oleObject_->DoVerb(OLEIVERB_SHOW, NULL,
                           static_cast<IOleClientSite*>(oleClientSite_.get()), 
                           0, windowHandle_, &rect);
    if (FAILED(hr)) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "DoVerb(OLEIVERB_INPLACEACTIVATE) failed";
        _ASSERT(false);
        return false;
    }

    hr = oleObject_->QueryInterface(IID_IOleInPlaceActiveObject, 
            (void**)&oleInPlaceActiveObject_);
    if (FAILED(hr) || !oleInPlaceActiveObject_) {
        LOG_DEBUG << "BrowserWindow::TranslateAccelerator() failed: "
                "IOleObject->QueryInterface(IOleInPlaceActiveObject) failed";
        return false;
    }

    oleObject_->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2_);
    if (FAILED(hr) || !webBrowser2_) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "QueryInterface(IID_IWebBrowser2) failed";
        _ASSERT(false);
        return false;
    }

    bool hide_dialog_boxes = (*settings)["msie"]["hide_dialog_boxes"];
    if (hide_dialog_boxes) {
        hr = webBrowser2_->put_Silent(VARIANT_TRUE);
        if (FAILED(hr)) {
            LOG_WARNING << "Hiding dialog boxes failed";
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
        LOG_WARNING << "BrowserWindow::CreateBrowserControl(): "
                    << "put_RegisterAsDropTarget(False) failed";
    }

    hr = webBrowser2_->put_RegisterAsBrowser(VARIANT_FALSE);
    _ASSERT(SUCCEEDED(hr));

    // AdviseEvent() takes care of logging errors.
    AdviseEvent(webBrowser2_, DIID_DWebBrowserEvents2,
                &dWebBrowserEvents2Cookie_);
    
    // Initial navigation.
    if (navigateUrl) {
        if (!Navigate(navigateUrl))
            return false;
    }

    return true;
}
bool BrowserWindow::Navigate(const wchar_t* navigateUrl) {
    if (!webBrowser2_)
        return false;
    _bstr_t bstrUrl(navigateUrl);
    _variant_t variantFlags((long)(navNoHistory | navNoReadFromCache 
            | navNoWriteToCache), VT_I4);
    HRESULT hr = webBrowser2_->Navigate(bstrUrl, &variantFlags, 0, 0, 0);
    if (FAILED(hr)) {
        LOG_ERROR << "BrowserWindow::Navigate() failed: "
                "IWebBrowser2->Navigate() failed";
        _ASSERT(false);
        return false;
    }
    return true;
}
void BrowserWindow::CloseBrowserControl() {
    if (!webBrowser2_)
        return;
    HRESULT hr;

    UnadviseEvent(webBrowser2_, DIID_DWebBrowserEvents2,
                    dWebBrowserEvents2Cookie_);

    _ASSERT(oleInPlaceActiveObject_);
    oleInPlaceActiveObject_.Release();

    // This is important, otherwise getting unhandled exceptions,
    // scenario: open popup, close it, navigate in main window,
    // exception! after DispatchMessage(), msg.message = 32770.
    DetachClickEvents();

    // Remember to check if webBrowser2_ is not empty before using it,
    // in other functions like TryAttachClickEvents().
    _ASSERT(webBrowser2_);
    hr = webBrowser2_->Stop();
    _ASSERT(SUCCEEDED(hr));
    hr = webBrowser2_->put_Visible(VARIANT_FALSE);
    _ASSERT(SUCCEEDED(hr));        
    // WebBrowser object (CLSID_WebBrowser) cannot call Quit(),
    // it is for Internet Explorer object (CLSID_InternetExplorer).
    // hr = webBrowser2_->Quit();
    // _ASSERT(SUCCEEDED(hr));
    webBrowser2_.Release();

    IOleInPlaceObjectPtr inPlaceObject;
    hr = oleObject_->QueryInterface(IID_IOleInPlaceObject,
            (void**)&inPlaceObject);
    _ASSERT(SUCCEEDED(hr));
    if (SUCCEEDED(hr) && inPlaceObject) {
        hr = inPlaceObject->InPlaceDeactivate();
        _ASSERT(SUCCEEDED(hr));
        // Assertion when using OLEIVERB_SHOW:
        // hr = inPlaceObject->UIDeactivate();
        // _ASSERT(SUCCEEDED(hr));
    }

    // It is important to set client site to NULL, otherwise
    // you will get first-chance exceptions when calling Close().
    _ASSERT(oleObject_);
    hr = OleSetContainedObject(static_cast<IUnknown*>(oleObject_), FALSE);
    _ASSERT(SUCCEEDED(hr));
    hr = oleObject_->DoVerb(OLEIVERB_HIDE, NULL, oleClientSite_.get(), 0, 
            windowHandle_, NULL);
    _ASSERT(SUCCEEDED(hr));
    hr = oleObject_->Close(OLECLOSE_NOSAVE);
    _ASSERT(SUCCEEDED(hr));
    hr = oleObject_->SetClientSite(0);
    _ASSERT(SUCCEEDED(hr));
    hr = CoDisconnectObject(static_cast<IUnknown*>(oleObject_), 0);
    _ASSERT(SUCCEEDED(hr));
    oleObject_.Release();
}
bool BrowserWindow::DetachClickEvents() {
    if (!webBrowser2_)
        return false;
    IDispatchPtr dispatch;
    HRESULT hr = webBrowser2_->get_Document(&dispatch);
    // This may fail when window is loading.
    if (FAILED(hr) || !dispatch) {
        return false;
    }
    IHTMLDocument2Ptr htmlDocument2;
    hr = dispatch->QueryInterface(IID_IHTMLDocument2,
                                  (void**)&htmlDocument2);
    if (FAILED(hr) || !htmlDocument2) {
        LOG_WARNING << "BrowserWindow::DetachClickEvents() failed: "
                       "QueryInterface(IHTMLDocument2)";
        return false;
    }
    _variant_t emptyClickDispatch;
    emptyClickDispatch.vt = VT_DISPATCH;
    emptyClickDispatch.pdispVal = 0;
    // If vt == (VT_BYREF | VT_DISPATCH) then you use ppdispVal:
    // emptyClickDispatch.ppdispVal = 0;
    hr = htmlDocument2->put_onclick(emptyClickDispatch);
    if (FAILED(hr)) {
        LOG_WARNING << "BrowserWindow::DetachClickEvents() failed: "
                       "htmlDocument2->put_onclick() failed";
        return false;
    }
    return true;
}
bool BrowserWindow::TryAttachClickEvents() {
    // Attach OnClick event - to catch clicking any external
    // links. Returns whether succeeded to attach click events,
    // it is required for the DOM to be ready, call this
    // function in a timer until it succeeds.After browser
    // navigation these click events need to be re-attached.
    
    if (!webBrowser2_) {
        // Web-browser control might be closing.
        return false;
    }
    HRESULT hr;        
    VARIANT_BOOL isBusy;
    hr = webBrowser2_->get_Busy(&isBusy);
    // This may fail when window is loading/unloading.
    if (FAILED(hr) || isBusy == VARIANT_TRUE) {
        return false;
    }
    IDispatchPtr dispatch;
    hr = webBrowser2_->get_Document(&dispatch);
    // This may fail when window is loading.
    if (FAILED(hr) || !dispatch) {
        return false;
    }
    IHTMLDocument3Ptr htmlDocument3;
    hr = dispatch->QueryInterface(IID_IHTMLDocument3,
                                  (void**)&htmlDocument3);
    if (FAILED(hr) || !htmlDocument3) {
        LOG_WARNING << "BrowserWindow::TryAttachClickEvents() failed "
                       "QueryInterface(IHTMLDocument3) failed";
        return false;
    }
    IHTMLElementPtr htmlElement;
    hr = htmlDocument3->get_documentElement(&htmlElement);
    if (FAILED(hr) || !htmlElement) {
        LOG_WARNING << "BrowserWindow::TryAttachClickEvents() failed "
                       "get_documentElement() failed";
        return false;
    }
    _bstr_t documentID;
    hr = htmlElement->get_id(&documentID.GetBSTR());
    if (FAILED(hr)) {
        LOG_WARNING << "BrowserWindow::TryAttachClickEvents() failed "
                       "htmlElement->get_id() failed";
        return false;
    }
    if (documentID.length() && documentID == documentUniqueID_) {
        return true;
    } else {
        // Document's identifier changed, browser navigated.
        this->clickEventsAttached_ = false;
        _bstr_t uniqueID;
        hr = htmlDocument3->get_uniqueID(&uniqueID.GetBSTR());
        if (FAILED(hr)) {
            LOG_WARNING << "BrowserWindow::TryAttachClickEvents() "
                           "failed: htmlDocument3->get_uniqueID() failed";
            return false;
        }
        hr = htmlElement->put_id(uniqueID.GetBSTR());
        if (FAILED(hr)) {
            LOG_WARNING << "BrowserWindow::TryAttachClickEvents() "
                           "failed: htmlElement->put_id() failed";
            return false;
        }
        documentUniqueID_.Assign(uniqueID.GetBSTR());
    }
    if (this->clickEventsAttached_) {
        return true;
    }
    IHTMLDocument2Ptr htmlDocument2;
    hr = dispatch->QueryInterface(IID_IHTMLDocument2,
                                  (void**)&htmlDocument2);
    if (FAILED(hr) || !htmlDocument2) {
        LOG_WARNING << "BrowserWindow::TryAttachClickEvents() failed: "
                       "QueryInterface(IHTMLDocument2)";
        return false;
    }
    hr = htmlDocument2->put_onclick(clickDispatch_);
    if (FAILED(hr)) {
        LOG_WARNING << "BrowserWindow::TryAttachClickEvents() failed: "
                       "htmlDocument2->put_onclick() failed";
        return false;
    }
    this->clickEventsAttached_ = true;
    return true;
}
bool BrowserWindow::AdviseEvent(IWebBrowser2Ptr webBrowser2, REFIID riid,
                 DWORD* adviseCookie) {
    IConnectionPointContainerPtr connectionPointContainer;
    HRESULT hr = webBrowser2->QueryInterface(IID_IConnectionPointContainer,
            (void**)&connectionPointContainer);
    if (FAILED(hr) || !connectionPointContainer) {
        LOG_WARNING << "BrowserWindow::AdviseEvent() failed: "
                       "QueryInterface(IConnectionPointContainer) failed";
        return false;
    }
    IConnectionPointPtr connectionPoint;
    hr = connectionPointContainer->FindConnectionPoint(riid,
            &connectionPoint);
    if (FAILED(hr) || !connectionPoint) {
        LOG_WARNING << "BrowserWindow::AdviseEvent() failed: "
                       "FindConnectionPoint() failed";
        return false;
    }
    IUnknownPtr unknown;
    hr = oleClientSite_.get()->QueryInterface(IID_IUnknown, (void**)&unknown);
    if (FAILED(hr) || !unknown) {
        LOG_WARNING << "BrowserWindow::AdviseEvent() failed: "
                       "QueryInterface(IUnknown) failed";
        return false;
    }
    hr = connectionPoint->Advise(unknown, adviseCookie);
    if (FAILED(hr) && !(*adviseCookie)) {
        LOG_WARNING << "BrowserWindow::AdviseEvent() failed: "
                       "connectionPoint->Advise() failed";
        return false;
    }
    return true;
}
bool BrowserWindow::UnadviseEvent(IWebBrowser2Ptr webBrowser2, REFIID riid,
                 DWORD adviseCookie) {
    IConnectionPointContainerPtr connectionPointContainer;
    HRESULT hr = webBrowser2->QueryInterface(IID_IConnectionPointContainer,
            (void**)&connectionPointContainer);
    if (FAILED(hr) || !connectionPointContainer) {
        LOG_DEBUG << "BrowserWindow::UnaviseEvent() failed: "
                  << "QueryInterface(IConnectionPointContainer) failed";
        return false;
    }
    IConnectionPointPtr connectionPoint;
    hr = connectionPointContainer->FindConnectionPoint(riid,
            &connectionPoint);
    if (FAILED(hr) || !connectionPoint) {
        LOG_DEBUG << "BrowserWindow::UnadviseEvent() failed: "
                  << "FindConnectionPoint() failed";
        return false;
    }
    hr = connectionPoint->Unadvise(adviseCookie);
    if (FAILED(hr)) {
        LOG_DEBUG << "BrowserWindow::UnadviseEvent() failed: "
                  << "Unadvise() failed";
        return false;
    }
    return true;
}
HWND BrowserWindow::GetWindowHandle() {
    _ASSERT(windowHandle_);
    return windowHandle_;
}
IOleClientSite* BrowserWindow::GetOleClientSite() {
    return static_cast<IOleClientSite*>(oleClientSite_.get());
}
const IWebBrowser2Ptr BrowserWindow::GetWebBrowser2() {
    _ASSERT(webBrowser2_);
    return webBrowser2_;
}
bool BrowserWindow::GetActiveHtmlElement(wchar_t* outTag, int outTagSize,
                          wchar_t* outType, int outTypeSize) {
    _ASSERT(outTagSize);
    _ASSERT(outTypeSize);
    // In case this function fails & returns false.
    if (outTagSize) outTag[0] = 0; else outTag = 0;
    if (outTypeSize) outType[0] = 0; else outType = 0;
    HRESULT hr;
    int ires;

    IDispatchPtr dispatch;
    hr = webBrowser2_->get_Document(&dispatch);
    if (FAILED(hr) || !dispatch) {
        LOG_WARNING << "BrowserWindow::GetActiveHtmlElement() failed "
                       "get_Document() failed";
        return false;
    }
    IHTMLDocument2Ptr htmlDocument2;
    hr = dispatch->QueryInterface(IID_IHTMLDocument2, 
                                  (void**)&htmlDocument2);
    if (FAILED(hr) || !htmlDocument2) {
        LOG_WARNING << "BrowserWindow::GetActiveHtmlElement() failed "
                       "QueryInterface(IHTMLDocument2) failed";
        return false;
    }
    IHTMLElementPtr htmlElement;
    hr = htmlDocument2->get_activeElement(&htmlElement);
    if (FAILED(hr) || !htmlElement) {
        LOG_WARNING << "BrowserWindow::GetActiveHtmlElement() failed "
                       "get_activeElement() failed";
        return false;
    }
    _bstr_t tag;
    hr = htmlElement->get_tagName(tag.GetAddress());
    if (FAILED(hr)) {
        LOG_WARNING << "BrowserWindow::GetActiveHtmlElement() failed "
                       "get_tagName() failed";
        return false;
    }
    ires = swprintf_s(outTag, outTagSize, L"%s", tag.GetBSTR());
    if (-1 == ires)
        return false;
    _bstr_t type(L"type");
    VARIANT attrvalue;
    VariantInit(&attrvalue);
    hr = htmlElement->getAttribute(type, 0 | 2, &attrvalue);
    if (FAILED(hr)) {
        LOG_WARNING << "BrowserWindow::GetActiveHtmlElement() failed"
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
int BrowserWindow::ExternalIdentifier(wchar_t* function) {
    LOG_DEBUG << "BrowserWindow::ExternalIdentifier(): function = "
              << WideToUtf8(function);
    return 0;
}
bool BrowserWindow::ExternalCall(int functionIdentifier) {
    return false;
}
void BrowserWindow::SetAllowedUrl(const wchar_t* inUrl) {
    wcsncpy_s(allowedUrl_, _countof(allowedUrl_), inUrl, _TRUNCATE);
}
bool BrowserWindow::IsUrlAllowed(const wchar_t* inUrl, int sizeInWords) {
    wchar_t* url_lower = new wchar_t[sizeInWords];
    wcsncpy_s(url_lower, sizeInWords, inUrl, _TRUNCATE);
    _wcslwr_s(url_lower, sizeInWords);
    bool ret = false;
    std::wstring url = url_lower;
    if (0 == wcscmp(allowedUrl_, L"nohttp")) {
        // Disallow: http://, https:// - case insensitive.
        if (0 == url.compare(0, wcslen(L"http://"), L"http://")
            || 0 == url.compare(0, wcslen(L"https://"), L"https://"))
        {
            ret = false;
        } else {
            ret = true;
        }
    } else {
        if (0 == url.compare(0, wcslen(allowedUrl_), allowedUrl_)) {
            ret = true;
        } else {
            ret = false;
        }
    }
    delete[] url_lower;
    return ret;
}
void BrowserWindow::SetTitle(const wchar_t* title) {
    BOOL b = SetWindowText(windowHandle_, title);
    _ASSERT(b);
}
bool BrowserWindow::IsPopup() {
    if (parentHandle_)
        return true;
    return false;
}
bool BrowserWindow::IsUsingMetaTitle() {
    if (IsPopup()) {
        json_value* settings = GetApplicationSettings();
        std::string fixed_title = (*settings)["popup_window"]["fixed_title"];
        return fixed_title.empty();
    }
    return false;
}
void BrowserWindow::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Need to re-attach click events after each browser navigation.
    TryAttachClickEvents();
    TrySetFocusAfterCreation();
}
void BrowserWindow::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!IsPopup()) {
        json_value* settings = GetApplicationSettings();
        static long minimum_width = 
                (*settings)["main_window"]["minimum_size"][0];
        static long minimum_height = 
                (*settings)["main_window"]["minimum_size"][1];
        static long maximum_width = 
                (*settings)["main_window"]["maximum_size"][0];
        static long maximum_height = 
                (*settings)["main_window"]["maximum_size"][1];
        MINMAXINFO* pMMI = (MINMAXINFO*)lParam;
        if (minimum_width)
            pMMI->ptMinTrackSize.x = minimum_width;
        if (minimum_height)
            pMMI->ptMinTrackSize.y = minimum_height;
        if (maximum_width)        
            pMMI->ptMaxTrackSize.x = maximum_width;
        if (maximum_height)
            pMMI->ptMaxTrackSize.y = maximum_height;
    }
}
void BrowserWindow::OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    DWORD clientWidth = LOWORD(lParam);
    DWORD clientHeight = HIWORD(lParam);
    if (webBrowser2_) {
        // WebBrowser2->put_Width() will dispatch an event
        // DWebBrowserEvents2::WindowSetWidth() which will call
        // BrowserWindow::SetWidth(), we should not do any resizing
        // during these events.
        isResizing_ = true;
        webBrowser2_->put_Width(clientWidth);
        webBrowser2_->put_Height(clientHeight);
        isResizing_ = false;
    } else {
        LOG_WARNING << "BrowserWindow::OnResize() failed: "
                       "WebBrowser2 object not created yet";
    }
}
void BrowserWindow::SetWidth(long newClientWidth) {
    if (isResizing_)
        return;
    RECT clientRect;
    HWND shellBrowserHandle = GetShellBrowserHandle();
    if (!shellBrowserHandle)
        return;
    BOOL b = GetClientRect(shellBrowserHandle, &clientRect);
    _ASSERT(b);
    _ASSERT(clientRect.left == 0 && clientRect.top == 0);
    clientRect.right = newClientWidth;
    RECT windowRect = clientRect;
    b = AdjustWindowRectEx(&windowRect, 
            GetWindowLong(windowHandle_, GWL_STYLE),
            GetMenu(windowHandle_) != NULL, 
            GetWindowLong(windowHandle_, GWL_EXSTYLE));
    _ASSERT(b);
    b = SetWindowPos(windowHandle_, HWND_TOP, 0, 0,
            windowRect.right - windowRect.left, 
            windowRect.bottom - windowRect.top,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
    _ASSERT(b);
}
void BrowserWindow::SetHeight(long newClientHeight) {
    if (isResizing_)
        return;
    RECT clientRect;
    HWND shellBrowserHandle = GetShellBrowserHandle();
    if (!shellBrowserHandle)
        return;
    BOOL b = GetClientRect(shellBrowserHandle, &clientRect);
    _ASSERT(b);
    _ASSERT(clientRect.left == 0 && clientRect.top == 0);
    clientRect.bottom = newClientHeight;
    RECT windowRect = clientRect;
    b = AdjustWindowRectEx(&windowRect, 
            GetWindowLong(windowHandle_, GWL_STYLE),
            GetMenu(windowHandle_) != NULL, 
            GetWindowLong(windowHandle_, GWL_EXSTYLE));
    _ASSERT(b);
    b = SetWindowPos(windowHandle_, HWND_TOP, 0, 0,
            windowRect.right - windowRect.left, 
            windowRect.bottom - windowRect.top,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
    _ASSERT(b);
}
void BrowserWindow::SetTop(long newTop) {
    RECT rect;
    BOOL b = GetWindowRect(windowHandle_, &rect);
    _ASSERT(b);
    b = SetWindowPos(windowHandle_, HWND_TOP, newTop, rect.left, 0, 0,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    _ASSERT(b);
}
void BrowserWindow::SetLeft(long newLeft) {
    RECT rect;
    BOOL b = GetWindowRect(windowHandle_, &rect);
    _ASSERT(b);
    b = SetWindowPos(windowHandle_, HWND_TOP, rect.top, newLeft, 0, 0,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    _ASSERT(b);
}
void BrowserWindow::SetTitleFromSettings() {
    if (IsPopup()) {
        json_value* settings = GetApplicationSettings();
        std::wstring popup_title = (*settings)["popup_window"]["fixed_title"];
        if (popup_title.empty())
            popup_title = (*settings)["main_window"]["title"];
        if (popup_title.empty())
            popup_title = Utf8ToWide(GetExecutableName());
        SetTitle(popup_title.c_str());
    }
    // Main window title is set in CreateMainWindow().
}
void BrowserWindow::SetIconFromSettings() {
    json_value* settings = GetApplicationSettings();
    const char* iconPath;
    if (IsPopup())
        iconPath = (*settings)["popup_window"]["icon"];
    else 
        iconPath = (*settings)["main_window"]["icon"];
    if (iconPath && iconPath[0] != 0) {
        wchar_t iconPathW[MAX_PATH];
        Utf8ToWide(iconPath, iconPathW, _countof(iconPathW));

        int bigX = GetSystemMetrics(SM_CXICON);
        int bigY = GetSystemMetrics(SM_CYICON);
        HANDLE bigIcon = LoadImage(0, iconPathW, IMAGE_ICON, bigX, bigY, 
                                   LR_LOADFROMFILE);
        if (bigIcon) {
            SendMessage(windowHandle_, WM_SETICON, ICON_BIG, (LPARAM)bigIcon);
        } else {
            LOG_WARNING << "Setting icon from settings file failed "
                           "(ICON_BIG)";
        }
        int smallX = GetSystemMetrics(SM_CXSMICON);
        int smallY = GetSystemMetrics(SM_CYSMICON);
        HANDLE smallIcon = LoadImage(0, iconPathW, IMAGE_ICON, smallX, 
                                     smallY, LR_LOADFROMFILE);
        if (smallIcon) {
            SendMessage(windowHandle_, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);
        } else {
            LOG_WARNING << "Setting icon from settings file failed "
                           "(ICON_SMALL)";
        }
    }
}
HWND BrowserWindow::GetShellBrowserHandle() {
    // Calling WebBrowser2->get_HWND() fails, need to go around.
    HRESULT hr;
    if (!webBrowser2_) {
        LOG_DEBUG << "BrowserWindow::GetShellBrowserHandle() failed: "
                     "WebBrowser2 is empty";
        return 0;
    }
    IServiceProviderPtr serviceProvider;
    hr = webBrowser2_->QueryInterface(IID_IServiceProvider,
            (void**)&serviceProvider);
    if (FAILED(hr) || !serviceProvider) {
        LOG_WARNING << "BrowserWindow::GetShellBrowserHandle() failed: "
                       "WebBrowser2->QueryInterface(IServiceProvider) failed";
        return 0;
    }
    IOleWindowPtr oleWindow;
    hr = serviceProvider->QueryService(SID_SShellBrowser, IID_IOleWindow,
            (void**)&oleWindow);
    if (FAILED(hr) || !oleWindow) {
        LOG_WARNING << "BrowserWindow::GetShellBrowserHandle() failed: "
                       "QueryService(SShellBrowser, IOleWindow) failed";
        return 0;
    }
    HWND shellBrowserHandle;
    hr = oleWindow->GetWindow(&shellBrowserHandle);
    if (FAILED(hr) || !shellBrowserHandle) {
        LOG_WARNING << "BrowserWindow::GetShellBrowserHandle() failed: "
                       "OleWindow->GetWindow() failed";
        return 0;
    }
    return shellBrowserHandle;
}
bool BrowserWindow::SetFocus() {
    // Calling SetFocus() on shellBrowser handle does not work.
    if (!oleInPlaceActiveObject_)
        return false;
    HRESULT hr = oleInPlaceActiveObject_->OnFrameWindowActivate(TRUE);
    if (FAILED(hr)) {
        LOG_DEBUG << "BrowserWindow::SetFocus(): "
                "IOleInPlaceActiveObject->OnFrameWindowActivate() failed";
        return false;
    }
    hr = oleInPlaceActiveObject_->OnDocWindowActivate(TRUE);
    if (FAILED(hr)) {
        LOG_DEBUG << "BrowserWindow::SetFocus(): "
                "IOleInPlaceActiveObject->OnDocWindowActivate() failed";
        return false;
    }
    return true;
    // Another way is to call:
    // IWebBrowser2->get_Document()
    // IDispatch->QueryInterface(IHTMLDocument2)
    // IHTMLDocument2->get_body()
    // IHTMLElement->QueryInterface(IHTMLElement2)
    // IHTMLElement2->focus()
}
bool BrowserWindow::TrySetFocusAfterCreation() {
    if (!focusedAfterCreation_) {
        if (SetFocus()) {
            focusedAfterCreation_ = true;
            return true;
        }
    }
    return false;
}
bool BrowserWindow::TranslateAccelerator(MSG* msg) {
    if (!oleInPlaceActiveObject_)
        return false;
    HRESULT hr = oleInPlaceActiveObject_->TranslateAccelerator(msg);
    if (FAILED(hr)) {
        LOG_DEBUG << "BrowserWindow::TranslateAccelerator() failed: "
                "IOleInPlaceActiveObject->TranslateAccelerator() failed";
        return false;
    }
    // S_OK - translated
    // S_FALSE - not translated
    // other values = FAILED()
    // Remember that (hr=S_FALSE) == SUCCEEDED(hr)
    return (hr == S_OK);
}
bool BrowserWindow::DisplayHtmlString(const wchar_t* htmlString) {
    if (!webBrowser2_)
        return false;
    IDispatchPtr documentDispatch;
    HRESULT hr = webBrowser2_->get_Document(&documentDispatch);
    if (FAILED(hr) || !documentDispatch) {
        LOG_DEBUG << "BrowserWindow::DisplayHtmlString(): "
                "IWebBrowser2->get_Document() failed";
        // If there is no document available navigate to blank page.
        bool navigated = Navigate(L"about:blank");
        LOG_DEBUG << "Navigated to about:blank";
        if (!navigated)
            return false;
        hr = webBrowser2_->get_Document(&documentDispatch);
        if (FAILED(hr) || !documentDispatch) {
            LOG_ERROR << "BrowserWindow::DisplayHtmlString() failed: "
                "IWebBrowser2->get_Document(about:blank) failed";
            _ASSERT(false);
            return false;
        }
    }
    IHTMLDocument2Ptr htmlDocument2;
    hr = documentDispatch->QueryInterface(&htmlDocument2);
    if (FAILED(hr) || !htmlDocument2) {
        LOG_ERROR << "BrowserWindow::DisplayHtmlString() failed: "
                "QueryInterface(IHTMLDocument2) failed";
        _ASSERT(false);
        return false;
    }
    SAFEARRAY *strings = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    if (!strings) {
        LOG_ERROR << "BrowserWindow::DisplayHtmlString() failed: "
                "SafeArrayCreateVector() failed";
        _ASSERT(false);
        return false;
    }
    // NOTICE: From now on returning false is allowed only at the end 
    // of the function, as we need to cleanup memory.
    VARIANT* variantParam;
    _bstr_t bstrParam = htmlString;
    hr = SafeArrayAccessData(strings, (void**)&variantParam);
    bool ret = false;
    if (SUCCEEDED(hr)) {
        variantParam->vt = VT_BSTR;
        variantParam->bstrVal = bstrParam.GetBSTR();
        hr = SafeArrayUnaccessData(strings);
        if (SUCCEEDED(hr)) {
            hr = htmlDocument2->write(strings);
            if (SUCCEEDED(hr)) {
                hr = htmlDocument2->close();
                if (SUCCEEDED(hr)) {
                    ret = true;
                } else {
                    LOG_ERROR << "BrowserWindow::DisplayHtmlString() failed: "
                            "IHTMLDocument2->close() failed";
                    _ASSERT(false);
                }
            } else {
                LOG_ERROR << "BrowserWindow::DisplayHtmlString() failed: "
                        "IHTMLDocument2->write() failed";
                _ASSERT(false);
            }
        } else {
            LOG_ERROR << "BrowserWindow::DisplayHtmlString() failed: "
                    "SafeArrayUnaccessData() failed";
            _ASSERT(false);
        }
    } else {
        LOG_ERROR << "BrowserWindow::DisplayHtmlString() failed: "
                "SafeArrayAccessData() failed";
        _ASSERT(false);
    }
    if (strings) {
        hr = SafeArrayDestroy(strings);
        strings = 0;
        _ASSERT(SUCCEEDED(hr));
    }
    return ret;
}
bool BrowserWindow::DisplayErrorPage(const wchar_t* navigateUrl, int statusCode) {
    json_value* settings = GetApplicationSettings();
    std::string error_page = (*settings)["msie"]["error_page"];
    if (error_page.empty())
        return false;
    std::string htmlFile = GetExecutableDirectory() + "\\" + error_page;
    htmlFile = GetRealPath(htmlFile);
    std::string htmlString = GetFileContents(htmlFile);
    if (htmlString.empty()) {
        LOG_WARNING << "BrowserWindow::DisplayErrorPage() failed: "
                "file not found: " << htmlFile;
        return false;
    }
    ReplaceStringInPlace(htmlString, "{{navigate_url}}", 
            WideToUtf8(navigateUrl));
    ReplaceStringInPlace(htmlString, "{{status_code}}", 
            IntToString(statusCode));
    if (DisplayHtmlString(Utf8ToWide(htmlString).c_str()))
        return true;
    return false;
}
