// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "executable.h"
#include "file_utils.h"
#include "json.h"
#include "log.h"

json_value* GetApplicationSettings() {
    static json_value* ret = new json_value();
    static bool settings_fetched = false;
    if (settings_fetched)
        return ret;
    settings_fetched = true;
    LOG(logDEBUG) << "Fetching settings from settings.json file";

    std::string settingsFile = GetExecutableDirectory() + "\\settings.json";
    std::string contents = GetFileContents(settingsFile);
    if (contents.empty()) {
        LOG(logWARNING) << "Error opening settings.json file";
        return ret;
    }

    json_settings settings;
    memset(&settings, 0, sizeof(json_settings));
    char error[256];
    json_value* json_parsed = json_parse_ex(&settings, contents.c_str(), 
                                            &error[0]);
    if (json_parsed == 0) {
        LOG(logWARNING) << "Error while parsing settings.json file: " << error;
        return ret;
    }
    ret = json_parsed;
    return ret;
}
