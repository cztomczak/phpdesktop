// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// TODO: Implement IDocHostUIHandler2::GetOptionKeyPath()
//       so that you can set browser features that are
//       customizable only through registry.

#pragma once

#include "../defines.h"
#include <UrlMon.h>
#pragma comment(lib, "UrlMon.lib")
#include "version.h"

// INTERNETFEATURELIST enumeration (does not include IE8 or later):
// http://msdn.microsoft.com/en-us/library/ms537169(v=vs.85).aspx

// Internet Feature Controls, this is a complete list, but many
// of them cannot be changed at runtime:
// http://msdn.microsoft.com/en-us/library/ee330720(v=vs.85).aspx

// Additional features that can be changed only through
// registry or by implementing IDocHostUIHandler2::GetOptionKeyPath().

// FEATURE_RESTRICT_ABOUT_PROTOCOL_IE7
// FEATURE_AJAX_CONNECTIONEVENTS
// FEATURE_SHOW_APP_PROTOCOL_WARN_DIALOG
// FEATURE_BROWSER_EMULATION
// FEATURE_ENABLE_CLIPCHILDREN_OPTIMIZATION
// FEATURE_MANAGE_SCRIPT_CIRCULAR_REFS
// FEATURE_ENABLE_SCRIPT_PASTE_URLACTION_IF_PROMPT
// FEATURE_BLOCK_SETCAPTURE_XDOMAIN
// FEATURE_CROSS_DOMAIN_REDIRECT_MITIGATION
// FEATURE_DOWNLOAD_INITIATOR_HTTP_HEADER
// FEATURE_CFSTR_INETURLW_DRAGDROP_FORMAT
// FEATURE_BLOCK_CROSS_PROTOCOL_FILE_NAVIGATION
// FEATURE_IE6_DEFAULT_FRAME_NAVIGATION_BEHAVIOR
// FEATURE_VIEWLINKEDWEBOC_IS_UNSAFE
// FEATURE_GPU_RENDERING
// FEATURE_MAXCONNECTIONSPER1_0SERVER
// FEATURE_MAXCONNECTIONSPERSERVER
// FEATURE_IFRAME_MAILTO_THRESHOLD
// FEATURE_MIME_TREAT_IMAGE_AS_AUTHORITATIVE
// FEATURE_SECURITYBAND
// FEATURE_IVIEWOBJECTDRAW_DMLT9_WITH_GDI
// FEATURE_NINPUT_LEGACYMODE
// FEATURE_BLOCK_LMZ_IMG
// FEATURE_BLOCK_LMZ_OBJECT
// FEATURE_BLOCK_LMZ_SCRIPT
// FEATURE_ISOLATE_NAMED_WINDOWS
// FEATURE_DOWNLOAD_PROMPT_META_CONTROL
// FEATURE_SCRIPTURL_MITIGATION
// FEATURE_WARN_ON_SEC_CERT_REV_FAILED
// FEATURE_LOAD_SHDOCLC_RESOURCES
// FEATURE_SPELLCHECKING
// FEATURE_STATUS_BAR_THROTTLING
// FEATURE_RESTRICT_CDL_CLSIDSNIFF
// FEATURE_SHIM_MSHELP_COMBINE
// FEATURE_WEBOC_DOCUMENT_ZOOM
// FEATURE_WEBOC_MOVESIZECHILD
// FEATURE_WEBSOCKET
// FEATURE_WEBSOCKET_AUTHPROMPT
// FEATURE_WEBSOCKET_CLOSETIMEOUT
// FEATURE_WEBSOCKET_MAXCONNECTIONSPERSERVER
// FEATURE_WEBSOCKET_FOLLOWHTTPREDIRECT
// FEATURE_RESTRICTED_ZONE_WHEN_FILE_NOT_FOUND

// INTERNETFEATURELIST for IE 6.0 SP2 that can be set
// per process:

