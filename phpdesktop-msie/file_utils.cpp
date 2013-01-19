// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include <windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "string_utils.h"

std::string GetFileContents(std::string file) {
    std::ifstream inFile;
    inFile.open(Utf8ToWide(file).c_str(), std::ios::in);
    if (!inFile)
        return "";
    std::string contents;
    inFile.seekg(0, std::ios::end);
    contents.resize(inFile.tellg());
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
