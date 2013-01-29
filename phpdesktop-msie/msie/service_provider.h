// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <ServProv.h>
#include <ShlGuid.h>
#include <Urlmon.h>

#include "internet_security_manager.h"
#include "html_om_window_services.h"
#include "browser_frame_interface.h"

template <class TopFrame>
class ServiceProvider : public IServiceProvider
{
public:
    InternetSecurityManager internetSecurityManager_;
    HtmlOmWindowServices<TopFrame> htmlOmWindowServices_;
    BrowserFrameInterface<TopFrame>* browserFrame_;

    ServiceProvider(BrowserFrameInterface<TopFrame>* inBrowserFrame)
            : browserFrame_(inBrowserFrame),
            htmlOmWindowServices_(HtmlOmWindowServices<TopFrame>(
                                  inBrowserFrame)) {
    }
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) {
        return browserFrame_->GetOleClientSite()->QueryInterface(riid, 
                                                                 ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }
    // IServiceProvider
    HRESULT STDMETHODCALLTYPE QueryService(REFGUID siid, REFIID riid, 
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
                LOG(logDEBUG) << "ServiceProvider::QueryService(): "
                                 "unknown service, riid = " << riid_name;
            }
            */
            *ppvObject = 0;
            return E_NOINTERFACE;
        }
        return S_OK;
    }
};
