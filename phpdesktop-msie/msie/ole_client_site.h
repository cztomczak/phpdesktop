// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <OleIdl.h>

class BrowserWindow;
#include "ole_in_place_frame.h"
#include "ole_in_place_site.h"
#include "service_provider.h"
#include "browser_events2.h"
#include "doc_host_show_ui.h"
#include "doc_host_ui_handler.h"
#include "external_dispatch.h"
#include "host_dispatch.h"
#include "ole_command_target.h"

class OleClientSite : public IOleClientSite {
private:
    BrowserWindow* browserWindow_;
    OleInPlaceFrame oleInPlaceFrame_;
    OleInPlaceSite oleInPlaceSite_;
    ServiceProvider serviceProvider_;
    BrowserEvents2 browserEvents2_;
    DocHostShowUi docHostShowUi_;
    ExternalDispatch externalDispatch_;
    DocHostUiHandler docHostUiHandler_;
    HostDispatch hostDispatch_;
    OleCommandTarget oleCommandTarget_;
public:
    OleClientSite(BrowserWindow* inBrowserWindow);
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    HRESULT STDMETHODCALLTYPE SaveObject(void);
    HRESULT STDMETHODCALLTYPE GetMoniker(
            /* [in] */ DWORD dwAssign,
            /* [in] */ DWORD dwWhichMoniker,
            /* [out] */ IMoniker **ppmk);
    HRESULT STDMETHODCALLTYPE GetContainer(
            /* [out] */ IOleContainer **ppContainer);
    HRESULT STDMETHODCALLTYPE ShowObject(void);
    HRESULT STDMETHODCALLTYPE OnShowWindow(
            /* [in] */ BOOL fShow);
    HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void);
};


