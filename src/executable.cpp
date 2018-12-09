// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include <wchar.h>
#include "string_utils.h"

std::string GetExecutablePath() {
    wchar_t path[MAX_PATH];
    std::wstring wideString;
    if (GetModuleFileName(NULL, path, _countof(path))) {
        if (GetLongPathName(path, path, _countof(path)))
            wideString = path;
    }
    return WideToUtf8(wideString);
}
std::string GetExecutablePathQuoted() {
    std::string path = GetExecutablePath();
    return path.insert(0, "\"").append("\"");
}
std::string GetExecutableDirectory() {
    std::string path = GetExecutablePath();
    wchar_t drive[_MAX_DRIVE];
    wchar_t directory[_MAX_DIR];
    wchar_t filename[_MAX_FNAME];
    wchar_t extension[_MAX_EXT];
    errno_t result = _wsplitpath_s(Utf8ToWide(path).c_str(),
            drive, _countof(drive),
            directory, _countof(directory),
            filename, _countof(filename),
            extension, _countof(extension));
    char utf8Drive[_MAX_DRIVE * 2];
    WideToUtf8(drive, utf8Drive, _countof(utf8Drive));
    char utf8Directory[_MAX_DIR * 2];
    WideToUtf8(directory, utf8Directory, _countof(utf8Directory));
    path.assign(utf8Drive).append(utf8Directory);
    if (path.length()) {
        char lastCharacter = path[path.length() - 1];
        if (lastCharacter == '\\' || lastCharacter == '/')
            path.erase(path.length() - 1);
    }
    return path;
}
std::string GetExecutableFilename() {
    std::string path = GetExecutablePath();
    wchar_t drive[_MAX_DRIVE];
    wchar_t directory[_MAX_DIR];
    wchar_t filename[_MAX_FNAME];
    wchar_t extension[_MAX_EXT];
    errno_t result = _wsplitpath_s(Utf8ToWide(path).c_str(),
            drive, _countof(drive),
            directory, _countof(directory),
            filename, _countof(filename),
            extension, _countof(extension));
    if (result != 0)
        return "";
    char utf8Filename[_MAX_FNAME * 2];
    WideToUtf8(filename, utf8Filename, _countof(utf8Filename));
    char utf8Extension[_MAX_EXT * 2];
    WideToUtf8(extension, utf8Extension, _countof(utf8Extension));
    return path.assign(utf8Filename).append(utf8Extension);
}
std::string GetExecutableName() {
    std::string path = GetExecutablePath();
    wchar_t drive[_MAX_DRIVE];
    wchar_t directory[_MAX_DIR];
    wchar_t filename[_MAX_FNAME];
    wchar_t extension[_MAX_EXT];
    errno_t result = _wsplitpath_s(Utf8ToWide(path).c_str(),
            drive, _countof(drive),
            directory, _countof(directory),
            filename, _countof(filename),
            extension, _countof(extension));
    char utf8Filename[_MAX_FNAME * 2];
    WideToUtf8(filename, utf8Filename, _countof(utf8Filename));
    return path.assign(utf8Filename);
}
