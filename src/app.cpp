// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "app.h"
#include "include/wrapper/cef_helpers.h"
#include "print_handler_gtk.h"
#include "settings.h"

App::App() {
}

// CefApp

void App::OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) {
    // command_line_switches option
    if (process_type.empty()) {
        // Browser process type is an empty string
        json_value* app_settings = get_app_settings();
        json_value switches =
                (*app_settings)["chrome"]["command_line_switches"];
        if (switches.type == json_object) {
            int length = switches.u.object.length;
            for (int i = 0; i < length; i++) {
                std::string name = switches.u.object.values[i].name;
                std::string value = static_cast<const char*>(
                        *switches.u.object.values[i].value);
                if (name.find("-") == 0) {
                    LOG(WARNING) << "Invalid command line switch: " << name;
                    continue;
                }
                if (value.empty()) {
                    LOG(INFO) << "Processing switch: " << name;
                } else {
                    LOG(INFO) << "Processing switch: " << name << "=" << value;
                }
                if (command_line->HasSwitch(name)) {
                    if (value.empty()) {
                        // Switch already set, do nothing
                    } else {
                        std::string old_value =
                                command_line->GetSwitchValue(name);
                        if (old_value != value) {
                            // Overwrite the switch with a new value
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
}

// CefBrowserProcessHandler

void App::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
    print_handler_ = new ClientPrintHandlerGtk();
}

CefRefPtr<CefPrintHandler> App::GetPrintHandler() {
    return print_handler_;
}
