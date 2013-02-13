// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "external_dispatch.h"
#include "browser_window.h"
#include <MsHtmdid.h>
#include "../log.h"
#include "../debug.h"

#include "INITGUID.H" 
// 30510406-98b5-11cf-bb82-00aa00bdce0b
DEFINE_GUID(IID_IMyUnknown1, 0x30510406, 0x98b5, 0x11cf, 
            0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b);
// 9bcb0016-bc2a-47b7-8154-8580a15c3ff0
DEFINE_GUID(IID_IMyUnknown2, 0x9bcb0016, 0xbc2a, 0x47b7,
            0x81, 0x54, 0x85, 0x80, 0xa1, 0x5c, 0x3f, 0xf0);
// 719c3050-f9d3-11cf-a493-00400523a8a0
DEFINE_GUID(IID_IMyUnknown3, 0x719c3050, 0xf9d3, 0x11cf,
            0xa4, 0x93, 0x00, 0x40, 0x05, 0x23, 0xa8, 0xa0);
// a0aac450-a77b-11cf-91d0-00aa00c14a7c
DEFINE_GUID(IID_IMyUnknown4, 0xa0aac450, 0xa77b, 0x11cf,
            0x91, 0xd0, 0x00, 0xaa, 0x00, 0xc1, 0x4a, 0x7c);

ExternalDispatch::ExternalDispatch(BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow) {
}
HRESULT STDMETHODCALLTYPE ExternalDispatch::QueryInterface( 
        /* [in] */ REFIID riid,
        /* [out] */ void **ppvObject) {
    // Asking for:
    // 00000008-0000-0000-c000-000000000046 - IProxyManager
    // 
    // a6ef9860-c720-11d0-9337-00a0c90dcaa9 - IDispatchEx
    if (ppvObject == 0)
        return E_POINTER;
    if (riid == IID_IUnknown) {
        *ppvObject = static_cast<IUnknown*>(this);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "ExternalDispatch::QueryInterface(): IUnknown";
            logged = true;
        }
    } else if (riid == IID_IDispatch) {
        *ppvObject = static_cast<IDispatch*>(this);
        static bool logged = false;
        if (!logged) {
            LOG_DEBUG << "ExternalDispatch::QueryInterface(): IUnknown";
            logged = true;
        }
    } else if (   riid == IID_IProxyManager
               || riid == IID_IDispatchEx
               || riid == IID_IMyUnknown1
               || riid == IID_IMyUnknown2
               || riid == IID_IMyUnknown3
               || riid == IID_IMyUnknown4) {
        *ppvObject = 0;
        return E_NOINTERFACE;
    } else {
        if (FILELog::ReportingLevel() >= logDEBUG) {
            char riid_name[128];
            GUID_TO_CHAR(&riid, riid_name, _countof(riid_name));
            LOG_DEBUG << "ExternalDispatch::QueryInterface(): "
                         "unknown interface, riid = " << riid_name;
        }
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
    return S_OK;
}    
ULONG STDMETHODCALLTYPE ExternalDispatch::AddRef(void) {
    return 1;
}    
ULONG STDMETHODCALLTYPE ExternalDispatch::Release(void) {
    return 1;
}    
HRESULT STDMETHODCALLTYPE ExternalDispatch::GetTypeInfoCount( 
        /* [out] */ UINT *pctinfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE ExternalDispatch::GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ ITypeInfo **ppTInfo) {
    return E_NOTIMPL;
}    
HRESULT STDMETHODCALLTYPE ExternalDispatch::GetIDsOfNames( 
        /* [in] */ REFIID riid,
        /* [in] */ LPOLESTR *rgszNames,
        /* [in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [out] */ DISPID *rgDispId) {
    if (riid != IID_NULL)
        return DISP_E_UNKNOWNINTERFACE;
    if (cNames == 1) {
        int id = browserWindow_->ExternalIdentifier(rgszNames[0]);
        if (id) {
            rgDispId[0] = id;
            return S_OK;
        }
    }
    for (UINT i = 0; i < cNames; i++) {
        rgDispId[i] = DISPID_UNKNOWN;
    }
    return DISP_E_UNKNOWNNAME;
}
HRESULT STDMETHODCALLTYPE ExternalDispatch::Invoke( 
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
    if (wFlags & (DISPATCH_PROPERTYGET | DISPATCH_PROPERTYPUT 
            | DISPATCH_PROPERTYPUTREF)) {
        return DISP_E_MEMBERNOTFOUND;
    }
    if (wFlags & DISPATCH_METHOD) {
        if (pDispParams->cArgs != 0) {
            // Currently no support for arguments, only simple function
            // call. See BrowserEvents2::Invoke() on how to fetch
            // arguments or return a value.
            return DISP_E_BADPARAMCOUNT;
        }
        if (browserWindow_->ExternalCall(dispId))
            return S_OK;
    }
    return DISP_E_MEMBERNOTFOUND;
}
