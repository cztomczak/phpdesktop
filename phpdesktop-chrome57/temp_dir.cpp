// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "temp_dir.h"
#include <Windows.h>
#include "string_utils.h"
#include "file_utils.h"
#include "log.h"
#include <vector>
#include "executable.h"

std::string GetAnsiTempDirectory() {
    // On Win7 GetTempPath returns path like this:
    // "C:\Users\%USER%\AppData\Local\Temp\"
    // Where %USER% may contain unicode characters and such
    // unicode path won't work with PHP.
    // --
    // On WinXP it returns a short path like this:
    // C:\DOCUME~1\UNICOD~1\LOCALS~1\Temp
    // PHP also doesn't like windows short paths. Sessions will
    // work fine, but other stuff like uploading files won't work.
    // --
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    // Whether temp is a short path.
    std::string tempPathString = WideToUtf8(tempPath);
    bool isShortPath = false;
    // We can't be 100% sure if "~1" is a pattern for short paths,
    // better check only "~".
    if (tempPathString.length() 
            && tempPathString.find("~") != std::string::npos) {
        isShortPath = true;
    }
    if (isShortPath || !AnsiDirectoryExists(WideToUtf8(tempPath))) {
        // This code will also run if the dir returned by
        // GetTempPathW was invalid.
        LOG_DEBUG << "The temp directory returned by OS contains "
                "unicode characters: " << WideToUtf8(tempPath).c_str();
        // Fallback 1: C:\\Windows\\Temp
        // -----------------------------
        TCHAR winDir[MAX_PATH];
        std::string winTemp;
        if (GetWindowsDirectory(winDir, MAX_PATH) == 0) {
            winTemp = "C:\\Windows\\Temp";
        } else {
            winTemp = WideToUtf8(winDir).append("\\Temp");
        }
        if (IsDirectoryWritable(winTemp)) {
            return winTemp;
        }
        // Fallback 2: C:\\Temp
        // --------------------
        if (IsDirectoryWritable("C:\\Temp")) {
            return "C:\\Temp";
        }
        // Fallback 3: ./temp
        // ------------------
        if (AnsiDirectoryExists(GetExecutableDirectory())) {
            // Application directory does not contain unicode characeters.
            std::string localTemp = GetExecutableDirectory().append("\\temp");
            if (DirectoryExists(localTemp) && IsDirectoryWritable(localTemp)) {
                // Local temp directory already exists and is writable.
                return localTemp;
            }
            if (CreateDirectoryA(localTemp.c_str(), NULL) != 0
                    && IsDirectoryWritable(localTemp)) {
                // CreateDirectoryA - If the function succeeds, the return 
                // value is nonzero.
                return localTemp;
            }
        }
        // Fallback 4: C:\\Users\\Public\\Temp
        // -----------------------------------
        std::string publicTemp = "C:\\Users\\Public\\Temp";
        if (DirectoryExists(publicTemp)
                && IsDirectoryWritable(publicTemp)) {
            return publicTemp;
        }
        if (CreateDirectoryA(publicTemp.c_str(), NULL) != 0
                && IsDirectoryWritable(publicTemp)) {
            // CreateDirectoryA - If the function succeeds, the return 
            // value is nonzero.
            return publicTemp;
        }
    }
    return WideToUtf8(tempPath);
}
