// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "WebBrowserFrameInterface.h"

class HTMLOMWindowServices : public IHTMLOMWindowServices
{
public:
	WebBrowserFrameInterface* webframe;

	HTMLOMWindowServices(WebBrowserFrameInterface* inwebframe)
	{
		webframe = inwebframe;
	}
	~HTMLOMWindowServices(){}

	// IUnknown

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		return E_NOTIMPL;
	}
	ULONG STDMETHODCALLTYPE AddRef()
	{
		return 1;
	}
	ULONG STDMETHODCALLTYPE Release()
	{
		return 1;
	}

	// IHTMLOMWindowServices

	HRESULT STDMETHODCALLTYPE moveTo(LONG x, LONG y)
	{
		SetWindowPos(webframe->GetHWND(), NULL, (int) x, (int) y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE moveBy(LONG x, LONG y)
	{
		return E_NOTIMPL;
	}
	HRESULT STDMETHODCALLTYPE resizeTo(LONG x, LONG y)
	{
		SetWindowPos(webframe->GetHWND(), NULL, 0, 0, (int) x, (int) y, SWP_NOMOVE | SWP_NOZORDER);
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE resizeBy(LONG x, LONG y)
	{
		return E_NOTIMPL;
	}
};
