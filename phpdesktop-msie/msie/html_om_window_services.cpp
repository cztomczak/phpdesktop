// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "html_om_window_services.h"
#include "browser_window.h"

HtmlOmWindowServices::HtmlOmWindowServices(BrowserWindow* inBrowserWindow) 
        : browserWindow_(inBrowserWindow) {
}
// IUnknown
HRESULT STDMETHODCALLTYPE HtmlOmWindowServices::QueryInterface(
        REFIID riid, 
        void **ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(riid, 
                                                              ppvObject);
}
ULONG STDMETHODCALLTYPE HtmlOmWindowServices::AddRef(void) {
    return 1;
}
ULONG STDMETHODCALLTYPE HtmlOmWindowServices::Release(void) {
    return 1;
}
// IHTMLOMWindowServices
HRESULT STDMETHODCALLTYPE HtmlOmWindowServices::moveTo(LONG x, LONG y) {
    SetWindowPos(browserWindow_->GetWindowHandle(), NULL, (int)x, (int)y, 
                 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    return S_OK;
}
HRESULT STDMETHODCALLTYPE HtmlOmWindowServices::moveBy(LONG x, LONG y) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE HtmlOmWindowServices::resizeTo(LONG x, LONG y) {
    SetWindowPos(browserWindow_->GetWindowHandle(), NULL, 0, 0, 
                 (int)x, (int)y, SWP_NOMOVE | SWP_NOZORDER);
    return S_OK;
}
HRESULT STDMETHODCALLTYPE HtmlOmWindowServices::resizeBy(LONG x, LONG y) {
    return E_NOTIMPL;
}
