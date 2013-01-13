//
// InternetFeatures.h
// Copyright (c) Czarek Tomczak. All rights reserved.
//

#pragma once

#include <UrlMon.h>
#include "IEVersion.h"

// ie9
#define FEATURE_DOMSTORAGE 28 // DomStorage in JS
#define FEATURE_XDOMAINREQUEST 29 // XDomainRequest object in JS ( cross-domain Asynchronous JavaScript and XML (AJAX) request)
#define FEATURE_DATAURI 30 // <img src="data:">
#define FEATURE_AJAX_CONNECTIONSERVICES 31 // deprecated

void SetInternetFeatures()
{
	// We must check IE version, it needs to be at least 6.0 SP2 (Internet Explorer 6 for Windows XP SP2).
	// http://support.microsoft.com/kb/969393
	// If it's lower then function "CoInternetSetFeatureEnabled" is not available.

	if (!IsAtLeastIE60SP2()) {
		return;
	}

	HRESULT	hres;

	// http://msdn.microsoft.com/en-us/library/ms537169(VS.85).aspx

	// ==== ASSERT() info ====
	// Do not check the hres, as some features might not be supported on IE6 or IE7 or IE8,
	// and we support all browsers >= IE6 SP2.

	hres = CoInternetSetFeatureEnabled(FEATURE_XMLHTTP, SET_FEATURE_ON_PROCESS, TRUE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_XMLHTTP");

	hres = CoInternetSetFeatureEnabled(FEATURE_WINDOW_RESTRICTIONS, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_WINDOW_RESTRICTIONS");
	
	hres = CoInternetSetFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_WEBOC_POPUPMANAGEMENT");
	
	/*
	The SET_FEATURE_ON_PROCESS flag is available for all values of INTERNETFEATURELIST except
	FEATURE_LOCALMACHINE_LOCKDOWN. All other values of dwFlags are available only when FeatureEntry is
	FEATURE_LOCALMACHINE_LOCKDOWN or FEATURE_PROTOCOL_LOCKDOWN.

	hres = CoInternetSetFeatureEnabled(FEATURE_LOCALMACHINE_LOCKDOWN, SET_FEATURE_ON_PROCESS, FALSE);
	ASSERT(SUCCEEDED(hres), "FEATURE_LOCALMACHINE_LOCKDOWN");
	*/	

	hres = CoInternetSetFeatureEnabled(FEATURE_RESTRICT_ACTIVEXINSTALL, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_RESTRICT_ACTIVEXINSTALL");

	hres = CoInternetSetFeatureEnabled(FEATURE_RESTRICT_FILEDOWNLOAD, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_RESTRICT_FILEDOWNLOAD");

	hres = CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, TRUE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_DISABLE_NAVIGATION_SOUNDS");

	hres = CoInternetSetFeatureEnabled(FEATURE_FORCE_ADDR_AND_STATUS, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_FORCE_ADDR_AND_STATUS");

	hres = CoInternetSetFeatureEnabled(FEATURE_BEHAVIORS, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_BEHAVIORS");

	hres = CoInternetSetFeatureEnabled(FEATURE_UNC_SAVEDFILECHECK, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_UNC_SAVEDFILECHECK");

	hres = CoInternetSetFeatureEnabled(FEATURE_BLOCK_INPUT_PROMPTS, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_BLOCK_INPUT_PROMPTS");	

	hres = CoInternetSetFeatureEnabled(FEATURE_BEHAVIORS, SET_FEATURE_ON_PROCESS, TRUE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_BEHAVIORS");	

	hres = CoInternetSetFeatureEnabled(FEATURE_TABBED_BROWSING, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_TABBED_BROWSING");

	// The FEATURE_ADDON_MANAGEMENT feature enables applications hosting 
	// the WebBrowser Control to respect add-on management selections made
	// using the Add-on Manager feature of Internet Explorer. Add-ons 
	// disabled by the user or by administrative group policy will also
	// be disabled in applications that enable this feature.
	hres = CoInternetSetFeatureEnabled(FEATURE_ADDON_MANAGEMENT, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_ADDON_MANAGEMENT");

	// ie9
	hres = CoInternetSetFeatureEnabled((INTERNETFEATURELIST)FEATURE_DOMSTORAGE, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_DOMSTORAGE");

	hres = CoInternetSetFeatureEnabled((INTERNETFEATURELIST)FEATURE_XDOMAINREQUEST, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_XDOMAINREQUEST");

	hres = CoInternetSetFeatureEnabled((INTERNETFEATURELIST)FEATURE_DATAURI, SET_FEATURE_ON_PROCESS, FALSE);
	//ASSERT(SUCCEEDED(hres), "FEATURE_DATAURI");	
}