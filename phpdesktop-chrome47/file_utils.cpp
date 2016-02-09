// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "file_utils.h"
#include "defines.h"
#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>

#include "string_utils.h"
#include "executable.h"
#include "random.h"

std::string GetFileContents(std::string file) {
    std::ifstream inFile;
    inFile.open(Utf8ToWide(file).c_str(), std::ios::in);
    if (!inFile)
        return "";
    std::string contents;
    inFile.seekg(0, std::ios::end);
    contents.resize(static_cast<unsigned int>(inFile.tellg()));
    inFile.seekg(0, std::ios::beg);
    inFile.read(&contents[0], contents.size());
    inFile.close();
    return contents;
}
std::string GetRealPath(std::string path) {
    wchar_t realPath[MAX_PATH];
    GetFullPathName(Utf8ToWide(path).c_str(), _countof(realPath), realPath,
                    NULL);
    return WideToUtf8(realPath);
}
std::string GetAbsolutePath(std::string path) {
    if (path.length() && path.find(":") == std::string::npos) {
        path = GetExecutableDirectory() + "\\" + path;
        path = GetRealPath(path);
    }
    return path;
}
bool DirectoryExists(std::string directory) {
    DWORD dwAttrib = GetFileAttributes(Utf8ToWide(directory).c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
bool AnsiDirectoryExists(std::string directory) {
    // To check whether directory doesn't contain unicode characters.
    DWORD dwAttrib = GetFileAttributesA(directory.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
std::string GetDirectoryPath(const std::string& path)
{
    size_t pos = path.find_last_of("\\/");
    return (std::string::npos == pos) ? "" : path.substr(0, pos);
}
bool IsDirectoryWritable(std::string directory) {
    int rand = random(0, 1000000);
    std::string tempFile = directory.append("\\phpdesktop")\
            .append(IntToString(rand)).append(".tmp");
    HANDLE handle = CreateFile(Utf8ToWide(tempFile).c_str(), 
            GENERIC_WRITE,
            (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE),
            NULL, OPEN_ALWAYS, 
            (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE),
            NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    } else {
        CloseHandle(handle);
        return true;
    }    
}
