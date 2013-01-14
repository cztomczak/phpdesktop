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
    OleInPlaceFrame<RootFrame> oleInPlaceFrame;
    OleInPlaceSite<RootFrame> oleInPlaceSite;
    IDispatch* dispatch;

public:
    OleClientSite(BrowserFrameInterface<RootFrame>* inWebFrame)
        :
        webFrame(inWebFrame),
        serviceProvider(webFrame),
        oleInPlaceFrame(
            OleInPlaceFrame<RootFrame>((IOleClientSite*)this, webFrame)),
        oleInPlaceSite(
            OleInPlaceSite<RootFrame>((IOleClientSite*)this, (IOleInPlaceFrame*)&oleInPlaceFrame, webFrame))
    {
        dispatch = 0;
    }

public:
    // IUnknown

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
    {
        if (riid == IID_IUnknown || riid == IID_IOleClientSite) {
            *ppvObject = (IOleClientSite*)this;
        } else if (riid == IID_IServiceProvider) {
            *ppvObject = &serviceProvider;
        } else if (riid == IID_IDispatch && dispatch) {
            *ppvObject = dispatch;
        //} else if (riid == IID_IOleInPlaceSite) {
            //*ppvObject = &oleInPlaceSite;
            // ("asking for IID_IOleInPlaceSite");
        //} else if (riid == IID_IOleInPlaceFrame) {
            //*ppvObject = &oleInPlaceFrame;
            // ("asking for IID_IOleInPlaceFrame");
        //} else if (riid == IID_IDocHostUIHandler) {
            // Needs to implement ICustomDoc first, then it asks for IID_IDocHostUIHandler.
            //*ppvObject = 0;
            // ("asking for IID_IDocHostUIHandler");
        } else {
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

public:
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
        return NOERROR;
    }

    HRESULT STDMETHODCALLTYPE OnShowWindow(
            /* [in] */ BOOL fShow)
    {
        return E_NOTIMPL;
    }

        HRESULT STDMETHODCALLTYPE RequestNewObjectLayout( void)
    {
        return E_NOTIMPL;
    }

};
