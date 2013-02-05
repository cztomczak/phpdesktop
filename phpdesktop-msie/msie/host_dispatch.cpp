// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "host_dispatch.h"
#include "browser_window.h"
#include <MsHtmdid.h>
#include "../log.h"
#include "../settings.h"

HostDispatch::HostDispatch(BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow) {
}
HRESULT STDMETHODCALLTYPE HostDispatch::QueryInterface( 
        /* [in] */ REFIID riid,
        /* [out] */ void **ppvObject) {
    *ppvObject = 0;
    return E_NOTIMPL;
}    
ULONG STDMETHODCALLTYPE HostDispatch::AddRef(void) {
    return 1;
}    
ULONG STDMETHODCALLTYPE HostDispatch::Release(void) {
    return 1;
}    
HRESULT STDMETHODCALLTYPE HostDispatch::GetTypeInfoCount( 
        /* [out] */ UINT *pctinfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE HostDispatch::GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ ITypeInfo **ppTInfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE HostDispatch::GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [out] */ DISPID *rgDispId) {
    if (riid != IID_NULL)
        return DISP_E_UNKNOWNINTERFACE;
    for (UINT i = 0; i < cNames; i++) {
        rgDispId[i] = DISPID_UNKNOWN;
    }
    return DISP_E_UNKNOWNNAME;
}
HRESULT STDMETHODCALLTYPE HostDispatch::Invoke( 
        /* [in] */ DISPID dispId,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS *pDispParams,
        /* [out] */ VARIANT *pVarResult,
        /* [out] */ EXCEPINFO *pExcepInfo,
        /* [out] */ UINT *puArgErr) {
    
    // When returning a result, you must check whether pVarResult
    // is not NULL and initialize it using VariantInit(). If it's
    // NULL then it doesn't expect a result.
    
    if (riid != IID_NULL)
        return DISP_E_UNKNOWNINTERFACE;
    pExcepInfo = 0;
    puArgErr = 0;
    json_value* settings = GetApplicationSettings();
    bool silent_operations = (*settings)["msie"]["silent_operations"];
    
    // DLCTL_* constants:
    // http://msdn.microsoft.com/en-us/library/aa741313(v=vs.85).aspx
    // #Download_Control

    if (dispId == DISPID_AMBIENT_DLCONTROL) {
        LOG_DEBUG << "DISPID_AMBIENT_DLCONTROL";
        // DLCTL_BGSOUNDS - The browsing component will play background 
        //                  sounds associated with the document.
        // DLCTL_DLIMAGES - The browsing component will download images from 
        //                  the server.
        // DLCTL_VIDEOS - The browsing component will play any video clips 
        //                that are contained in the document.
        // DLCTL_PRAGMA_NO_CACHE - The browsing component will force the 
        //                         request through to the server and ignore 
        //                         the proxy, even if the proxy indicates that
        //                         the data is up to date. 
        // DLCTL_RESYNCHRONIZE - The browsing component will ignore what is 
        //                       in the cache and ask the server for updated 
        //                       information. The cached information will be 
        //                       used if the server indicates that the cached 
        //                       information is up to date.
        // DLCTL_SILENT - The browsing component will not display any user 
        //                interface.
        pVarResult->vt = VT_I4;
        pVarResult->lVal = DLCTL_DLIMAGES | DLCTL_VIDEOS 
                | DLCTL_PRAGMA_NO_CACHE | DLCTL_RESYNCHRONIZE;
        if (silent_operations) {
            pVarResult->lVal |= DLCTL_SILENT;
        }
        return S_OK;
    }

    return DISP_E_MEMBERNOTFOUND;
}
