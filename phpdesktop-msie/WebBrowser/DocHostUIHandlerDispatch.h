// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

// Mo¿na ten interfejs zaimplementowaæ na inny sposób,
// pobieaj¹c ICustomDoc z oleObject i odpalaj¹c ICustomDoc::setUIHandler().

// Pe³na dokumentacja programowanie interfejsów Internet Explorera:
// http://msdn.microsoft.com/en-us/library/ie/aa752038(v=vs.85).aspx

#include "keycodes.h"

class DocHostUIHandlerDispatch : public IDocHostUIHandlerDispatch
{
public:
	IOleClientSite* oleClientSite;
	WebBrowserFrameInterface* webframe;

	DocHostUIHandlerDispatch(IOleClientSite* oleClientSite, WebBrowserFrameInterface* inwebframe)
	{
		oleClientSite = oleClientSite;
		webframe = inwebframe;
	}

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

	// IDispatch

	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo)
	{
		*pctinfo = 0;
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID  lcid, ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames,
		LCID lcid, DISPID *rgDispId)
	{
		if (riid != IID_NULL) {
			return ResultFromScode(DISP_E_UNKNOWNINTERFACE);
		}
		return DISP_E_UNKNOWNNAME;
	}

	HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
	{
		return E_NOTIMPL;
		/*
		if (riid != IID_NULL) {
			return ResultFromScode(DISP_E_UNKNOWNINTERFACE);
		}
		if (wFlags & ~(DISPATCH_METHOD | DISPATCH_PROPERTYGET
			| DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF)) {
			return ResultFromScode(E_INVALIDARG);
		}
		return NOERROR;
		*/
	}

	// IDocHostUIHandler

	HRESULT STDMETHODCALLTYPE ShowContextMenu( 
		/* [in] */ DWORD dwID,
		/* [in] */ DWORD x,
		/* [in] */ DWORD y,
		/* [in] */ IUnknown *pcmdtReserved,
		/* [in] */ IDispatch *pdispReserved,
		/* [retval][out] */ HRESULT *dwRetVal)
	{
		*dwRetVal = S_OK;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetHostInfo( 
		/* [out][in] */ DWORD *pdwFlags,
		/* [out][in] */ DWORD *pdwDoubleClick)
	{
		// http://msdn.microsoft.com/en-us/library/ie/aa753277(v=vs.85).aspx
		*pdwFlags = DOCHOSTUIFLAG_DISABLE_HELP_MENU | DOCHOSTUIFLAG_NO3DBORDER;

		// http://msdn.microsoft.com/en-us/library/ie/aa753276(v=vs.85).aspx
		*pdwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ShowUI( 
		/* [in] */ DWORD dwID,
		/* [in] */ IUnknown *pActiveObject,
		/* [in] */ IUnknown *pCommandTarget,
		/* [in] */ IUnknown *pFrame,
		/* [in] */ IUnknown *pDoc,
		/* [retval][out] */ HRESULT *dwRetVal)
	{
		*dwRetVal = S_OK;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE HideUI( void)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE UpdateUI( void)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE EnableModeless( 
		/* [in] */ VARIANT_BOOL fEnable)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( 
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ResizeBorder( 
		/* [in] */ long left,
		/* [in] */ long top,
		/* [in] */ long right,
		/* [in] */ long bottom,
		/* [in] */ IUnknown *pUIWindow,
		/* [in] */ VARIANT_BOOL fFrameWindow)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
		/* [in] */ DWORD_PTR hWnd,
		/* [in] */ DWORD nMessage,
		/* [in] */ DWORD_PTR wParam,
		/* [in] */ DWORD_PTR lParam,
		/* [in] */ BSTR bstrGuidCmdGroup,
		/* [in] */ DWORD nCmdID,
		/* [retval][out] */ HRESULT *dwRetVal)
	{
		HRESULT hr = S_FALSE;

		// TODO:
		// VK_BACK = backspace (history back)
		if (nMessage == WM_KEYDOWN && wParam == VK_BACK) {
			// Check if the active element is <input type=text|search|...> or <textarea>,
			// if so then allow Backspace, as it will delete text and not Back History.
			// <input type=color> is a selectbox in opera
			// <input type=date> also selectbox
			// Inputs that are certain to be text: (checked in fifefox, chrome, opera)
			// - text, search, email, tel, url, number, time.
			wchar_t* tag = new wchar_t[50];
			wchar_t* typeattr = new wchar_t[50];
			if (webframe->GetActiveElement(tag, typeattr)) {
				
				_wcsupr_s(tag, 50);
				_wcsupr_s(typeattr, 50);

				//ASSERT((wcscmp(tag, L"INPUT") == 0), "tag != INPUT");
				//DEBUGW(tag);
				//DEBUG_INT(wcslen(tag));
				
				if (wcscmp(tag, L"INPUT") == 0
					&& (wcscmp(typeattr, L"TEXT") == 0 
						|| wcscmp(typeattr, L"SEARCH") == 0
						|| wcscmp(typeattr, L"EMAIL") == 0
						|| wcscmp(typeattr, L"TEL") == 0
						|| wcscmp(typeattr, L"URL") == 0
						|| wcscmp(typeattr, L"NUMBER") == 0
						|| wcscmp(typeattr, L"TIME") == 0
					)
				) {
					hr = S_FALSE;
				}
				else if (wcscmp(tag, L"TEXTAREA") == 0) {
					hr = S_FALSE;
				}
				else {
					hr = S_OK;
				}
				//DEBUGW2(tag, typeattr);
			}
			delete[] tag;
			delete[] typeattr;
		}
	
		// This seem to work, others are just to be sure:		
		// VK_F1 = help
		// VK_F5 = refresh

		if (nMessage == WM_KEYDOWN &&
			(wParam == VK_BROWSER_BACK || wParam == VK_BROWSER_FAVORITES
			|| wParam == VK_BROWSER_FORWARD || wParam == VK_BROWSER_HOME || wParam == VK_BROWSER_REFRESH
			|| wParam == VK_BROWSER_SEARCH || wParam == VK_BROWSER_STOP || wParam == VK_F1 || wParam == VK_F5
			|| wParam == VK_HELP || wParam == VK_MENU || wParam == VK_PRINT || wParam == VK_RMENU
			|| wParam == VK_LMENU || wParam == VK_ZOOM)
		) {
			// The message was handled.
			hr = S_OK;
		}
		
		// WM_SYSKEYDOWN = F10 or ALT is pressed.
		// Blocks: alt+left, alt+right - history backward/forward navigation.
		if (nMessage == WM_SYSKEYDOWN &&
			(wParam == VK_LEFT || wParam == VK_RIGHT)
		) {
			hr = S_OK;
		}

		// ctrl+f = find, allowed
		// ctrl+p = print, disable
		// ctrl+o = open, disable
		// ctrl+l = open, disable
		// ctrl+n = new window, disable
		if (nMessage == WM_KEYDOWN &&
			(wParam == VK_P || wParam == VK_O || wParam == VK_L || wParam == VK_N)
			&& (GetKeyState(VK_CONTROL)<0)
		) {
			hr = S_OK;
		}

		// ctrl+c, ctrl+x, ctrl+v, ctrl+z (undo)
		// ctrl+a (select all)
		// Let these shortcuts work.

		*dwRetVal = hr;
		return hr;
	}

	HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
		/* [out] */ BSTR *pbstrKey,
		/* [in] */ DWORD dw)
	{
		*pbstrKey = 0;
		return (S_FALSE);
	}

	HRESULT STDMETHODCALLTYPE GetDropTarget( 
		/* [in] */ IUnknown *pDropTarget,
		/* [out] */ IUnknown **ppDropTarget)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetExternal( 
		/* [out] */ IDispatch **ppDispatch)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE TranslateUrl( 
		/* [in] */ DWORD dwTranslate,
		/* [in] */ BSTR bstrURLIn,
		/* [out] */ BSTR *pbstrURLOut)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE FilterDataObject( 
		/* [in] */ IUnknown *pDO,
		/* [out] */ IUnknown **ppDORet)
	{
		return E_NOTIMPL;
	}
};
