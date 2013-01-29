// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

template <class TopFrame>
class ExternalDispatch : public IDispatch {
public:
    TopFrame* topFrame_;

    ExternalDispatch(TopFrame* inTopFrame)
        : topFrame_(inTopFrame) {
    }
    HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObject) {
        *ppvObject = 0;
        return E_NOTIMPL;
        return topFrame_->GetOleClientSite()->QueryInterface(riid, ppvObject);
    }    
    ULONG STDMETHODCALLTYPE AddRef(void) {
        return 1;
    }    
    ULONG STDMETHODCALLTYPE Release(void) {
        return 1;
    }    
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo) {
        *pctinfo = 0;
        return E_NOTIMPL;
    }    
    HRESULT STDMETHODCALLTYPE GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo) {
        *ppTInfo = 0;
        return E_NOTIMPL;
    }    
    HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [out] */ DISPID *rgDispId) {
        if (riid != IID_NULL)
            return ResultFromScode(DISP_E_UNKNOWNINTERFACE);
        if (cNames == 1) {
            int id = topFrame_->ExternalIdentifier(rgszNames[0]);
            if (id) {
                rgDispId[0] = id;
                return S_OK;
            }
        }
        for (UINT i = 0; i < cNames; i++) {
            rgDispId[i] = DISPID_UNKNOWN;
        }
        return ResultFromScode(DISP_E_UNKNOWNNAME);
    }
    HRESULT STDMETHODCALLTYPE Invoke( 
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
            return ResultFromScode(DISP_E_UNKNOWNINTERFACE);
        pExcepInfo = 0;
        puArgErr = 0;
        if (wFlags & (DISPATCH_PROPERTYGET | DISPATCH_PROPERTYPUT 
                | DISPATCH_PROPERTYPUTREF)) {
            return ResultFromScode(DISP_E_MEMBERNOTFOUND);
        }
        if (wFlags & DISPATCH_METHOD) {
            if (pDispParams->cArgs != 0) {
                // Currently no support for arguments, only simple function
                // call. See BrowserEvents2::Invoke() on how to fetch
                // arguments or return a value.
                return ResultFromScode(DISP_E_BADPARAMCOUNT);
            }
            if (topFrame_->ExternalCall(dispId))
                return S_OK;
        }
        return ResultFromScode(DISP_E_MEMBERNOTFOUND);
    }
};
