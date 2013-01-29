// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <OleIdl.h>

#include "browser_frame_interface.h"
#include "ole_in_place_frame.h"
#include "ole_in_place_site.h"
#include "ole_control_site.h"
#include "service_provider.h"
#include "browser_events2.h"
#include "doc_host_show_ui.h"
#include "doc_host_ui_handler.h"
#include "external_dispatch.h"

template <class TopFrame>
class OleClientSite : public IOleClientSite {
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;
    TopFrame* topFrame_;
    OleInPlaceFrame<TopFrame> oleInPlaceFrame_;
    OleInPlaceSite<TopFrame> oleInPlaceSite_;
    OleControlSite<TopFrame> oleControlSite_;
    ServiceProvider<TopFrame> serviceProvider_;
    BrowserEvents2<TopFrame> browserEvents2_;
    DocHostShowUi<TopFrame> docHostShowUi_;
    ExternalDispatch<TopFrame> externalDispatch_;
    DocHostUiHandler<TopFrame> docHostUiHandler_;
    
    OleClientSite(TopFrame* inTopFrame, 
                  BrowserFrameInterface<TopFrame>* inBrowserFrame)
            : browserFrame_(inBrowserFrame),
            topFrame_(inTopFrame),
            oleInPlaceFrame_(inBrowserFrame),
            oleInPlaceSite_(inBrowserFrame, &oleInPlaceFrame_),
            oleControlSite_(inBrowserFrame),
            serviceProvider_(inBrowserFrame),
            browserEvents2_(inBrowserFrame),
            docHostShowUi_(inBrowserFrame),
            externalDispatch_(inTopFrame),
            docHostUiHandler_(inBrowserFrame, &externalDispatch_) {
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) {
        if (ppvObject == 0)
            return E_POINTER;
        if (riid == IID_IUnknown) {
            *ppvObject = static_cast<IUnknown*>(this);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IUnknown";
                logged = true;
            }
        } else if (riid == IID_IOleClientSite) {
            *ppvObject = static_cast<IOleClientSite*>(this);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IOleClientSite";
                logged = true;
            }
        } else if (riid == IID_IOleInPlaceSite) {
            *ppvObject = static_cast<IOleInPlaceSite*>(&oleInPlaceSite_);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IOleInPlaceSite";
                logged = true;
            }
        } else if (riid == IID_IOleInPlaceFrame) {
            // This interface is retrieved in 
            // IOleInPlaceSite::GetWindowContext().
            *ppvObject = static_cast<IOleInPlaceFrame*>(&oleInPlaceFrame_);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IOleInPlaceFrame";
                logged = true;
            }
        } else if (riid == IID_IOleControlSite) {
            *ppvObject = static_cast<IOleControlSite*>(&oleControlSite_);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IOleControlSite";
                logged = true;
            }
        } else if (riid == IID_IServiceProvider) {
            *ppvObject = static_cast<IServiceProvider*>(&serviceProvider_);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IServiceProvider";
                logged = true;
            }
        } else if (riid == DIID_DWebBrowserEvents2) {
            *ppvObject = static_cast<DWebBrowserEvents2*>(&browserEvents2_);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): DWebBrowserEvents2";
                logged = true;
            }
        } else if (riid == IID_IDocHostShowUI) {
            *ppvObject = static_cast<IDocHostShowUI*>(&docHostShowUi_);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IDocHostShowUI";
                logged = true;
            }
        } else if (riid == IID_IDocHostUIHandler) {
            *ppvObject = static_cast<IDocHostUIHandler*>(&docHostUiHandler_);
            static bool logged = false;
            if (!logged) {
                LOG(logDEBUG) << "QueryInterface(): IDocHostUIHandler";
                logged = true;
            }
        } else if (riid == IID_IAxWinHostWindow) {
            *ppvObject = 0;
            return E_NOINTERFACE;
        } else if (   riid == IID_IDocHostUIHandler2
                   || riid == IID_IOleInPlaceSiteEx
                   || riid == IID_IOleCommandTarget
                   || riid == IID_IDispatch) {
            *ppvObject = 0;
            return E_NOINTERFACE;
        } else {
            if (FILELog::ReportingLevel() >= logDEBUG) {
                char riid_name[128];
                GUID_TO_CHAR(&riid, riid_name, _countof(riid_name));
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): "
                                 "unknown interface, riid = " << riid_name;
            }
            *ppvObject = 0;
            return E_NOINTERFACE;
        }
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }
    HRESULT STDMETHODCALLTYPE SaveObject(void) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetMoniker(
            /* [in] */ DWORD dwAssign,
            /* [in] */ DWORD dwWhichMoniker,
            /* [out] */ IMoniker **ppmk) {
        *ppmk = 0;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetContainer(
            /* [out] */ IOleContainer **ppContainer) {
        *ppContainer = 0;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE ShowObject(void) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnShowWindow(
            /* [in] */ BOOL fShow) {
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void) {
        return E_NOTIMPL;
    }
};