// FEATURE_OBJECT_CACHING
// FEATURE_ZONE_ELEVATION
// FEATURE_MIME_HANDLING
// FEATURE_MIME_SNIFFING
// FEATURE_WINDOW_RESTRICTIONS
// FEATURE_WEBOC_POPUPMANAGEMENT
// FEATURE_BEHAVIORS
// FEATURE_DISABLE_MK_PROTOCOL
// FEATURE_SECURITYBAND
// FEATURE_RESTRICT_ACTIVEXINSTALL
// FEATURE_VALIDATE_NAVIGATE_URL
// FEATURE_RESTRICT_FILEDOWNLOAD
// FEATURE_ADDON_MANAGEMENT
// FEATURE_HTTP_USERNAME_PASSWORD_DISABLE
// FEATURE_SAFE_BINDTOOBJECT
// FEATURE_UNC_SAVEDFILECHECK
// FEATURE_GET_URL_DOM_FILEPATH_UNENCODED
// FEATURE_TABBED_BROWSING
// FEATURE_SSLUX
// FEATURE_DISABLE_NAVIGATION_SOUNDS
// FEATURE_DISABLE_LEGACY_COMPRESSION
// FEATURE_FORCE_ADDR_AND_STATUS
// FEATURE_XMLHTTP
// FEATURE_DISABLE_TELNET_PROTOCOL
// FEATURE_FEEDS
// FEATURE_BLOCK_INPUT_PROMPTS

// Cannot be set per process:

// FEATURE_PROTOCOL_LOCKDOWN
// FEATURE_LOCALMACHINE_LOCKDOWN

// Following features found in Wine 1.1.3, but setting them
// per process fails:
// (http://source.winehq.org/source/include/urlmon.idl?v=wine-1.1.3)

// FEATURE_DOMSTORAGE
// FEATURE_XDOMAINREQUEST
// FEATURE_DATAURI
// FEATURE_AJAX_CONNECTIONSERVICES

