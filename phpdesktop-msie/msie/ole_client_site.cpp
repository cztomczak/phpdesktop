// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "ole_client_site.h"
#include "browser_window.h"

// Must be included before defining GUID, otherwise you get 
// "unresolved external symbol _IID_IAxWinHostWindow".
#include "INITGUID.H" 
DEFINE_GUID(IID_IAxWinHostWindow, 0xb6ea2050, 0x048a, 0x11d1, 
            0x82, 0xb9, 0x00, 0xc0, 0x4f, 0xb9, 0x94, 0x2e);

#include "../debug.h"
#include "../log.h"

OleClientSite::OleClientSite(BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow),
        oleInPlaceFrame_(inBrowserWindow),
        oleInPlaceSite_(inBrowserWindow, &oleInPlaceFrame_),
        serviceProvider_(inBrowserWindow),
        browserEvents2_(inBrowserWindow),
        docHostShowUi_(inBrowserWindow),
        externalDispatch_(inBrowserWindow),
        docHostUiHandler_(inBrowserWindow, &externalDispatch_),
        hostDispatch_(inBrowserWindow) {
}
HRESULT STDMETHODCALLTYPE OleClientSite::QueryInterface(
        REFIID riid, void** ppvObject) {
    if (ppvObject == 0)
        return E_POINTER;
    if (riid == IID_IUnknown) {
        *ppvObject = static_cast<IUnknown*>(this);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IUnknown";
            logged = true;
        }
    } else if (riid == IID_IOleClientSite) {
        *ppvObject = static_cast<IOleClientSite*>(this);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IOleClientSite";
            logged = true;
        }
    } else if (riid == IID_IOleInPlaceSite) {
        *ppvObject = static_cast<IOleInPlaceSite*>(&oleInPlaceSite_);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IOleInPlaceSite";
            logged = true;
        }
    } else if (riid == IID_IOleInPlaceFrame) {
        // This interface is retrieved in 
        // IOleInPlaceSite::GetWindowContext().
        *ppvObject = static_cast<IOleInPlaceFrame*>(&oleInPlaceFrame_);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IOleInPlaceFrame";
            logged = true;
        }
    } else if (riid == IID_IServiceProvider) {
        *ppvObject = static_cast<IServiceProvider*>(&serviceProvider_);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IServiceProvider";
            logged = true;
        }
    } else if (riid == DIID_DWebBrowserEvents2) {
        *ppvObject = static_cast<DWebBrowserEvents2*>(&browserEvents2_);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): DWebBrowserEvents2";
            logged = true;
        }
    } else if (riid == IID_IDocHostShowUI) {
        *ppvObject = static_cast<IDocHostShowUI*>(&docHostShowUi_);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IDocHostShowUI";
            logged = true;
        }
    } else if (riid == IID_IDocHostUIHandler) {
        *ppvObject = static_cast<IDocHostUIHandler*>(&docHostUiHandler_);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IDocHostUIHandler";
            logged = true;
        }
    } else if (riid == IID_IDispatch) {
        *ppvObject = static_cast<IDispatch*>(&hostDispatch_);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "QueryInterface(): IDispatch";
            logged = true;
        }
    } else if (   riid == IID_IDocHostUIHandler2
               || riid == IID_IOleControlSite
               || riid == IID_IOleInPlaceSiteEx
               || riid == IID_IOleCommandTarget
               || riid == IID_IAxWinHostWindow) {
        *ppvObject = 0;
        return E_NOINTERFACE;
    } else {
        if (FILELog::ReportingLevel() >= logDEBUG) {
            char riid_name[128];
            GUID_TO_CHAR(&riid, riid_name, _countof(riid_name));
            LOG_DEBUG << "OleClientSite::QueryInterface(): "
                         "unknown interface, riid = " << riid_name;
        }
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
    return S_OK;
}
ULONG STDMETHODCALLTYPE OleClientSite::AddRef(void) {
    return 1;
}
ULONG STDMETHODCALLTYPE OleClientSite::Release(void) {
    return 1;
}
HRESULT STDMETHODCALLTYPE OleClientSite::SaveObject(void) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleClientSite::GetMoniker(
        /* [in] */ DWORD dwAssign,
        /* [in] */ DWORD dwWhichMoniker,
        /* [out] */ IMoniker **ppmk) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE OleClientSite::GetContainer(
        /* [out] */ IOleContainer **ppContainer) {
    *ppContainer = 0;
    return E_NOINTERFACE;
}
HRESULT STDMETHODCALLTYPE OleClientSite::ShowObject(void) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleClientSite::OnShowWindow(
        /* [in] */ BOOL fShow) {
    return S_OK;
}
HRESULT STDMETHODCALLTYPE OleClientSite::RequestNewObjectLayout(void) {
    return E_NOTIMPL;
}
