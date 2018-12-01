// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "include/cef_app.h"

// Implement application-level callbacks for the browser process
class App : public CefApp,
            public CefBrowserProcessHandler
{
public:
    App();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
            override {
        return this;
    }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(App);
};
