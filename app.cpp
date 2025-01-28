// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include <stdlib.h>

#include "app.h"
#include "client.h"
#include "mongoose_server.h"
#include "settings.h"

#include "include/wrapper/cef_helpers.h"

void CreateMainBrowser()
{
    LOG(INFO) << "Create main browser";

    json_value* app_settings = Settings();
    CefBrowserSettings browser_settings;

    CefWindowInfo window_info;
    std::string runtime_style((*app_settings)["chrome"]["runtime_style"]);
    if (runtime_style == "alloy") {
        window_info.runtime_style = CEF_RUNTIME_STYLE_ALLOY;
        LOG(INFO) << "Runtime style: alloy";
    } else if (runtime_style == "chrome") {
        window_info.runtime_style = CEF_RUNTIME_STYLE_CHROME;
        LOG(INFO) << "Runtime style: chrome";
    } else {
        LOG(WARNING) << "Invalid runtime style in settings.json: " << runtime_style;
        window_info.runtime_style = CEF_RUNTIME_STYLE_ALLOY;
    }

    // Create window explicitilly TODO
    // std::string app_icon_path((*app_settings)["main_window"]["icon"]);
    // app_icon_path = GetFullPath(app_icon_path);
    // bool center_on_screen = (*app_settings)["main_window"]["center_on_screen"];
    // const char* title = (*Settings())["main_window"]["title"];

    int_fast64_t default_width = (*app_settings)["main_window"]["default_size"][0];
    int_fast64_t default_height = (*app_settings)["main_window"]["default_size"][1];
    CefRect browser_rect(0, 0, default_width, default_height);
    window_info.SetAsChild(nullptr, browser_rect);

    CefBrowserHost::CreateBrowser(
        window_info,
        Client::GetInstance(),
        mongoose_get_url(),
        browser_settings,
        nullptr,
        nullptr);
}

App::App()
{
}

// CefApp.

void App::OnBeforeCommandLineProcessing(const CefString& process_type,
                                        CefRefPtr<CefCommandLine> command_line)
{
    if (process_type.empty())
    {
        // Browser process type is an empty string
        if (getenv("PHPDESKTOP_ENABLE_LOGGING_STDERR")) {
            command_line->AppendSwitchWithValue("enable-logging", "stderr");
        }
        json_value* app_settings = Settings();
        json_value switches =(*app_settings)["chrome"]["command_line_switches"];
        if (switches.type == json_object) {
            int length = switches.u.object.length;
            for (int i = 0; i < length; i++) {
                std::string name = switches.u.object.values[i].name;
                std::string value = static_cast<const char*>(*switches.u.object.values[i].value);
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
                        std::string old_value = command_line->GetSwitchValue(name);
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

// CefBrowserProcessHandler.

void App::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();
    CreateMainBrowser();
}
