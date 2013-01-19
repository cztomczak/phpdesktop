// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include <iostream>
#include <fstream>

#include "executable.h"
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

    std::ifstream inFile;
    inFile.open(Utf8ToWide(settingsFile).c_str(), std::ios::in);
    if (!inFile) {
        LOG(logWARNING) << "Error while opening settings.json file";
        return ret;
    }
    std::string json_data;
    inFile.seekg(0, std::ios::end);
    json_data.resize(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    inFile.read(&json_data[0], json_data.size());
    inFile.close();

    json_settings settings;
    memset(&settings, 0, sizeof(json_settings));
    char error[256];
    json_value* json_parsed = json_parse_ex(&settings, json_data.c_str(), 
                                            &error[0]);
    if (json_parsed == 0) {
        LOG(logWARNING) << "Error while parsing settings.json file: " << error;
        return ret;
    }
    ret = json_parsed;
    return ret;
}
