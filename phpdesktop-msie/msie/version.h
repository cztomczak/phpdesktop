// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#pragma comment(lib, "Advapi32")

#include <Winreg.h>
#include <string>
#include <sstream>

// Parse() and LessThanVersion() taken from:
// http://stackoverflow.com/questions/2941491/compare-versions-as-strings

void Parse(int result[4], const std::string& input)
{
    std::istringstream parser(input);
    parser >> result[0];
    for(int idx = 1; idx < 4; idx++)
    {
        parser.get(); //Skip period
        parser >> result[idx];
    }
}

bool LessThanVersion(const std::string& a,const std::string& b)
{
    int parsedA[4], parsedB[4];
    Parse(parsedA, a);
    Parse(parsedB, b);
    return std::lexicographical_compare(parsedA, parsedA + 4, parsedB, parsedB + 4);
}

// GetStringRegKey() taken from here:
// http://stackoverflow.com/questions/34065/how-to-read-a-value-from-the-windows-registry

LONG GetStringRegKey(HKEY hKey, const std::string &strValueName,
             std::string &strValue, const std::string &strDefaultValue)
{
        strValue = strDefaultValue;
        char szBuffer[512];
        DWORD dwBufferSize = sizeof(szBuffer);
        ULONG nError;
        nError = RegQueryValueExA(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
        if (ERROR_SUCCESS == nError) {
            strValue = szBuffer;
        }
        return nError;
}

bool IsAtLeastIE60SP2()
{
    std::string registryVersion = "0.0.0.0";

    HKEY hKey;
    LONG lRes = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Internet Explorer", 0, KEY_READ, &hKey);
    if (lRes == ERROR_SUCCESS) {
        GetStringRegKey(hKey, "Version", registryVersion, "0.0.0.0");
        LOG(logINFO) << "IE version from registry: " << registryVersion.c_str();
    }
    RegCloseKey(hKey);

    // Version from registry must be valid, ohterwise for example
    // comparing "abc" will give true.
    // 6.00.2900.2180 == IE 6.0 SP2
    // 6.00.2800.1106 == IE 6.0 SP1
    if (LessThanVersion(registryVersion, "6.00.2900.2180")) {
        LOG(logERROR) << "IE 6.0 SP2 (6.00.2900.2180) is required for the"
                         "program to run correctly";
        return false;
    }
    return true;
}