void SetInternetFeatures() {
    // We must check IE version, it needs to be at least 6.0 SP2,
    // as CoInternetSetFeatureEnabled() is not available in earlier
    // versions:
    // http://support.microsoft.com/kb/969393

    if (!IsAtLeastInternetExplorer60ServicePack2())
        return;

    HRESULT hres;

    // Internet Explorer 7. Enable or disable the native
    // XMLHttpRequest object.
    hres = CoInternetSetFeatureEnabled(FEATURE_XMLHTTP,
            SET_FEATURE_ON_PROCESS, TRUE);
    LOG_DEBUG << "FEATURE_XMLHTTP succeeded: " << SUCCEEDED(hres);

    // Constrain to the viewable desktop area and force to
    // have a status bar. Also, restrict the size and position
    // of pop-up windows without chrome so that they cannot
    // overlay important information on their parent windows
    // and cannot overlay system dialog box information.
    hres = CoInternetSetFeatureEnabled(FEATURE_WINDOW_RESTRICTIONS,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_WINDOW_RESTRICTIONS succeeded: "
              << SUCCEEDED(hres);

    // Enable applications hosting the WebBrowser Control to
    // receive the default Internet Explorer pop-up management
    // behavior.
    hres = CoInternetSetFeatureEnabled(FEATURE_WEBOC_POPUPMANAGEMENT,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_WEBOC_POPUPMANAGEMENT succeeded: "
              << SUCCEEDED(hres);

    // Block ActiveX controls on pages that instantiate or
    // prompt for ActiveX controls to be installed or when
    // the control is an update to a control that is not
    // installed.
    hres = CoInternetSetFeatureEnabled(FEATURE_RESTRICT_ACTIVEXINSTALL,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_RESTRICT_ACTIVEXINSTALL succeeded: "
              << SUCCEEDED(hres);

    // Block file downloads that navigate to a resource,
    // that display a file download dialog box, or that
    // are not initiated explicitly by a user action (for
    // example, a mouse click or key press).
    hres = CoInternetSetFeatureEnabled(FEATURE_RESTRICT_FILEDOWNLOAD,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_RESTRICT_FILEDOWNLOAD succeeded: "
              << SUCCEEDED(hres);

    // Internet Explorer 7. Disable the sounds that are
    // played when the user opens a link.
    hres = CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS,
                                       SET_FEATURE_ON_PROCESS, TRUE);
    LOG_DEBUG << "FEATURE_DISABLE_NAVIGATION_SOUNDS succeeded: "
              << SUCCEEDED(hres);

    // Internet Explorer 7. Require new windows to include
    // both address and status bar.
    hres = CoInternetSetFeatureEnabled(FEATURE_FORCE_ADDR_AND_STATUS,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_FORCE_ADDR_AND_STATUS succeeded: "
              << SUCCEEDED(hres);

    // Allow Binary Behaviors to run securely.
    // TRUE to disable, FALSE to enable.
    hres = CoInternetSetFeatureEnabled(FEATURE_BEHAVIORS,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_BEHAVIORS succeeded: "
              << SUCCEEDED(hres);

    // Evaluate the "saved from url" information for files
    // on a UNC share. This feature increases security on
    // UNC paths but at a performance cost.
    hres = CoInternetSetFeatureEnabled(FEATURE_UNC_SAVEDFILECHECK,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_UNC_SAVEDFILECHECK succeeded: "
              << SUCCEEDED(hres);

    // Internet Explorer 7. Enable or disable the pop-up
    // blocker to show input prompt dialog boxes. Used pop-up
    // blocker to mitigate the risk of spoofing.
    hres = CoInternetSetFeatureEnabled(FEATURE_BLOCK_INPUT_PROMPTS,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_BLOCK_INPUT_PROMPTS succeeded: "
              << SUCCEEDED(hres);

    // Internet Explorer 7. Enable tabbed browsing shortcuts
    // and notifications.
    hres = CoInternetSetFeatureEnabled(FEATURE_TABBED_BROWSING,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_TABBED_BROWSING succeeded: "
              << SUCCEEDED(hres);

    // The FEATURE_ADDON_MANAGEMENT feature enables applications hosting
    // the WebBrowser Control to respect add-on management selections made
    // using the Add-on Manager feature of Internet Explorer. Add-ons
    // disabled by the user or by administrative group policy will also
    // be disabled in applications that enable this feature.
    hres = CoInternetSetFeatureEnabled(FEATURE_ADDON_MANAGEMENT,
                                       SET_FEATURE_ON_PROCESS, FALSE);
    LOG_DEBUG << "FEATURE_ADDON_MANAGEMENT succeeded: "
              << SUCCEEDED(hres);

    /*
    Setting these features fails:

    FEATURE_DOMSTORAGE
    FEATURE_XDOMAINREQUEST
    FEATURE_DATAURI

    // Internet Explorer 8. When enabled, the FEATURE_DOMSTORAGE
    // feature allows Internet Explorer and applications hosting
    // the WebBrowser Control to use the Web Storage API.
    hres = CoInternetSetFeatureEnabled(
            (INTERNETFEATURELIST)FEATURE_DOMSTORAGE,
            SET_FEATURE_ON_PROCESS, TRUE);
    LOG_DEBUG << "FEATURE_DOMSTORAGE succeeded: "
              << SUCCEEDED(hres);

    // When enabled, the FEATURE_XDOMAINREQUEST feature enables
    // the XDomainRequest object, which represents a cross-domain
    // Asynchronous JavaScript and XML (AJAX) request.
    hres = CoInternetSetFeatureEnabled(
            (INTERNETFEATURELIST)FEATURE_XDOMAINREQUEST,
            SET_FEATURE_ON_PROCESS, TRUE);
    LOG_DEBUG << "FEATURE_XDOMAINREQUEST succeeded: "
              << SUCCEEDED(hres);

    // Internet Explorer 9.
    hres = CoInternetSetFeatureEnabled(
            (INTERNETFEATURELIST)FEATURE_DATAURI,
            SET_FEATURE_ON_PROCESS, TRUE);
    LOG_DEBUG << "FEATURE_DATAURI succeeded: "
              << SUCCEEDED(hres);
    */
}