//
// ClickEvents.h
// Copyright (c) Czarek Tomczak. All rights reserved.
//

#pragma once

// See "SubmitEvents.h" for explanation in English.

/*
	Wykrywanie przejscia na nowa strone w przegladarce i w razie
	potrzeby blokowanie zewnêtrznych linkow ktore probuja sie odpalic
	w oknie przegladarki, dla nich osobne wywolanie systemowe "start http://".

	DWebBrowserEvents2::BeforeNavigate2 - nie jesteœmy w stanie rozró¿niæ, czy
		url wgrywany to nowy url dla danego okna czy url dla skryptu czy obrazka.

	Jest opcja, ¿eby podpi¹æ siê pod event "onunload" - jest on wywolywane tuz przed
	opuszczeniem strony - i je¿eli BeforeNavigate2 odpala siê po onunload to 
	moglibyœmy to wykorzystaæ i byæ pewni ¿e to g³ówne okno przegl¹darki zmienia
	lokacjê, a nie jakiœ skypt, styl css czy obrazek.

	Podpi¹æ pod "onunload" event mo¿emy wykorzystuj¹c:
		
	IHTMLWindow2::put_onunload(VARIANT p)
	IHTMLWindow3::attachEvent(BSTR event, IDispatch *pDisp, VARIANT_BOOL *pfResult)

	Jak pobraæ IHTMLWindow?
	IWebBrowser2::get_document() => IHTMLDocument2::get_parentWindow()

	SprawdŸ czy w BeforeNavigate2 w "headers" nie ma przypadkiem informacji, któr¹
	mo¿na by zidentyfikowaæ ró¿nego typu nawigacje: czy to skrypt, czy css, czy
	g³ówne okno przegl¹darki.

	----

	Zrób attachEvent na evencie "click" w ten sposób wy³apiesz wszystkie kliki na 
	linkach, sprawdzisz czy zewnêtrzny, jeœli tak to nie pozwolisz na odpalenie
	go w oknie przegl¹darki, lecz wywolasz zewnêtrzne polecenie "start http://".
	
	Dodatkowo trzeba wychwyciæ wszelkie wywo³ania "window.location", trzeba te¿
	je kontrolowaæ.
	
	- sprawdŸ czy ten attachEvent dla click wychwytuje te¿ clicki w iframie, bo
		w iframie pozwalamy na nawigacjê po zewnêtrznych stronach.

	Jak pobraæ IHTMLWindow3:
	1. IWebBrowser2->getdocument
	2. IHTMLDocument2->get_parentWindow
	3. HTMLWindow2->QueryInterface(IHTMLWindow3)

	Przyk³ad attachEvent:
	(http://webcache.googleusercontent.com/search?hl=pl&q=cache%3Ahttps%3A%2F%2Fforums.embarcadero.com%2Fthread.jspa%3FthreadID%3D18052%26tstart%3D4)

	CComBSTR onScrollName( TEXT( "onscroll" ) );
	VARIANT_BOOL result = VARIANT_TRUE;
	hr = window3->attachEvent( onScrollName, events, &result );
	if ( result != VARIANT_TRUE ) {}

	class Events : public IDispatch{
		GetTypeInfoCount : E_NOTIMPL
		GetTypeInfo : E_NOTIMPL
		GetIDsOfNames : E_NOTIMPL
		
		STDMETHODIMP CIE4Events::Invoke(DISPID dispidMember, REFIID riid, LCID 
		lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pvarResult,
		EXCEPINFO* pExcepInfo, UINT* puArgErr)
		{
			if ( !pDispParams ) {
			return E_INVALIDARG;
			}
			if ( _browserControl != NULL &&
			pDispParams->cArgs == 1 && pDispParams->rgvarg [0].vt == 
			VT_DISPATCH ) {

			ComPtrQI <IHTMLEventObj > event( ComPtrQI <IDispatch> ( 
			pDispParams->rgvarg [0].pdispVal ) );

			if ( event ) {

			event IHTMLEventObj::get_srcElement(IHTMLElement **p);
			
			event IHTMLEventObj::put_returnValue(VARIANT v)
			false	Default action of the event on the source object is canceled.

		}

	}

	IDispEventSimpleImpl:
	http://gears.googlecode.com/svn/trunk/gears/base/common/html_event_monitor_ie.cc

*/

class ClickEvents : public IDispatch
{
public:
	ClickEvents(){}

	// IDispatch


};
