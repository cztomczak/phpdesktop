// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "service_provider.h"
#include "browser_window.h"

ServiceProvider::ServiceProvider(BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow),
        htmlOmWindowServices_(inBrowserWindow),
        internetSecurityManager_(inBrowserWindow) {
}
// IUnknown
HRESULT STDMETHODCALLTYPE ServiceProvider::QueryInterface(
        REFIID riid,
        void **ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(riid, ppvObject);
}
ULONG STDMETHODCALLTYPE ServiceProvider::AddRef(void) {
    return 1;
}
ULONG STDMETHODCALLTYPE ServiceProvider::Release(void) {
    return 1;
}
// IServiceProvider
HRESULT STDMETHODCALLTYPE ServiceProvider::QueryService(
        REFGUID siid,
        REFIID riid,
        void **ppvObject) {
    if (siid == IID_IInternetSecurityManager &&
            riid == IID_IInternetSecurityManager) {
        *ppvObject = static_cast<IInternetSecurityManager*>(
                                 &internetSecurityManager_);
    } else if (siid == SID_SHTMLOMWindowServices &&
            riid == IID_IHTMLOMWindowServices) {
        *ppvObject = static_cast<IHTMLOMWindowServices*>(
                                 &htmlOmWindowServices_);
    } else {
        /*
        if (FILELog::ReportingLevel() >= logDEBUG) {
            char riid_name[128];
            GUID_TO_CHAR(&riid, riid_name, _countof(riid_name));
            LOG_DEBUG << "ServiceProvider::QueryService(): "
                         "unknown service, riid = " << riid_name;
        }
        */
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
    return S_OK;
}
