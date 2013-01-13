// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

/*
	Author: Czarek Tomczak <czarek.tomczak@@gmail.com>

	How to use WebBrowser:
	
	1. You should include "DEBUG.h" which defines macro ASSERT_EXIT(bool, msb).
	2. Before creating window include "InternetFeatures.h" and call SetInternetFeatures(),
		this gets rid of any restrictions in IE webbrowser.
	3. MainFrame should inherit:	
		public WebBrowserFrame<MainFrame>,
		public FullScreenFrame<MainFrame>
	4. MainView member should be named "rootview".
	5. If you call SetFullScreen() you must call ShowTaskBar() in WM_CLOSE message,
		check isfullscreen: if (isfullscreen) ShowTaskBar(true).
*/

#include "WebBrowserFrameInterface.h"
#include "OleClientSite.h"
#include "DocHostUIHandlerDispatch.h"

template <class T, bool t_bHasSip = true>
class WebBrowserFrame 
	:
	public WebBrowserFrameInterface
{
public:

	OleClientSite oleClientSite;
	DocHostUIHandlerDispatch docHostUIHandlerDispatch;
	T* self;

	WebBrowserFrame()
		:
		oleClientSite((WebBrowserFrameInterface*)this),
		docHostUIHandlerDispatch(DocHostUIHandlerDispatch((IOleClientSite*)&oleClientSite, (WebBrowserFrameInterface*)this))
	{
		self = static_cast<T*>(this);
	}

	void CreateBrowser(wchar_t* navigateurl)
	{
		self->m_hWndClient = self->rootview.Create(self->m_hWnd, self->rcDefault, (LPCTSTR) navigateurl,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
		ASSERT_EXIT(self->m_hWndClient, "self->m_hWndClient");

		// IOleClientSite
		HRESULT hr;

		CComQIPtr<IOleObject> oleObject;
		int ctrlid = self->rootview.GetDlgCtrlID();
		CComQIPtr<IWebBrowser2> webBrowser;
		
		hr = self->GetDlgControl(ctrlid, IID_IWebBrowser2, (void**) &webBrowser);

		ASSERT_EXIT(SUCCEEDED(hr), "self->GetDlgControl(IID_IWebBrowser2) failed");
		ASSERT_EXIT(webBrowser, "webBrowser is empty");

		hr = webBrowser->QueryInterface(IID_IOleObject, (void**) &oleObject);

		ASSERT_EXIT(SUCCEEDED(hr), "webBrowser->QueryInterface(IID_IOleObject)");
		ASSERT_EXIT(oleObject, "oleObject is empty");		

		oleObject->SetClientSite(&oleClientSite);
		self->rootview.SetExternalUIHandler(&docHostUIHandlerDispatch);

		// Do not allow displaying files dragged into the window.
		webBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);

		//self->SetFullScreen(true); // see also ShowTaskBar(true) in OnClose() message.
	}

	virtual HWND GetHWND()
	{
		return self->m_hWnd;
	}

	virtual HWND View_GetHWND()
	{
		return self->rootview.m_hWnd;
	}

	virtual IOleClientSite* GetOleClientSite()
	{
		return &oleClientSite;
	}

	virtual int View_GetDlgCtrlID()
	{
		return self->rootview.GetDlgCtrlID();
	}

	virtual HRESULT Self_GetDlgControl(int nID, REFIID iid, void** ppCtrl)
	{
		return self->GetDlgControl(nID, iid, ppCtrl);
	}

	virtual bool GetActiveElement(wchar_t* outTag, wchar_t* outTypeAttr)
	{
		CComQIPtr<IWebBrowser2> webBrowser;
		HRESULT hr = self->GetDlgControl(self->rootview.GetDlgCtrlID(), IID_IWebBrowser2, (void**) &webBrowser);
		ASSERT_EXIT(SUCCEEDED(hr), "self->GetDlgControl(IID_IWebBrowser2) failed");

		CComQIPtr<IDispatch> dispatch;
		hr = webBrowser->get_Document(&dispatch);
		ASSERT_EXIT(SUCCEEDED(hr), "webBrowser->getDocument()");
		if (dispatch == NULL) {
			outTag = 0;
			return false;
		}

		CComQIPtr<IHTMLDocument2> htmlDocument2;
		hr = dispatch->QueryInterface(IID_IHTMLDocument2, (void**) &htmlDocument2);
		ASSERT_EXIT(SUCCEEDED(hr), "dispatch->QueryInterface(IID_IHTMLDocument2)");
		if (htmlDocument2 == NULL) {
			// Is it possible to return NULL for document?
			outTag = 0;
			return false;
		}

		CComQIPtr<IHTMLElement> htmlElement;
		hr = htmlDocument2->get_activeElement(&htmlElement);
		ASSERT_EXIT(SUCCEEDED(hr), "htmlDocument2->get_ActiveElement()");
		if (htmlElement == NULL) {
			// htmlElement might be NULL when document is not yet loaded.
			outTag = 0;
			return false;
		}

		CComBSTR tag;
		hr = htmlElement->get_tagName(&tag);
		ASSERT_EXIT(SUCCEEDED(hr), "htmlElement->get_tagName()");
		swprintf_s(outTag, 50, L"%s", tag.m_str);
		
		CComBSTR type(L"type");
		VARIANT attrvalue;
		VariantInit(&attrvalue);
		hr = htmlElement->getAttribute(type, 0 | 2, &attrvalue);
		ASSERT_EXIT(SUCCEEDED(hr), "htmlElement->getAttribute()");
		
		// Other way of doing this (found example in google):
		// hr = VariantChangeType(&attrvalue, &attrvalue, VARIANT_LOCALBOOL, VT_BSTR);
		// ASSERT_EXIT(SUCCEEDED(hr), "VariantChangeType()");
		// DEBUG_INT(attrvalue.vt); // (body)1==VT_NULL, (input)8==VT_BSTR
		// ASSERT_EXIT((attrvalue.vt == VT_BSTR), "attrvalue.vt != VT_BSTR");

		if (attrvalue.vt == VT_BSTR) {
			swprintf_s(outTypeAttr, 50, L"%s", attrvalue.bstrVal);
		} else {
			swprintf_s(outTypeAttr, 50, L"%s", L"");
		}
		
		return true;
	}
};
