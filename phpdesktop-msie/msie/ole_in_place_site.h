// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "../defines.h"
#include <OleIdl.h>
class BrowserWindow;
class OleInPlaceFrame;

class OleInPlaceSite : public IOleInPlaceSite {
private:
    BrowserWindow* browserWindow_;
    IOleInPlaceFrame* oleInPlaceFrame_;
public:
    OleInPlaceSite(BrowserWindow* inBrowserWindow,
                   IOleInPlaceFrame* inOleInPlaceFrame);
    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);
    // IOleWindow
    HRESULT STDMETHODCALLTYPE GetWindow(HWND FAR* lphwnd);
    HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
    // IOleInPlaceSite
    HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void);
    HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void);
    HRESULT STDMETHODCALLTYPE OnUIActivate(void);
    HRESULT STDMETHODCALLTYPE GetWindowContext(
            /* [out] */ IOleInPlaceFrame **ppFrame,
            /* [out] */ IOleInPlaceUIWindow **ppDoc,
            /* [out] */ LPRECT lprcPosRect,
            /* [out] */ LPRECT lprcClipRect,
            /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo);
    HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtent);
    HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable);
    HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void);
    HRESULT STDMETHODCALLTYPE DiscardUndoState(void);
    HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void);
    HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect);
};
