// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include <string>

#include "executable.h"
#include "settings.h"
#include "string_utils.h"

void FatalError(HWND hwnd, std::string message) {
    json_value* settings = GetApplicationSettings();
    std::string title = (*settings)["main_window"]["title"];
    if (title.empty())
        title = GetExecutableName();
    MessageBox(hwnd, Utf8ToWide(message).c_str(), Utf8ToWide(title).c_str(),
               MB_ICONERROR);
    exit(-1);
}
