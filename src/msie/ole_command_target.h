// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
class BrowserWindow;
#include <DocObj.h>

class OleCommandTarget : public IOleCommandTarget {
private:
    BrowserWindow* browserWindow_;
public:
    OleCommandTarget(BrowserWindow* browserWindowIn);
    HRESULT STDMETHODCALLTYPE QueryInterface(
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    HRESULT STDMETHODCALLTYPE QueryStatus(
        /* [in] */ const GUID *pguidCmdGroup,
        /* [in] */ ULONG cCmds,
        /* [out][in] */ OLECMD prgCmds[],
        /* [out][in] */ OLECMDTEXT *pCmdText);
    HRESULT STDMETHODCALLTYPE Exec(
        /* [in] */ const GUID *pguidCmdGroup,
        /* [in] */ DWORD nCmdID,
        /* [in] */ DWORD nCmdexecopt,
        /* [in] */ VARIANT *pvaIn,
        /* [out][in] */ VARIANT *pvaOut);
};
