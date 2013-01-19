// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include <windows.h>
#include <string>
#include <sstream>

void Utf8ToWide(const char* utf8String, wchar_t* wideString, int wideSize) {
    int copiedCharacters = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, 
            wideString, wideSize);
}
std::wstring Utf8ToWide(const char* utf8String) {
    std::wstring wideString;
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, 0, 0);
    wideString.resize(requiredSize);
    int copiedCharacters = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1,
            &wideString[0], wideString.capacity());
    return wideString;
}
std::wstring Utf8ToWide(const std::string utf8String) {
    std::wstring wideString;
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], -1, 
                                           0, 0);
    wideString.resize(requiredSize);
    int copiedCharacters = MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], -1,
            &wideString[0], wideString.capacity());
    return wideString;
}
void WideToUtf8(const wchar_t* wideString, char* utf8String, int utf8Size) {
    int copiedBytes = WideCharToMultiByte(CP_UTF8, 0, wideString, -1,
            utf8String, utf8Size, NULL, NULL);
    if (copiedBytes == 0)
        utf8String[0] = 0;
}
std::string WideToUtf8(const wchar_t* wideString) {
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideString, -1,
                                           0, 0, NULL, NULL);
    char* utf8String = (char*)malloc(requiredSize);
    int copiedBytes = WideCharToMultiByte(CP_UTF8, 0, wideString, -1,
            utf8String, requiredSize, NULL, NULL);
    std::string returnedString(utf8String);
    free(utf8String);
    utf8String = 0;
    return returnedString;
}
std::string WideToUtf8(const std::wstring wideString) {    
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1,
                                           0, 0, NULL, NULL);
    char* utf8String = (char*)malloc(requiredSize);
    int copiedBytes = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1,
            utf8String, requiredSize, NULL, NULL);
    std::string returnedString(utf8String);
    free(utf8String);
    utf8String = 0;
    return returnedString;
}
std::string IntToString(long number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}
std::string BoolToString(bool value) {
    if (value)
        return "true";
    else
        return "false";
}
