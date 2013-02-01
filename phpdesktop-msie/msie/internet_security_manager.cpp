// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "../defines.h"
#include "internet_security_manager.h"
#include "browser_window.h"

// These constants are not defined cause we set _WIN32_IE to
// _WIN32_IE_IE60SP2, so that IE7 and later features are not
// available in our code, so that backward compatibility is fine.
// But still, we only use this constants features for security
// manager, and that does not make any functionality broken.

#define URLACTION_LOOSE_XAML 0x00002402
#define URLACTION_MANAGED_SIGNED 0x00002001
#define URLACTION_MANAGED_UNSIGNED 0x00002004
#define URLACTION_WINDOWS_BROWSER_APPLICATIONS 0x00002400
#define URLACTION_WINFX_SETUP 0x00002600
#define URLACTION_XPS_DOCUMENTS 0x00002401
#define URLACTION_ALLOW_AUDIO_VIDEO 0x00002701
#define URLACTION_ALLOW_STRUCTURED_STORAGE_SNIFFING 0x00002703
#define URLACTION_ALLOW_XDOMAIN_SUBFRAME_RESIZE 0x00001408
#define URLACTION_FEATURE_CROSSDOMAIN_FOCUS_CHANGE 0x00002107
#define URLACTION_SHELL_PREVIEW 0x0000180F
#define URLACTION_SHELL_REMOTEQUERY 0x0000180E
#define URLACTION_SHELL_SECURE_DRAGSOURCE 0x0000180D
#define URLACTION_ALLOW_APEVALUATION 0x00002301
#define URLACTION_LOWRIGHTS 0x00002500
#define URLACTION_ALLOW_ACTIVEX_FILTERING 0x00002702
#define URLACTION_DOTNET_USERCONTROLS 0x00002005
#define URLACTION_FEATURE_DATA_BINDING              0x00002106
#define URLACTION_FEATURE_CROSSDOMAIN_FOCUS_CHANGE  0x00002107
#define URLACTION_SCRIPT_XSSFILTER                  0x00001409

InternetSecurityManager::InternetSecurityManager(
        BrowserWindow* inBrowserWindow)
        : browserWindow_(inBrowserWindow) {
    }
