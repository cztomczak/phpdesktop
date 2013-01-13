// not used currently... as we implemented DocHostUIHandlerDispatch()
// and this is nore more needed.

//
// OleInPlaceFrame.h
// Copyright (c) Czarek Tomczak. All rights reserved.
//

#pragma once

class OleInPlaceFrame : public IOleInPlaceFrame
{
public:
	IOleClientSite* oleClientSite;
	WebBrowserFrameInterface* webframe;
	
	OleInPlaceFrame(IOleClientSite* oleClientSite, WebBrowserFrameInterface* inwebframe)
	{
		oleClientSite = oleClientSite;
		webframe = inwebframe;
	}
public:
	
	// IUnknown

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
	{
		return oleClientSite->QueryInterface(riid, ppvObj);
	}
	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return 1;
	}
	ULONG STDMETHODCALLTYPE Release(void)
	{
		return 1;
	}

	// IOleWindow

	HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd)
	{
		*lphwnd = webframe->View_GetHWND();
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode)
	{
		return E_NOTIMPL;
	}

	// IOleInPlaceUIWindow

	HRESULT STDMETHODCALLTYPE GetBorder(LPRECT lprectBorder)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE RequestBorderSpace(LPCBORDERWIDTHS pborderwidths)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE SetBorderSpace(LPCBORDERWIDTHS pborderwidths)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE SetActiveObject(IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
	{
		return S_OK;
	}

	// IOleInPlaceFrame

	HRESULT STDMETHODCALLTYPE InsertMenus(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE SetMenu(HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RemoveMenus(HMENU hmenuShared)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE SetStatusText(LPCOLESTR pszStatusText)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE EnableModeless(BOOL fEnable)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE TranslateAccelerator(LPMSG lpmsg, WORD wID)
	{
		return E_NOTIMPL;
	}
};
