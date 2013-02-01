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
#include "../window_utils.h"

std::map<HWND, BrowserWindow*> g_browserWindows;
extern std::string g_webServerUrl;

BrowserWindow* GetBrowserWindow(HWND hwnd) {
    std::map<HWND, BrowserWindow*>::iterator it;
    it = g_browserWindows.find(hwnd);
    if (it != g_browserWindows.end()) {
        return it->second;
    }
    // GetBrowserWindow() may fail during window creation, so log
    // severity is only DEBUG.
    LOG_DEBUG << "GetBrowserWindow() failed: hwnd = " << (int)hwnd;
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
        isResizing_(false) {

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

    // Creating a Web Browser Object:
    // http://msdn.microsoft.com/en-ca/library/aa451946.aspx
    // IUnknown retrieved through a call to CoCreateInstance, then 
    // IOleObject, do OLEIVERB_UIACTIVATE, retrieve IWebBrowser2 as last.

    // Create browser control.
    IUnknownPtr unknown;
    hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC,
                          IID_IUnknown, (void**)&unknown);
    if (FAILED(hr) || !unknown) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "CoCreateInstance(CLSID_WebBrowser) failed";
        return false;
    }

    IOleObjectPtr oleObject;
    hr = unknown->QueryInterface(IID_IOleObject, (void**)&oleObject);
    if (FAILED(hr) || !oleObject) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "QueryInterface(IOleObject) failed";
        return false;
    }
    
    hr = oleObject->SetClientSite(oleClientSite_.get());
    if (FAILED(hr)) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "SetClientSite() failed";
        return false;
    }

    RECT rect;
    b = GetClientRect(windowHandle_, &rect);
    if (!b) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "GetClientRect() failed";
        return false;
    }
    
    // OLEIVERB_INPLACEACTIVATE or OLEIVERB_UIACTIVATE.
    // When OLEIVERB_UIACTIVATE is set, IOleControlSite is queried 
    // just after IOleInPlaceSite.
    // When OLEIVERB_INPLACEACTIVATE is set IOleControlSite is queried 
    // only when window loses/gains focus.
    hr = oleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, 
                           static_cast<IOleClientSite*>(oleClientSite_.get()), 
                           0, windowHandle_, &rect);
    if (FAILED(hr)) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "DoVerb(OLEIVERB_INPLACEACTIVATE) failed";
        return false;
    }

    oleObject->QueryInterface(IID_IWebBrowser2, (void**)&webBrowser2_);
    if (FAILED(hr) || !webBrowser2_) {
        LOG_ERROR << "BrowserWindow::CreateBrowserControl() failed: "
                     "QueryInterface(IID_IWebBrowser2) failed";
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

    // AdviseEvent() takes care of logging errors.
    AdviseEvent(webBrowser2_, DIID_DWebBrowserEvents2,
                &dWebBrowserEvents2Cookie_);
    
    // Initial navigation.
    if (navigateUrl) {
        _variant_t varUrl = navigateUrl;
        _variant_t varFlags((long)0, VT_I4);
        _variant_t varTargetFrame(L"_self");
        _variant_t varPostData;
        _variant_t varHeaders;
        hr = webBrowser2_->Navigate2(&varUrl, &varFlags, &varTargetFrame, 
                                     &varPostData, &varHeaders);
        if (FAILED(hr)) {
            LOG_WARNING << "BrowserWindow::CreateBrowserControl() failed: "
                           "WebBrowser2->Navigate2() failed";
            return false;
        }
    }

    return true;
}
void BrowserWindow::CloseBrowserControl() {
    if (webBrowser2_) {
        UnadviseEvent(webBrowser2_, DIID_DWebBrowserEvents2,
                      dWebBrowserEvents2Cookie_);
        webBrowser2_->Stop();
        webBrowser2_->put_Visible(VARIANT_FALSE);
        
        IOleObjectPtr oleObject;
        HRESULT hr = webBrowser2_->QueryInterface(IID_IOleObject, 
                                                  (void**)&oleObject);
        if (FAILED(hr) || !oleObject) {
            LOG_DEBUG << "BrowserWindow::CloseBrowserControl(): "
                         "QueryInterface(IOleObject) failed";
        }

        // Remember to check if webBrowser2_ is not empty before using it,
        // in other functions like TryAttachClickEvents().
        webBrowser2_->Quit();
        webBrowser2_.Release();

        // It is important to set client site to NULL, otherwise
        // you will get first-chance exceptions when calling Close().
        oleObject->SetClientSite(0);
        oleObject->Close(OLECLOSE_NOSAVE);
        oleObject.Release();
        
        // Need to release all member variables otherwise you get exception:
        // "First-chance exception at 0x76034974 in php-desktop-msie.exe:
        //  0xC0000005: Access violation reading location 0xfeeefef6".
        // Calling oleObject->SetClientSite(0) fixed these problems, so
        // commenting off:
        // documentUniqueID_.~_bstr_t();
        // clickDispatch_.~_variant_t();
        // clickEvents_.reset();
        // externalDispatch_.reset();
        // oleClientSite_.reset();
    }
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
bool BrowserWindow::IsUrlAllowed(wchar_t* inUrl, int sizeInWords) {
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
