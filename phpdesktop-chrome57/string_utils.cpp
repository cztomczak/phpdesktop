// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>

void Utf8ToWide(const char* utf8String, wchar_t* wideString, int wideSize) {
    int copiedCharacters = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1,
            wideString, wideSize);
}
std::wstring Utf8ToWide(const char* utf8String) {
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, 0, 0);
    wchar_t* wideString = new wchar_t[requiredSize];
    int copiedCharacters = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1,
                                               wideString, requiredSize);
    std::wstring returnedString(wideString);
    delete[] wideString;
    wideString = 0;
    return returnedString;
}
std::wstring Utf8ToWide(const std::string& utf8String) {
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(), -1,
                                           0, 0);
    wchar_t* wideString = new wchar_t[requiredSize];
    int copiedCharacters = MultiByteToWideChar(CP_UTF8, 0, utf8String.c_str(),
            -1, wideString, requiredSize);
    std::wstring returnedString(wideString);
    delete[] wideString;
    wideString = 0;
    return returnedString;
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
    char* utf8String = new char[requiredSize];
    int copiedBytes = WideCharToMultiByte(CP_UTF8, 0, wideString, -1,
            utf8String, requiredSize, NULL, NULL);
    std::string returnedString(utf8String);
    delete[] utf8String;
    utf8String = 0;
    return returnedString;
}
std::string UpperString(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}
std::string WideToUtf8(const std::wstring& wideString) {
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1,
                                           0, 0, NULL, NULL);
    char* utf8String = new char[requiredSize];
    int copiedBytes = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1,
            utf8String, requiredSize, NULL, NULL);
    std::string returnedString(utf8String);
    delete[] utf8String;
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
std::string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}
void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}
static inline std::string &lTrimString(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}
static inline std::string &rTrimString(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}
std::string TrimString(std::string s) {
    return lTrimString(rTrimString(s));
}
