// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "dpi_aware.h"
#include "settings.h"
#include "log.h"

// Note that VC project file includes DeclareDPIAware.manifest,
// so application is always set to be DPI aware even if "dpi_aware"
// is set to false in settings.json.

void SetBrowserDpiSettings(CefRefPtr<CefBrowser> cefBrowser) {
    // Chrome 39 handles High DPI just fine. Looks like it uses
    // zooming and not native OS display scaling that would make
    // fonts look fuzzy. Commenting out the old code from Chrome 31.
    // We still need to handle window size when dpi_aware=true
    // in settings.json. So GetDpiAwareWindowSize() is still required.
    return;

    /*
    // Setting zoom level immediately after browser was created
    // won't work. We need to wait a moment before we can set it.
    REQUIRE_UI_THREAD();

    json_value* appSettings = GetApplicationSettings();
    if (!(*appSettings)["application"]["dpi_aware"]) {
        return;
    }

    double oldZoomLevel = cefBrowser->GetHost()->GetZoomLevel();
    double newZoomLevel = 0.0;

    // Win7:
    // text size Larger 150% => ppix/ppiy 144
    // text size Medium 125% => ppix/ppiy 120
    // text size Smaller 100% => ppix/ppiy 96
    HWND cefHandle = cefBrowser->GetHost()->GetWindowHandle();
    HDC hdc = GetDC(cefHandle);
    int ppix = GetDeviceCaps(hdc, LOGPIXELSX);
    int ppiy = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(cefHandle, hdc);

    if (ppix > 96) {
        newZoomLevel = (ppix - 96) / 24;
    }

    if (oldZoomLevel != newZoomLevel) {
        cefBrowser->GetHost()->SetZoomLevel(newZoomLevel);
        if (cefBrowser->GetHost()->GetZoomLevel() != oldZoomLevel) {
            // Succes.
            LOG_INFO << "DPI, ppix = " << ppix << ", ppiy = " << ppiy;
            LOG_INFO << "DPI, browser zoom level = "
                      << cefBrowser->GetHost()->GetZoomLevel();
        }
    } else {
        // This code block running can also be a result of executing
        // SetZoomLevel(), as GetZoomLevel() didn't return the new
        // value that was set. Documentation says that if SetZoomLevel
        // is called on the UI thread, then GetZoomLevel should
        // immediately return the same value that was set. Unfortunately
        // this seems not to be true.
        static bool already_logged = false;
        if (!already_logged) {
            already_logged = true;
            // Success.
            LOG_INFO << "DPI, ppix = " << ppix << ", ppiy = " << ppiy;
            LOG_INFO << "DPI, browser zoom level = "
                      << cefBrowser->GetHost()->GetZoomLevel();
        }
    }
    // We need to check zooming constantly, during loading of pages.
    // If we set zooming to 2.0 for localhost/ and then it navigates
    // to google.com, then the zomming is back at 0.0 and needs to
    // be set again.
    CefPostDelayedTask(
            TID_UI,
            NewCefRunnableFunction(&SetBrowserDpiSettings, cefBrowser),
            50);
    */
}

void GetDpiAwareWindowSize(int* width, int* height) {
    // Win7 DPI:
    // text size Larger 150% => ppix/ppiy 144
    // text size Medium 125% => ppix/ppiy 120
    // text size Smaller 100% => ppix/ppiy 96
    HDC hdc = GetDC(HWND_DESKTOP);
    int ppix = GetDeviceCaps(hdc, LOGPIXELSX);
    int ppiy = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(HWND_DESKTOP, hdc);
    double newZoomLevel = 0.0;
    if (ppix > 96) {
        newZoomLevel = (ppix - 96) / 24;
    }
    if (newZoomLevel > 0.0) {
        *width = *width + (int)ceil(newZoomLevel * 0.25 * (*width));
        *height = *height + (int)ceil(newZoomLevel * 0.25 * (*height));
        LOG_INFO << "DPI, window enlarged by "
                  << ceil(newZoomLevel * 0.25 * 100) << "%"
                  << " new width/height = " << *width << "/" << *height;
    }
}
