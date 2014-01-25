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

App::App() {
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
                std::string value = (*switches.u.object.values[i].value);
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
