// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "version.h"
#include <Windows.h>
#include "log.h"
#include <assert.h>
#include "string_utils.h"

#pragma comment(lib, "Version")

std::string GetPhpDesktopVersion() {
    TCHAR versionFile[MAX_PATH];
    if (!GetModuleFileName(NULL, versionFile, MAX_PATH))
        return "";
    DWORD versionHandle = NULL;
    DWORD versionSize = GetFileVersionInfoSize(versionFile, &versionHandle);
    if (versionSize == NULL)
        return "";
    LPSTR versionData = new char[versionSize];
    if (GetFileVersionInfo(versionFile, versionHandle, versionSize, versionData)) {
        UINT querySize = 0;
        LPBYTE queryBuffer = NULL;
        if (VerQueryValue(versionData, L"\\", (VOID**)&queryBuffer, &querySize)) {
            if (querySize) {
                VS_FIXEDFILEINFO *versionInfo = (VS_FIXEDFILEINFO*)queryBuffer;
                if (versionInfo->dwSignature == 0xfeef04bd)
                {
                    int major = HIWORD(versionInfo->dwFileVersionMS);
                    int minor = LOWORD(versionInfo->dwFileVersionMS);
                    int build = versionInfo->dwFileVersionLS;
                    delete[] versionData;
                    return IntToString(major).append(".").append(IntToString(minor));
                }
            }
        }
    }
    delete[] versionData;
    return "";
}