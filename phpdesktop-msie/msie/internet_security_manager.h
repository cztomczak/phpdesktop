// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <UrlMon.h>
class BrowserWindow;

class InternetSecurityManager : public IInternetSecurityManager {
private:
    BrowserWindow* browserWindow_;
public:
    InternetSecurityManager(BrowserWindow* inBrowserWindow);
    // IInternetSecurityManager
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    HRESULT STDMETHODCALLTYPE SetSecuritySite(
            /* [in] */ IInternetSecurityMgrSite *pSite);
    HRESULT STDMETHODCALLTYPE GetSecuritySite(
            /* [out] */ IInternetSecurityMgrSite **ppSite);
    HRESULT STDMETHODCALLTYPE MapUrlToZone(
            /* [in] */ LPCWSTR pwszUrl,
            /* [out] */ DWORD *pdwZone,
            /* [in] */ DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE GetSecurityId(
            /* [in] */ LPCWSTR pwszUrl,
            /* [out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved);
    HRESULT STDMETHODCALLTYPE ProcessUrlAction(
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ DWORD dwAction,
            /* [out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved);
    HRESULT STDMETHODCALLTYPE QueryCustomPolicy(
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ REFGUID guidKey,
            /* [out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwReserved);
    HRESULT STDMETHODCALLTYPE SetZoneMapping(
            /* [in] */ DWORD dwZone,
            /* [in] */ LPCWSTR lpszPattern,
            /* [in] */ DWORD dwFlags);
    HRESULT STDMETHODCALLTYPE GetZoneMappings(
            /* [in] */ DWORD dwZone,
            /* [out] */ IEnumString **ppenumString,
            /* [in] */ DWORD dwFlags);
};
