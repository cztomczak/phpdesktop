// Copyright (c) 2024 Czarek Tomczak, PHP Desktop.

#include "include/cef_app.h"

class App : public CefApp,
            public CefBrowserProcessHandler
{
public:
    App();

    // CefApp.
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
    {
        return this;
    }
    virtual void OnBeforeCommandLineProcessing(
            const CefString& process_type,
            CefRefPtr<CefCommandLine> command_line) override;

    // CefBrowserProcessHandler.
    virtual void OnContextInitialized() override;

private:
    IMPLEMENT_REFCOUNTING(App);
};