// IInternetSecurityManager
HRESULT STDMETHODCALLTYPE InternetSecurityManager::QueryInterface(
        REFIID riid, 
        void **ppvObject) {
    return browserWindow_->GetOleClientSite()->QueryInterface(riid, ppvObject);
}
ULONG STDMETHODCALLTYPE InternetSecurityManager::AddRef(void){
    return 1;
}
ULONG STDMETHODCALLTYPE InternetSecurityManager::Release(void) {
    return 1;
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::SetSecuritySite(
        /* [in] */ IInternetSecurityMgrSite *pSite) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::GetSecuritySite(
        /* [out] */ IInternetSecurityMgrSite **ppSite) {
    *ppSite = 0;
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::MapUrlToZone(
        /* [in] */ LPCWSTR pwszUrl, 
        /* [out] */ DWORD *pdwZone, 
        /* [in] */ DWORD dwFlags) {
    // const char *rgZoneNames[] = 
    // { "Local", "Intranet", "Trusted", "Internet", "Restricted" };
    // 0 == Local Machine. Gdy ustawimy 1 i w ProcessUrlAction()
    // damy return INET_E_DEFAULT_ACTION dla wszystkich, to od razu
    // wyskoczy bblad, ze nie moze utworzyc obiektu automatyzacji 
    // WScript.Shell, jezeli ustawimy 0, a w ProcessUrlAction damy
    // return INET_E_DEFAULT_ACTION to sie pojawi komunikat o
    // bezpieczenstwie.
    *pdwZone = 0;
    return S_OK;
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::GetSecurityId(
        /* [in] */ LPCWSTR pwszUrl, 
        /* [out] */ BYTE *pbSecurityId, 
        /* [out][in] */ DWORD *pcbSecurityId, 
        /* [in] */ DWORD_PTR dwReserved) {
    // Implementing this method allows different zones to interact with.
    // So for example an Internet webpage can call javascript on our 
    // Local desktop webpage, or the other way. We don't want any 
    // restrictions in webbrowser control.

    // http://stackoverflow.com/q/1498211/623622
    // http://msdn.microsoft.com/en-us/library/ie/ms537122(v=vs.85).aspx
    // (see comments on msdn).

    #define MY_SECURITY_DOMAIN "file:"
    int cbSecurityDomain = strlen(MY_SECURITY_DOMAIN);
    if (*pcbSecurityId >= MAX_SIZE_SECURITY_ID) {
        memset(pbSecurityId, 0, *pcbSecurityId);
        #pragma warning(disable:4996)
        strcpy((char*)pbSecurityId, MY_SECURITY_DOMAIN);
        #pragma warning(default:4996)
        // Last 4 bytes are <URLZONE> and then 3 zeros.
        pbSecurityId[ cbSecurityDomain + 1 ] = URLZONE_LOCAL_MACHINE;
        pbSecurityId[ cbSecurityDomain + 2 ] = 0;
        pbSecurityId[ cbSecurityDomain + 3 ] = 0;
        pbSecurityId[ cbSecurityDomain + 4 ] = 0;
        // plus the 4 bytes from above.
        *pcbSecurityId = (DWORD) cbSecurityDomain + 4;
        return S_OK;
    } else {
        pbSecurityId = 0;
        *pcbSecurityId = 0;
        return INET_E_DEFAULT_ACTION;
    }
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::ProcessUrlAction(
        /* [in] */ LPCWSTR pwszUrl, 
        /* [in] */ DWORD dwAction, 
        /* [out] */ BYTE *pPolicy, 
        /* [in] */ DWORD cbPolicy,
        /* [in] */ BYTE *pContext, 
        /* [in] */ DWORD cbContext, 
        /* [in] */ DWORD dwFlags, 
        /* [in] */ DWORD dwReserved) {
    
    // You can't set all actions to ALLOW, as it sometimes works
    // the other way, an ALLOW could set restrictions we don't 
    // want to. We have to check and seteach flag separately.

    // URLACTION flags: 
    // http://msdn.microsoft.com/en-us/library/ms537178(VS.85).aspx
    
    // Default settings: 
    // http://msdn.microsoft.com/en-us/library/ms537186(VS.85).aspx
    
    // URLPOLICY flags: 
    // http://msdn.microsoft.com/en-us/library/ms537179(VS.85).aspx

    // Not implemented:
    // URLACTION_ACTIVEX_OVERRIDE_DOMAINLIST (ie9)
    // URLACTION_INPRIVATE_BLOCKING

    switch (dwAction) {
        case URLACTION_ACTIVEX_CONFIRM_NOOBJECTSAFETY:
        case URLACTION_ACTIVEX_OVERRIDE_DATA_SAFETY:
        case URLACTION_ACTIVEX_OVERRIDE_SCRIPT_SAFETY:
        case URLACTION_FEATURE_BLOCK_INPUT_PROMPTS:
        case URLACTION_SCRIPT_OVERRIDE_SAFETY:
        case URLACTION_SHELL_EXTENSIONSECURITY:
        case URLACTION_ACTIVEX_NO_WEBOC_SCRIPT:
        case URLACTION_ACTIVEX_OVERRIDE_OBJECT_SAFETY:
        case URLACTION_ACTIVEX_OVERRIDE_OPTIN:
        case URLACTION_ACTIVEX_OVERRIDE_REPURPOSEDETECTION:
        case URLACTION_ACTIVEX_RUN:
        case URLACTION_ACTIVEX_SCRIPTLET_RUN:
        case URLACTION_ACTIVEX_DYNSRC_VIDEO_AND_ANIMATION:
        case URLACTION_ALLOW_RESTRICTEDPROTOCOLS:
        case URLACTION_AUTOMATIC_ACTIVEX_UI:
        case URLACTION_AUTOMATIC_DOWNLOAD_UI:
        case URLACTION_BEHAVIOR_RUN:
        case URLACTION_CLIENT_CERT_PROMPT:
        case URLACTION_COOKIES:
        case URLACTION_COOKIES_ENABLED:
        case URLACTION_COOKIES_SESSION:
        case URLACTION_COOKIES_SESSION_THIRD_PARTY:
        case URLACTION_COOKIES_THIRD_PARTY:
        case URLACTION_CROSS_DOMAIN_DATA:
        case URLACTION_DOWNLOAD_SIGNED_ACTIVEX:
        case URLACTION_DOWNLOAD_UNSIGNED_ACTIVEX:
        case URLACTION_FEATURE_DATA_BINDING:
        case URLACTION_FEATURE_FORCE_ADDR_AND_STATUS:
        case URLACTION_FEATURE_MIME_SNIFFING:
        case URLACTION_FEATURE_SCRIPT_STATUS_BAR:
        case URLACTION_FEATURE_WINDOW_RESTRICTIONS:
        case URLACTION_FEATURE_ZONE_ELEVATION:
        case URLACTION_HTML_FONT_DOWNLOAD:
        case URLACTION_HTML_INCLUDE_FILE_PATH:
        case URLACTION_HTML_JAVA_RUN:
        case URLACTION_HTML_META_REFRESH:
        case URLACTION_HTML_MIXED_CONTENT:
        case URLACTION_HTML_SUBFRAME_NAVIGATE:
        case URLACTION_HTML_SUBMIT_FORMS:
        case URLACTION_HTML_SUBMIT_FORMS_FROM:
        case URLACTION_HTML_SUBMIT_FORMS_TO:
        case URLACTION_HTML_USERDATA_SAVE:
        case URLACTION_LOOSE_XAML:
        case URLACTION_MANAGED_SIGNED:
        case URLACTION_MANAGED_UNSIGNED:
        case URLACTION_SCRIPT_JAVA_USE:
        case URLACTION_SCRIPT_PASTE:
        case URLACTION_SCRIPT_RUN:
        case URLACTION_SCRIPT_SAFE_ACTIVEX:
        case URLACTION_SHELL_EXECUTE_HIGHRISK:
        case URLACTION_SHELL_EXECUTE_LOWRISK:
        case URLACTION_SHELL_EXECUTE_MODRISK:
        case URLACTION_SHELL_FILE_DOWNLOAD:
        case URLACTION_SHELL_INSTALL_DTITEMS:
        case URLACTION_SHELL_MOVE_OR_COPY:
        case URLACTION_SHELL_VERB:
        case URLACTION_SHELL_WEBVIEW_VERB:
        case URLACTION_WINDOWS_BROWSER_APPLICATIONS:
        case URLACTION_WINFX_SETUP:
        case URLACTION_XPS_DOCUMENTS:
        case URLACTION_ALLOW_AUDIO_VIDEO: // ie9
        case URLACTION_ALLOW_STRUCTURED_STORAGE_SNIFFING: // ie9
        case URLACTION_ALLOW_XDOMAIN_SUBFRAME_RESIZE: // ie7
        case URLACTION_FEATURE_CROSSDOMAIN_FOCUS_CHANGE: // ie7
        case URLACTION_SHELL_ENHANCED_DRAGDROP_SECURITY:
        case URLACTION_SHELL_PREVIEW: // win7
        case URLACTION_SHELL_REMOTEQUERY: // win7
        case URLACTION_SHELL_RTF_OBJECTS_LOAD: // ie6sp2
        case URLACTION_SHELL_SECURE_DRAGSOURCE: // ie7
        // ie6sp2, value the same as URLACTION_SHELL_EXECUTE_HIGHRISK
        // case URLACTION_SHELL_SHELLEXECUTE: 
        // ie8, probably registry only
        case URLACTION_DOTNET_USERCONTROLS: 
            *pPolicy = URLPOLICY_ALLOW;
            return S_OK;

        case URLACTION_CHANNEL_SOFTDIST_PERMISSIONS:
            //*pPolicy = URLPOLICY_CHANNEL_SOFTDIST_AUTOINSTALL;
            *pPolicy = URLPOLICY_ALLOW;
            return S_OK;

        case URLACTION_JAVA_PERMISSIONS:
            //*pPolicy = URLPOLICY_JAVA_LOW;
            *pPolicy = URLPOLICY_ALLOW;
            return S_OK;

        case URLACTION_CREDENTIALS_USE:
            //*pPolicy = URLPOLICY_CREDENTIALS_SILENT_LOGON_OK;
            *pPolicy = URLPOLICY_ALLOW;
            return S_OK;

        case URLACTION_ALLOW_APEVALUATION: // Phishing filter.
        case URLACTION_LOWRIGHTS: // Vista Protected Mode.
        case URLACTION_SHELL_POPUPMGR:
        case URLACTION_SCRIPT_XSSFILTER:
        case URLACTION_ACTIVEX_TREATASUNTRUSTED:
        case URLACTION_ALLOW_ACTIVEX_FILTERING: // ie9
            *pPolicy = URLPOLICY_DISALLOW;
            return S_FALSE;
    }
    *pPolicy = 0;
    return INET_E_DEFAULT_ACTION;
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::QueryCustomPolicy(
        /* [in] */ LPCWSTR pwszUrl, 
        /* [in] */ REFGUID guidKey,
        /* [out] */ BYTE **ppPolicy,
        /* [out] */ DWORD *pcbPolicy,
        /* [in] */ BYTE *pContext, 
        /* [in] */ DWORD cbContext,
        /* [in] */ DWORD dwReserved) {
    // Gdy zwracamy INET_E_DEFAULT_ACTION w ProcessUrlAction() 
    // to moze byc uruchomione QueryCustomPolicy(). Gdy dalismy
    // *pPolicy = URLPOLICY_ALLOW dla wszystkich to ta funkcja
    // nigdy nie jest wywolywana, ale dla pewnosci zwrocmy i 
    // INET_E_DEFAULT_ACTION a nie "not implemented".
    *ppPolicy = 0;
    *pcbPolicy = 0;
    return INET_E_DEFAULT_ACTION;
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::SetZoneMapping(
        /* [in] */ DWORD dwZone, 
        /* [in] */ LPCWSTR lpszPattern, 
        /* [in] */ DWORD dwFlags) {
    return E_NOTIMPL;
}
HRESULT STDMETHODCALLTYPE InternetSecurityManager::GetZoneMappings(
        /* [in] */ DWORD dwZone, 
        /* [out] */ IEnumString **ppenumString, 
        /* [in] */ DWORD dwFlags) {
    *ppenumString = 0;
    return E_NOTIMPL;
}
