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
            if (riid == IID_IOleAdviseHolder) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleAdviseHolder";
            } else if (riid == IID_IOleCache) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleCache";
            } else if (riid == IID_IOleCache2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleCache2";
            } else if (riid == IID_IOleCacheControl) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleCacheControl";
            } else if (riid == IID_IParseDisplayName) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IParseDisplayName";
            } else if (riid == IID_IOleContainer) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleContainer";
            } else if (riid == IID_IOleClientSite) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleClientSite";
            } else if (riid == IID_IOleObject) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleObject";
            } else if (riid == IID_IOleWindow) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleWindow";
            } else if (riid == IID_IOleLink) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleLink";
            } else if (riid == IID_IOleItemContainer) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleItemContainer";
            } else if (riid == IID_IOleInPlaceUIWindow) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleInPlaceUIWindow";
            } else if (riid == IID_IOleInPlaceActiveObject) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleInPlaceActiveObject";
            } else if (riid == IID_IOleInPlaceFrame) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleInPlaceFrame";
            } else if (riid == IID_IOleInPlaceObject) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleInPlaceObject";
            } else if (riid == IID_IOleInPlaceSite) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOleInPlaceSite";
            } else if (riid == IID_IContinue) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IContinue";
            } else if (riid == IID_IViewObject) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IViewObject";
            } else if (riid == IID_IViewObject2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IViewObject2";
            } else if (riid == IID_IDropSource) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IDropSource";
            } else if (riid == IID_IDropTarget) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IDropTarget";
            } else if (riid == IID_IDropSourceNotify) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IDropSourceNotify";
            } else if (riid == IID_IEnumOLEVERB) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IEnumOLEVERB";
            } else if (riid == IID_IHostDialogHelper) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHostDialogHelper";
            } else if (riid == IID_IDocHostUIHandler) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IDocHostUIHandler";
            } else if (riid == IID_IDocHostUIHandler2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IDocHostUIHandler2";
            } else if (riid == IID_ICustomDoc) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_ICustomDoc";
            } else if (riid == IID_IDocHostShowUI) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IDocHostShowUI";
            } else if (riid == IID_IClassFactoryEx) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IClassFactoryEx";
            } else if (riid == IID_IHTMLOMWindowServices) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLOMWindowServices";
            } else if (riid == IID_IHTMLWindow2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLWindow2";
            } else if (riid == IID_IElementBehaviorFactory) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IElementBehaviorFactory";
            } else if (riid == IID_IHTMLFiltersCollection) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLFiltersCollection";
            } else if (riid == IID_IHTMLEventObj) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLEventObj";
            } else if (riid == IID_IElementBehaviorSite) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IElementBehaviorSite";
            } else if (riid == IID_IElementBehavior) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IElementBehavior";
            } else if (riid == IID_IElementBehaviorSiteOM) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IElementBehaviorSiteOM";
            } else if (riid == IID_IElementBehaviorRender) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IElementBehaviorRender";
            } else if (riid == IID_IElementBehaviorSiteRender) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IElementBehaviorSiteRender";
            } else if (riid == IID_IHTMLStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLStyle";
            } else if (riid == IID_IHTMLStyle2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLStyle2";
            } else if (riid == IID_IHTMLStyle3) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLStyle3";
            } else if (riid == IID_IHTMLStyle4) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLStyle4";
            } else if (riid == IID_IHTMLStyle5) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLStyle5";
            } else if (riid == IID_IHTMLRuleStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRuleStyle";
            } else if (riid == IID_IHTMLRuleStyle2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRuleStyle2";
            } else if (riid == IID_IHTMLRuleStyle3) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRuleStyle3";
            } else if (riid == IID_IHTMLRuleStyle4) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRuleStyle4";
            } else if (riid == IID_IHTMLRuleStyle5) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRuleStyle5";
            } else if (riid == DIID_DispHTMLStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=DIID_DispHTMLStyle";
            } else if (riid == DIID_DispHTMLRuleStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=DIID_DispHTMLRuleStyle";
            } else if (riid == IID_IHTMLRenderStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRenderStyle";
            } else if (riid == DIID_DispHTMLRenderStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=DIID_DispHTMLRenderStyle";
            } else if (riid == IID_IHTMLCurrentStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLCurrentStyle";
            } else if (riid == IID_IHTMLCurrentStyle2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLCurrentStyle2";
            } else if (riid == IID_IHTMLCurrentStyle3) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLCurrentStyle3";
            } else if (riid == IID_IHTMLCurrentStyle4) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLCurrentStyle4";
            } else if (riid == DIID_DispHTMLCurrentStyle) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=DIID_DispHTMLCurrentStyle";
            } else if (riid == IID_IHTMLElement) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLElement";
            } else if (riid == IID_IHTMLRect) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRect";
            } else if (riid == IID_IHTMLRectCollection) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLRectCollection";
            } else if (riid == IID_IHTMLDocument) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDocument";
            } else if (riid == IID_IHTMLUnknownElement) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLUnknownElement";
            } else if (riid == IID_IOmHistory) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IOmHistory";
            } else if (riid == IID_IHTMLDocument2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDocument2";
            } else if (riid == IID_IHTMLWindow2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLWindow2";
            } else if (riid == IID_IHTMLWindow3) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLWindow3";
            } else if (riid == IID_IHTMLScreen) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLScreen";
            } else if (riid == IID_IHTMLScreen2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLScreen2";
            } else if (riid == IID_IHTMLWindow4) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLWindow4";
            } else if (riid == IID_IHTMLWindow5) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLWindow5";
            } else if (riid == IID_IHTMLDocument3) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDocument3";
            } else if (riid == IID_IHTMLDocument4) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDocument4";
            } else if (riid == IID_IHTMLDocument5) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDocument5";
            } else if (riid == IID_IIMEServices) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IIMEServices";
            } else if (riid == IID_IWPCBlockedUrls) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IWPCBlockedUrls";
            } else if (riid == IID_IHTMLDialog) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDialog";
            } else if (riid == IID_IHTMLDialog2) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDialog2";
            } else if (riid == IID_IHTMLDialog3) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLDialog3";
            } else if (riid == IID_IHTMLPopup) {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): riid=IID_IHTMLPopup";
            } else {
                LOG(logDEBUG) << "OleClientSite::QueryInterface(): unknown interface, "
                              << "riid = " << &riid;
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
