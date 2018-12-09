// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <Windows.h>
#include <Winreg.h>
#include "log.h"
#include "string_utils.h"

bool CreateRegistryKey(HKEY hKeyRoot, const wchar_t* path) {
    HKEY hKey;
    DWORD dwDisposition;
    LONG result = RegCreateKeyEx(hKeyRoot, path,
            0, 0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE,
            0, &hKey, &dwDisposition);
    bool ret = false;
    if (result == ERROR_SUCCESS) {
        ret = true;
    } else {
        LOG_WARNING << "CreateRegistryKey() failed: "
                "RegCreateKeyEx() failed: path = "  << WideToUtf8(path);
        _ASSERT(false);
    }
    if (hKey)
        RegCloseKey(hKey);
    return ret;
}
bool CreateRegistryString(HKEY hKeyRoot, const wchar_t* path,
                                 const wchar_t* stringName,
                                 const wchar_t* stringValue) {
    HKEY hKey;
    DWORD dwDisposition;
    LONG result = RegCreateKeyEx(hKeyRoot, path,
            0, 0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE,
            0, &hKey, &dwDisposition);
    bool ret = false;
    if (result == ERROR_SUCCESS) {
        DWORD bufferSize = (wcslen(stringValue) + 1) * sizeof(wchar_t);
        result = RegSetValueEx(hKey, stringName, 0, REG_SZ,
                (LPBYTE)stringValue, bufferSize);
        if (result == ERROR_SUCCESS) {
            ret = true;
        } else {
            LOG_WARNING << "CreateRegistryString() failed: "
                    "RegSetValueEx() failed: "
                    "stringName = " << WideToUtf8(stringName) <<
                    "stringValue = " << WideToUtf8(stringValue);
            _ASSERT(false);
        }
    } else {
        LOG_WARNING << "CreateRegistryString() failed: "
                "RegCreateKeyEx() failed: path = "  << WideToUtf8(path);
        _ASSERT(false);
    }
    if (hKey)
        RegCloseKey(hKey);
    return true;
}
