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
