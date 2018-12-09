// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <MsHtmHst.h>
class BrowserWindow;

class HtmlOmWindowServices : public IHTMLOMWindowServices {
private:
    BrowserWindow* browserWindow_;
public:
    HtmlOmWindowServices(BrowserWindow* inBrowserWindow);
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    // IHTMLOMWindowServices
    HRESULT STDMETHODCALLTYPE moveTo(LONG x, LONG y);
    HRESULT STDMETHODCALLTYPE moveBy(LONG x, LONG y);
    HRESULT STDMETHODCALLTYPE resizeTo(LONG x, LONG y);
    HRESULT STDMETHODCALLTYPE resizeBy(LONG x, LONG y);
};
