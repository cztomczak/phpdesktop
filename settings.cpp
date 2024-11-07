// Copyright (c) 2024 Czarek Tomczak, PHP Desktop.

#include "json.h"
#include "utils.h"
#include "include/base/cef_logging.h"

std::string g_SettingsError = "";

json_value* Settings()
{
    static json_value* ret = new json_value();
    static bool settings_fetched = false;
    if (settings_fetched) {
        return ret;
    }
    settings_fetched = true;
    LOG(INFO) << "Fetching settings from settings.json file";

    std::string settings_file;
    GetResourcesDir(settings_file);
    settings_file.append("/settings.json");
    std::string contents = GetFileContents(settings_file);
    if (contents.empty()) {
        g_SettingsError = "Error while reading settings.json file";
        LOG(ERROR) << g_SettingsError.c_str();
        return ret;
    }

    json_settings settings;
    memset(&settings, 0, sizeof(json_settings));
    char error[256];
    json_value* json_parsed = json_parse_ex(&settings, contents.c_str(),
                                            contents.length(), &error[0]);
    if (json_parsed == 0) {
        g_SettingsError = "Error while parsing settings.json file";
        LOG(ERROR) << g_SettingsError << ": " << error;
        return ret;
    }
    ret = json_parsed;
    return ret;
}

std::string SettingsError()
{
    return g_SettingsError;
}
