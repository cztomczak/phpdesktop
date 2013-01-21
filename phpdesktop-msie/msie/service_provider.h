// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <ServProv.h>

#include "internet_security_manager.h"
#include "html_om_window_services.h"
#include "browser_frame_interface.h"

template <class RootFrame>
class ServiceProvider : public IServiceProvider
{
public:
    InternetSecurityManager internetSecurityManager_;
    HTMLOMWindowServices<RootFrame> htmlOMWindowServices_;

    ServiceProvider(BrowserFrameInterface<RootFrame>* inWebFrame)
        :
        htmlOMWindowServices_(HTMLOMWindowServices<RootFrame>(inWebFrame))
    {
    }

    // IServiceProvider

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
    {
        return E_NOTIMPL;
    }
    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void)
    {
        return 1;
    }
    HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, REFIID riid, void **ppvObject)
    {
        if (riid == IID_IInternetSecurityManager) {
            *ppvObject = &internetSecurityManager_;
        } else if(riid == IID_IHTMLOMWindowServices) {
            *ppvObject = &htmlOMWindowServices_;
        } else {
            *ppvObject = 0;
            return E_NOINTERFACE;
        }
        return S_OK;
    }
};
