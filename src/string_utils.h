// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "defines.h"
#include <string>

void Utf8ToWide(const char* charString, wchar_t* wideString, int wideSize);
std::wstring Utf8ToWide(const char* utf8String);
std::wstring Utf8ToWide(const std::string& utf8String);
void WideToUtf8(const wchar_t* wideString, char* utf8String, int utf8Size);
std::string WideToUtf8(const wchar_t* wideString);
std::string WideToUtf8(const std::wstring& wideString);

std::string IntToString(long number);
std::string BoolToString(bool value);

std::string UpperString(std::string str);

std::string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace);
void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);
std::string TrimString(std::string s);
