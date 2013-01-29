// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <windows.h>
#include <MsHtmHst.h>

#ifndef DOCHOSTUIFLAG_DPI_AWARE
 // Available since Internet Explorer 8.
 #define DOCHOSTUIFLAG_DPI_AWARE 0x40000000
#endif

#include "browser_frame_interface.h"
#include "external_dispatch.h"
#include "../settings.h"
#include "virtual_keys.h"

template <class TopFrame>
class DocHostUiHandler : public IDocHostUIHandler {
public:
    BrowserFrameInterface<TopFrame>* browserFrame_;
    IDispatch* externalDispatch_;

    DocHostUiHandler(BrowserFrameInterface<TopFrame>* inBrowserFrame,
                     IDispatch* inExternalDispatch)
            : browserFrame_(inBrowserFrame),
            externalDispatch_(inExternalDispatch) {
    }
    HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObject) {
        return browserFrame_->GetOleClientSite()->QueryInterface(riid, 
                                                                 ppvObject);
    }
    ULONG STDMETHODCALLTYPE AddRef( void) {
        return 1;
    }
    ULONG STDMETHODCALLTYPE Release( void) {
        return 1;
    }
    HRESULT STDMETHODCALLTYPE ShowContextMenu( 
            /* [in] */ DWORD dwID,
            /* [in] */ POINT *ppt,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch *pdispReserved) {
        // If desired, we can pop up our own custom context menu here.
        
        // Do not show context menus by default with S_OK.
        HRESULT hr(S_OK);

        if (dwID == CONTEXT_MENU_TEXTSELECT) {
            // If text select, then return S_FALSE to show menu.
            hr = S_FALSE;
        } else {
            // Notify a frame window that the user clicked the
            // right mouse button in the window.
            POINT pt;
    	    GetCursorPos(&pt);
            PostMessage(browserFrame_->GetWindowHandle(), WM_CONTEXTMENU, 
                        pt.x, pt.y);
        }

        return hr;
    }    
    HRESULT STDMETHODCALLTYPE GetHostInfo( 
            /* [out][in] */ DOCHOSTUIINFO *pInfo) {
        json_value* settings = GetApplicationSettings();
        bool use_themes = (*settings)["msie"]["use_themes"];
        bool windowed_select_control = 
                (*settings)["msie"]["windowed_select_control"];
        bool disable_scrollbars = (*settings)["msie"]["disable_scrollbars"];
        bool flat_scrollbars = (*settings)["msie"]["flat_scrollbars"];
        bool utf8_url_encoding = (*settings)["msie"]["utf8_url_encoding"];
        bool autocomplete_forms = (*settings)["msie"]["autocomplete_forms"];
        bool dpi_aware = (*settings)["msie"]["dpi_aware"];

        pInfo->cbSize = sizeof(DOCHOSTUIINFO);
        pInfo->dwFlags = DOCHOSTUIFLAG_DISABLE_HELP_MENU
                | DOCHOSTUIFLAG_NO3DBORDER;

        if (use_themes)
            pInfo->dwFlags |= DOCHOSTUIFLAG_THEME;
        else
            pInfo->dwFlags |= DOCHOSTUIFLAG_NOTHEME;

        if (windowed_select_control)
            pInfo->dwFlags |= DOCHOSTUIFLAG_USE_WINDOWED_SELECTCONTROL;
        else
            pInfo->dwFlags |= DOCHOSTUIFLAG_USE_WINDOWLESS_SELECTCONTROL;

        if (disable_scrollbars)
            pInfo->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;
        
        if (flat_scrollbars)
            pInfo->dwFlags |= DOCHOSTUIFLAG_FLAT_SCROLLBAR;

        if (utf8_url_encoding)
            pInfo->dwFlags |= DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8;
        else
            pInfo->dwFlags |= DOCHOSTUIFLAG_URL_ENCODING_DISABLE_UTF8;

        if (autocomplete_forms)
            pInfo->dwFlags |= DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE;
        
        if (dpi_aware)
            pInfo->dwFlags |= DOCHOSTUIFLAG_DPI_AWARE;

        pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
        return S_OK;
    }    
    HRESULT STDMETHODCALLTYPE ShowUI( 
            /* [in] */ DWORD dwID,
            /* [in] */ IOleInPlaceActiveObject *pActiveObject,
            /* [in] */ IOleCommandTarget *pCommandTarget,
            /* [in] */ IOleInPlaceFrame *pFrame,
            /* [in] */ IOleInPlaceUIWindow *pDoc) {
        // Enables the host to replace MSHTML menus and toolbars.
        return S_OK;
    }    
    HRESULT STDMETHODCALLTYPE HideUI( void) {
        // Enables the host to remove its menus and toolbars.
        return S_OK;
    }    
    HRESULT STDMETHODCALLTYPE UpdateUI( void) {
        // Notifies the host that the command state has changed.
        return S_OK;
    }    
    HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) {
        // Called by the MSHTML implementation of 
        // IOleInPlaceActiveObject::EnableModeless. 
        // Also called when MSHTML displays a modal UI.
        return S_OK;
    }    
    HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
            /* [in] */ BOOL fActivate) {
        // Notifies the active in-place object when the container's
        // document window is activated or deactivated.
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( 
            /* [in] */ BOOL fActivate) {
        // Notifies the object when the container's top-level
        // frame window is activated or deactivated.
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE ResizeBorder( 
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IOleInPlaceUIWindow *pUIWindow,
            /* [in] */ BOOL fRameWindow) {
        // Alerts the object that it needs to resize its border 
        // space.
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ LPMSG lpMsg,
            /* [in] */ const GUID *pguidCmdGroup,
            /* [in] */ DWORD nCmdID) {
        // Passes a keystroke to the control site for processing.
        // Return S_OK if message was handled, S_FALSE for the
        // default behavior.
        
        json_value* settings = GetApplicationSettings();
        HRESULT hr = S_FALSE;

        if (lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_BACK) {
            // Allow backspace in text controls. Inputs that are
            // certain to be text: text, password, search, email,
            // tel, url, number, time.
            wchar_t tag[50];
            wchar_t typeAttr[50];
            if (browserFrame_->GetActiveHtmlElement(tag, _countof(tag),
                                            typeAttr, _countof(typeAttr))) {
                _wcsupr_s(tag, _countof(tag));
                _wcsupr_s(typeAttr, _countof(typeAttr));
                if (wcscmp(tag, L"INPUT") == 0 && 
                        (  wcscmp(typeAttr, L"TEXT") == 0
                        || wcscmp(typeAttr, L"PASSWORD") == 0
                        || wcscmp(typeAttr, L"SEARCH") == 0
                        || wcscmp(typeAttr, L"EMAIL") == 0
                        || wcscmp(typeAttr, L"TEL") == 0
                        || wcscmp(typeAttr, L"URL") == 0
                        || wcscmp(typeAttr, L"NUMBER") == 0
                        || wcscmp(typeAttr, L"TIME") == 0 )) {
                    hr = S_FALSE;
                } else if (wcscmp(tag, L"TEXTAREA") == 0) {
                    hr = S_FALSE;
                } else {
                    // Backspace outside of text control acts as
                    // "history back" action in browser, disabling it.
                    hr = S_OK;
                }
            }
        }

        if (lpMsg->wParam == VK_F5) {
            bool enable_f5_refresh = (*settings)["msie"]["enable_f5_refresh"];
            if (enable_f5_refresh)
                hr = S_FALSE;
        }

        if (lpMsg->message == WM_KEYDOWN && 
                (  lpMsg->wParam == VK_BROWSER_BACK 
                || lpMsg->wParam == VK_BROWSER_FAVORITES 
                || lpMsg->wParam == VK_BROWSER_FORWARD 
                || lpMsg->wParam == VK_BROWSER_HOME
                || lpMsg->wParam == VK_BROWSER_REFRESH 
                || lpMsg->wParam == VK_BROWSER_SEARCH 
                || lpMsg->wParam == VK_BROWSER_STOP 
                || lpMsg->wParam == VK_F1
                || lpMsg->wParam == VK_HELP 
                || lpMsg->wParam == VK_MENU 
                || lpMsg->wParam == VK_PRINT
                || lpMsg->wParam == VK_RMENU
                || lpMsg->wParam == VK_LMENU 
                || lpMsg->wParam == VK_ZOOM)) {
            hr = S_OK;
        }

        if (lpMsg->message == WM_SYSKEYDOWN &&
                (lpMsg->wParam == VK_LEFT || lpMsg->wParam == VK_RIGHT)) {
            // WM_SYSKEYDOWN = F10 or ALT is pressed.
            // Blocks: alt+left, alt+right - history backward
            // and forward navigation.
            hr = S_OK;
        }

        if (lpMsg->message == WM_KEYDOWN &&
                (lpMsg->wParam == VK_P || lpMsg->wParam == VK_O 
                || lpMsg->wParam == VK_L || lpMsg->wParam == VK_N) 
                && (GetKeyState(VK_CONTROL) < 0)) {
            // ctrl+p = print, NO
            // ctrl+o = open, NO
            // ctrl+l = open, NO
            // ctrl+n = new window, NO
            hr = S_OK;
        }

        // Let these shortcuts work:
        // ctrl+f Find, YES
        // ctrl+c Copy, YES
        // ctrl+x Cut, YES
        // ctrl+v Paste, YES
        // ctrl+z Undo, YES
        // ctrl+a Select All, YES

        return hr;
    }
    HRESULT STDMETHODCALLTYPE GetOptionKeyPath( 
            /* [out] */ LPOLESTR *pchKey,
            /* [in] */ DWORD dw) {
        // Gets a registry subkey path that overrides the
        // default Windows Internet Explorer registry settings.
        
        // We are overriding it to prevent the browser from 
        // using its default settings in the registry.
        
        // While googling I've encountered users saying that
        // Internet Explorer 9 is not calling this method anymore.
        // The solution seems to be to hook to registry api calls,
        // but this might raise an alarm by antivirus software.
        // See this StackOverflow answer:
        // http://stackoverflow.com/a/8918293/623622

        /* Should we create this key?
           RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\MyCompany\\MyApp",
               NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE,
               NULL, &hKey, &dwDisposition);
        */

        // TODO: disable script debugging messages?
        // http://stackoverflow.com/q/2936279/623622
        // Create registry key "Main\\Disable Script Debugger" and
        // set to "yes".

        if (!pchKey)
		    return E_INVALIDARG;
        // This registry path does not need to exist, it should
        // be empty.
        wchar_t registryPath[200] =
                L"Software\\PHP Desktop\\WebBrowser2 settings";
        LOG(logDEBUG) << "DocHostUiHandler::GetOptionKeyPath(): "
                         "registry path: " << registryPath;
        int registrySize = wcslen(registryPath) + 1;
        int memorySize = registrySize * sizeof(registryPath[0]);
        *pchKey = reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(memorySize));
        if (*pchKey) {
            wcscpy_s(*pchKey, registrySize, registryPath);
            return S_OK;
        } else {
            return E_OUTOFMEMORY;
        }
    }
    HRESULT STDMETHODCALLTYPE GetDropTarget( 
            /* [in] */ IDropTarget *pDropTarget,
            /* [out] */ IDropTarget **ppDropTarget) {
        if (pDropTarget == 0)
            return E_INVALIDARG;
        *ppDropTarget = 0;
        return E_NOTIMPL;
    }
    HRESULT STDMETHODCALLTYPE GetExternal( 
            /* [out] */ IDispatch **ppDispatch) {
        *ppDispatch = externalDispatch_;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE TranslateUrl( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ OLECHAR *pchURLIn,
            /* [out] */ OLECHAR **ppchURLOut) {
        *ppchURLOut = 0;
        return S_FALSE;
    }
    HRESULT STDMETHODCALLTYPE FilterDataObject( 
            /* [in] */ IDataObject *pDO,
            /* [out] */ IDataObject **ppDORet) {
        *ppDORet = 0;
        return S_FALSE;
    }
};
