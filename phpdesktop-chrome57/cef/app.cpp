// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")

#include "app.h"
#include <string>
#include "util.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "app.h"
#include "../log.h"
#include "client_handler.h"
#include "../settings.h"
#include "javascript_api.h"

// ----------------------------------------------------------------------------
// CefRenderProcessHandler methods
// ----------------------------------------------------------------------------

///
// Called when a new message is received from a different process. Return true
// if the message was handled or false otherwise. Do not keep a reference to
// or attempt to access the message outside of this callback.
///
/*--cef()--*/
bool App::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                    CefProcessId source_process,
                                    CefRefPtr<CefProcessMessage> message) {
    LOG_DEBUG << "renderer[" << browser->GetIdentifier() << "] "
              << "OnProcessMessageReceived: " << message->GetName().ToString();
    if (message->GetName() == "SetIsFullscreen") {
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        bool isFullscreen = args->GetBool(0);
        CefRefPtr<JavascriptApi> javascriptApi = GetJavascriptApi(browser);
        if (javascriptApi.get()) {
            javascriptApi->SetIsFullscreen(isFullscreen);
        }
        return true;
    }
    LOG_ERROR << "Unhandled message in OnProcessMessageReceived";
    return false;
}

///
// Called immediately after the V8 context for a frame has been created. To
// retrieve the JavaScript 'window' object use the CefV8Context::GetGlobal()
// method. V8 handles can only be accessed from the thread on which they are
// created. A task runner for posting tasks on the associated thread can be
// retrieved via the CefV8Context::GetTaskRunner() method.
///
/*--cef()--*/
void App::OnContextCreated(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefV8Context> context) {
    // RENDERER PROCESS.
    LOG_DEBUG << "OnContextCreated()";
    CefRefPtr<CefV8Value> window = context->GetGlobal();
    CefRefPtr<CefV8Handler> handler = GetJavascriptApi(browser);
    if (!handler.get()) {
        LOG_ERROR << "GetJavascriptApi() failed in OnContextCreated()";
        return;
    }
    // Javascipt bindings.
    // The phpdesktop object.
    CefRefPtr<CefV8Value> phpdesktop = CefV8Value::CreateObject(NULL, NULL);
    window->SetValue("phpdesktop", phpdesktop, V8_PROPERTY_ATTRIBUTE_READONLY);
    // Methods.
    const char* methods[] = {
        "GetVersion",
        "ToggleFullscreen",
        "IsFullscreen",
        NULL
    };
    for (int i = 0; methods[i] != NULL; i++) {
        CefRefPtr<CefV8Value> method = CefV8Value::CreateFunction(
                methods[i], handler);
        phpdesktop->SetValue(method->GetFunctionName(), method,
                V8_PROPERTY_ATTRIBUTE_READONLY);
    }
}

///
// Called after a browser has been created. When browsing cross-origin a new
// browser will be created before the old browser with the same identifier is
// destroyed.
///
/*--cef()--*/
void App::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
    LOG_DEBUG << "OnBrowserCreated()";
    StoreJavascriptApi(browser, new JavascriptApi(browser));
}

///
// Called before a browser is destroyed.
///
/*--cef()--*/
void App::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
    LOG_DEBUG << "OnBrowserDestroyed()";
    RemoveJavascriptApi(browser);
}

// ----------------------------------------------------------------------------
// CefApp methods
// ----------------------------------------------------------------------------

///
// Provides an opportunity to view and/or modify command-line arguments before
// processing by CEF and Chromium. The |process_type| value will be empty for
// the browser process. Do not keep a reference to the CefCommandLine object
// passed to this method. The CefSettings.command_line_args_disabled value
// can be used to start with an empty command-line object. Any values
// specified in CefSettings that equate to command-line arguments will be set
// before this method is called. Be cautious when using this method to modify
// command-line arguments for non-browser processes as this may result in
// undefined behavior including crashes.
///
/*--cef(optional_param=process_type)--*/
void App::OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) {
    // May be called on any thread?
    if (process_type.empty()) {
        // Browser process.
        json_value* appSettings = GetApplicationSettings();
        json_value switches = (*appSettings)["chrome"]["command_line_switches"];
        if (switches.type == json_object) {
            int length = switches.u.object.length;
            for (int i = 0; i < length; i++) {
                std::string name = switches.u.object.values[i].name;
                std::string value = static_cast<const char*>(*switches.u.object.values[i].value);
                if (name.find("-") == 0) {
                    LOG_WARNING << "Invalid command line switch: " << name;
                    continue;
                }
                if (command_line->HasSwitch(name)) {
                    if (value.empty()) {
                        // Switch already set, do nothing.
                    } else {
                        std::string oldValue = command_line->GetSwitchValue(name);
                        if (oldValue != value) {
                            // Overwrite the switch with a new value.
                            command_line->AppendSwitchWithValue(name, value);
                        }
                    }
                } else {
                    if (value.empty()) {
                        command_line->AppendSwitch(name);
                    } else {
                        command_line->AppendSwitchWithValue(name, value);
                    }
                }
            }
        }
    }
    std::string process_name = "browser";
    if (!process_type.empty()) {
        process_name = process_type;
    }
    LOG_DEBUG << "Command line string for the " << process_name << " process: "
              << command_line->GetCommandLineString().ToString();
}

// ----------------------------------------------------------------------------
// CefBrowserProcessHandler methods
// ----------------------------------------------------------------------------

///
// Called on the browser process UI thread immediately after the CEF context
// has been initialized.
///
void App::OnContextInitialized() {
  REQUIRE_UI_THREAD();
  LOG_DEBUG << "App::OnContextInitialized()";
}

