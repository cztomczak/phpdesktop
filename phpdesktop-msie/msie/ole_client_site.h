// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <OleIdl.h>

#include "ole_inplace_site.h"
#include "ole_inplace_frame.h"
#include "service_provider.h"

template <class RootFrame>
class OleClientSite : public IOleClientSite
{
public:
    BrowserFrameInterface<RootFrame>* webFrame;
    ServiceProvider<RootFrame> serviceProvider;
    IDispatch* dispatch;
    // OleInPlaceFrame<RootFrame> oleInPlaceFrame;
    // OleInPlaceSite<RootFrame> oleInPlaceSite;
    // OleControlSite<RootFrame> oleControlSite;

    OleClientSite(BrowserFrameInterface<RootFrame>* inWebFrame)
        :
        webFrame(inWebFrame),
        serviceProvider(webFrame),
        dispatch(0)
        // oleInPlaceFrame(OleInPlaceFrame<RootFrame>((IOleClientSite*)this, webFrame)),
        // oleInPlaceSite(OleInPlaceSite<RootFrame>((IOleClientSite*)this, (IOleInPlaceFrame*)&oleInPlaceFrame, webFrame)),
        // oleControlSite()        
    {}

    // IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
    {
        // When using DocHostUIHandlerDispatch() you do not need to
        // implement: IOleInPlaceFrame, IOleInPlaceSite, IOleControlSite.

        if (riid == IID_IUnknown || riid == IID_IOleClientSite) {
            *ppvObject = (IOleClientSite*)this;
        } else if (riid == IID_IServiceProvider) {
            *ppvObject = &serviceProvider;
        } else if (riid == IID_IDispatch && dispatch) {
            *ppvObject = dispatch;
        } else if (riid == IID_IOleInPlaceSite) {
            // Off: *ppvObject = &oleInPlaceSite;
            *ppvObject = 0;
            return E_NOINTERFACE;
        } else if (riid == IID_IOleInPlaceFrame) {
            // Off: *ppvObject = &oleInPlaceFrame;
            *ppvObject = 0;
            return E_NOINTERFACE;
        } else if (riid == IID_IOleControlSite) {
            // Off: *ppvObject = &oleControlSite;
            *ppvObject = 0;
            return E_NOINTERFACE;
        } else if (riid == IID_IAxWinHostWindow) {
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

    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return 1;
    }

    ULONG STDMETHODCALLTYPE Release(void)
    {
        return 1;
    }

    // IOleClientSite

    HRESULT STDMETHODCALLTYPE SaveObject( void)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetMoniker(
            /* [in] */ DWORD dwAssign,
            /* [in] */ DWORD dwWhichMoniker,
            /* [out] */ __RPC__deref_out_opt IMoniker **ppmk)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetContainer(
            /* [out] */ __RPC__deref_out_opt IOleContainer **ppContainer)
    {
        *ppContainer = NULL;
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE ShowObject( void)
    {
        // LOG(logDEBUG) << "OleClientSite::ShowObject()";
        return NOERROR;
    }

    HRESULT STDMETHODCALLTYPE OnShowWindow(
            /* [in] */ BOOL fShow)
    {
        // LOG(logDEBUG) << "OleClientSite::OnShowWindow()";
        return E_NOTIMPL;
    }

        HRESULT STDMETHODCALLTYPE RequestNewObjectLayout( void)
    {
        return E_NOTIMPL;
    }

};
