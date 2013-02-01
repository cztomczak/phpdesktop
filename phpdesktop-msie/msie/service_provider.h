// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <ServProv.h>
#include <ShlGuid.h>
#include <Urlmon.h>

class BrowserWindow;
#include "internet_security_manager.h"
#include "html_om_window_services.h"

class ServiceProvider : public IServiceProvider {
private:
    BrowserWindow* browserWindow_;
    InternetSecurityManager internetSecurityManager_;
    HtmlOmWindowServices htmlOmWindowServices_;
public:
    ServiceProvider(BrowserWindow* inBrowserWindow);
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    // IServiceProvider
    HRESULT STDMETHODCALLTYPE QueryService(REFGUID siid, REFIID riid, 
                                           void **ppvObject);
};
