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
            OleInPlaceSite<RootFrame>((IOleClientSite*)this, (IOleInPlaceFrame*)&oleInPlaceFrame, webFrame)),
        dispatch(0)
    {}

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
        } else if (riid == IID_IOleInPlaceSite) {
            *ppvObject = &oleInPlaceSite;
        } else if (riid == IID_IOleInPlaceFrame) {
            *ppvObject = &oleInPlaceFrame;
        } else {
            /*
            // OleIdl.h
            if (riid == IID_IOleAdviseHolder) {
                DEBUG_ASCII("IID_IOleAdviseHolder");
            } else if (riid == IID_IOleCache) {
                DEBUG_ASCII("IID_IOleCache");
            } else if (riid == IID_IOleCache2) {
                DEBUG_ASCII("IID_IOleCache2");
            } else if (riid == IID_IOleCacheControl) {
                DEBUG_ASCII("IID_IOleCacheControl");
            } else if (riid == IID_IParseDisplayName) {
                DEBUG_ASCII("IID_IParseDisplayName");
            } else if (riid == IID_IOleContainer) {
                DEBUG_ASCII("IID_IOleContainer");
            } else if (riid == IID_IOleClientSite) {
                DEBUG_ASCII("IID_IOleClientSite");
            } else if (riid == IID_IOleObject) {
                DEBUG_ASCII("IID_IOleObject");
            } else if (riid == IID_IOleWindow) {
                DEBUG_ASCII("IID_IOleWindow");
            } else if (riid == IID_IOleLink) {
                DEBUG_ASCII("IID_IOleLink");
            } else if (riid == IID_IOleItemContainer) {
                DEBUG_ASCII("IID_IOleItemContainer");
            } else if (riid == IID_IOleInPlaceUIWindow) {
                DEBUG_ASCII("IID_IOleInPlaceUIWindow");
            } else if (riid == IID_IOleInPlaceActiveObject) {
                DEBUG_ASCII("IID_IOleInPlaceActiveObject");
            } else if (riid == IID_IOleInPlaceFrame) {
                DEBUG_ASCII("IID_IOleInPlaceFrame");
            } else if (riid == IID_IOleInPlaceObject) {
                DEBUG_ASCII("IID_IOleInPlaceObject");
            } else if (riid == IID_IOleInPlaceSite) {
                DEBUG_ASCII("IID_IOleInPlaceSite");
            } else if (riid == IID_IContinue) {
                DEBUG_ASCII("IID_IContinue");
            } else if (riid == IID_IViewObject) {
                DEBUG_ASCII("IID_IViewObject");
            } else if (riid == IID_IViewObject2) {
                DEBUG_ASCII("IID_IViewObject2");
            } else if (riid == IID_IDropSource) {
                DEBUG_ASCII("IID_IDropSource");
            } else if (riid == IID_IDropTarget) {
                DEBUG_ASCII("IID_IDropTarget");
            } else if (riid == IID_IDropSourceNotify) {
                DEBUG_ASCII("IID_IDropSourceNotify");
            } else if (riid == IID_IEnumOLEVERB) {
                DEBUG_ASCII("IID_IEnumOLEVERB");
            }

            // MsHtmHst.h
            if (riid == IID_IHostDialogHelper) {
                DEBUG_ASCII("IID_IHostDialogHelper");
            } else if (riid == IID_IDocHostUIHandler) {
                DEBUG_ASCII("IID_IDocHostUIHandler");
            } else if (riid == IID_IDocHostUIHandler2) {
                DEBUG_ASCII("IID_IDocHostUIHandler2");
            } else if (riid == IID_ICustomDoc) {
                DEBUG_ASCII("IID_ICustomDoc");
            } else if (riid == IID_IDocHostShowUI) {
                DEBUG_ASCII("IID_IDocHostShowUI");
            } else if (riid == IID_IClassFactoryEx) {
                DEBUG_ASCII("IID_IClassFactoryEx");
            } else if (riid == IID_IHTMLOMWindowServices) {
                DEBUG_ASCII("IID_IHTMLOMWindowServices");
            }

            // MsHTML.h
            if (riid == IID_IHTMLWindow2) {
                DEBUG_ASCII("IID_IHTMLWindow2");
            } else if (riid == IID_IElementBehaviorFactory) {
                DEBUG_ASCII("IID_IElementBehaviorFactory");
            } else if (riid == IID_IHTMLFiltersCollection) {
                DEBUG_ASCII("IID_IHTMLFiltersCollection");
            } else if (riid == IID_IHTMLEventObj) {
                DEBUG_ASCII("IID_IHTMLEventObj");
            } else if (riid == IID_IElementBehaviorSite) {
                DEBUG_ASCII("IID_IElementBehaviorSite");
            } else if (riid == IID_IElementBehavior) {
                DEBUG_ASCII("IID_IElementBehavior");
            } else if (riid == IID_IElementBehaviorSiteOM) {
                DEBUG_ASCII("IID_IElementBehaviorSiteOM");
            } else if (riid == IID_IElementBehaviorRender) {
                DEBUG_ASCII("IID_IElementBehaviorRender");
            } else if (riid == IID_IElementBehaviorSiteRender) {
                DEBUG_ASCII("IID_IElementBehaviorSiteRender");
            } else if (riid == IID_IHTMLStyle) {
                DEBUG_ASCII("IID_IHTMLStyle");
            } else if (riid == IID_IHTMLStyle2) {
                DEBUG_ASCII("IID_IHTMLStyle2");
            } else if (riid == IID_IHTMLStyle3) {
                DEBUG_ASCII("IID_IHTMLStyle3");
            } else if (riid == IID_IHTMLStyle4) {
                DEBUG_ASCII("IID_IHTMLStyle4");
            } else if (riid == IID_IHTMLStyle5) {
                DEBUG_ASCII("IID_IHTMLStyle5");
            } else if (riid == IID_IHTMLRuleStyle) {
                DEBUG_ASCII("IID_IHTMLRuleStyle");
            } else if (riid == IID_IHTMLRuleStyle2) {
                DEBUG_ASCII("IID_IHTMLRuleStyle2");
            } else if (riid == IID_IHTMLRuleStyle3) {
                DEBUG_ASCII("IID_IHTMLRuleStyle3");
            } else if (riid == IID_IHTMLRuleStyle4) {
                DEBUG_ASCII("IID_IHTMLRuleStyle4");
            } else if (riid == IID_IHTMLRuleStyle5) {
                DEBUG_ASCII("IID_IHTMLRuleStyle5");
            } else if (riid == DIID_DispHTMLStyle) {
                DEBUG_ASCII("DIID_DispHTMLStyle");
            } else if (riid == DIID_DispHTMLRuleStyle) {
                DEBUG_ASCII("DIID_DispHTMLRuleStyle");
            } else if (riid == IID_IHTMLRenderStyle) {
                DEBUG_ASCII("IID_IHTMLRenderStyle");
            } else if (riid == DIID_DispHTMLRenderStyle) {
                DEBUG_ASCII("DIID_DispHTMLRenderStyle");
            } else if (riid == IID_IHTMLCurrentStyle) {
                DEBUG_ASCII("IID_IHTMLCurrentStyle");
            } else if (riid == IID_IHTMLCurrentStyle2) {
                DEBUG_ASCII("IID_IHTMLCurrentStyle2");
            } else if (riid == IID_IHTMLCurrentStyle3) {
                DEBUG_ASCII("IID_IHTMLCurrentStyle3");
            } else if (riid == IID_IHTMLCurrentStyle4) {
                DEBUG_ASCII("IID_IHTMLCurrentStyle4");
            } else if (riid == DIID_DispHTMLCurrentStyle) {
                DEBUG_ASCII("DIID_DispHTMLCurrentStyle");
            } else if (riid == IID_IHTMLElement) {
                DEBUG_ASCII("IID_IHTMLElement");
            } else if (riid == IID_IHTMLRect) {
                DEBUG_ASCII("IID_IHTMLRect");
            } else if (riid == IID_IHTMLRectCollection) {
                DEBUG_ASCII("IID_IHTMLRectCollection");
            } else if (riid == IID_IHTMLDocument) {
                DEBUG_ASCII("IID_IHTMLDocument");
            } else if (riid == IID_IHTMLUnknownElement) {
                DEBUG_ASCII("IID_IHTMLUnknownElement");
            } else if (riid == IID_IOmHistory) {
                DEBUG_ASCII("IID_IOmHistory");
            } else if (riid == IID_IHTMLDocument2) {
                DEBUG_ASCII("IID_IHTMLDocument2");
            } else if (riid == IID_IHTMLWindow2) {
                DEBUG_ASCII("IID_IHTMLWindow2");
            } else if (riid == IID_IHTMLWindow3) {
                DEBUG_ASCII("IID_IHTMLWindow3");
            } else if (riid == IID_IHTMLScreen) {
                DEBUG_ASCII("IID_IHTMLScreen");
            } else if (riid == IID_IHTMLScreen2) {
                DEBUG_ASCII("IID_IHTMLScreen2");
            } else if (riid == IID_IHTMLWindow4) {
                DEBUG_ASCII("IID_IHTMLWindow4");
            } else if (riid == IID_IHTMLWindow5) {
                DEBUG_ASCII("IID_IHTMLWindow5");
            } else if (riid == IID_IHTMLDocument3) {
                DEBUG_ASCII("IID_IHTMLDocument3");
            } else if (riid == IID_IHTMLDocument4) {
                DEBUG_ASCII("IID_IHTMLDocument4");
            } else if (riid == IID_IHTMLDocument5) {
                DEBUG_ASCII("IID_IHTMLDocument5");
            } else if (riid == IID_IIMEServices) {
                DEBUG_ASCII("IID_IIMEServices");
            } else if (riid == IID_IWPCBlockedUrls) {
                DEBUG_ASCII("IID_IWPCBlockedUrls");
            } else if (riid == IID_IHTMLDialog) {
                DEBUG_ASCII("IID_IHTMLDialog");
            } else if (riid == IID_IHTMLDialog2) {
                DEBUG_ASCII("IID_IHTMLDialog2");
            } else if (riid == IID_IHTMLDialog3) {
                DEBUG_ASCII("IID_IHTMLDialog3");
            } else if (riid == IID_IHTMLPopup) {
                DEBUG_ASCII("IID_IHTMLPopup");
            }
            */

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
