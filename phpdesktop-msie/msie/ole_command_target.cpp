// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "ole_command_target.h"
#include "browser_window.h"
#include "../log.h"

// This interface is NOT USED.
// See ole_client_site.cpp > QueryInterface().

OleCommandTarget::OleCommandTarget(BrowserWindow* browserWindowIn)
        : browserWindow_(browserWindowIn) {
}
HRESULT STDMETHODCALLTYPE OleCommandTarget::QueryInterface( 
        /* [in] */ REFIID riid,
        /* [out] */ void **ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(riid, ppvObject);
}
ULONG STDMETHODCALLTYPE OleCommandTarget::AddRef() {
    return 1;
}
ULONG STDMETHODCALLTYPE OleCommandTarget::Release() {
    return 1;
}
HRESULT STDMETHODCALLTYPE OleCommandTarget::QueryStatus( 
        /* [in] */ const GUID *pguidCmdGroup,
        /* [in] */ ULONG cCmds,
        /* [out][in] */ OLECMD prgCmds[],
        /* [out][in] */ OLECMDTEXT *pCmdText) {
    if (prgCmds == NULL)
        return E_POINTER;
    bool cmdGroupFound = false;
    for (ULONG nCmd = 0; nCmd < cCmds; nCmd++) {
        LOG_DEBUG << "OleCommandTarget::QueryStatus(): cmdID = " << prgCmds[nCmd].cmdID;
        prgCmds[nCmd].cmdf = 0;
    }
    return OLECMDERR_E_UNKNOWNGROUP;
}
HRESULT STDMETHODCALLTYPE OleCommandTarget::Exec( 
        /* [in] */ const GUID *pguidCmdGroup,
        /* [in] */ DWORD nCmdID,
        /* [in] */ DWORD nCmdexecopt,
        /* [in] */ VARIANT *pvaIn,
        /* [out][in] */ VARIANT *pvaOut) {
    // LOG_DEBUG << "OleCommandTarget::Exec(): nCmdID = " << nCmdID;
    if (nCmdID == OLECMDID_CLOSE) {
        // Window is being closed.
    }
    return OLECMDERR_E_UNKNOWNGROUP;
}
