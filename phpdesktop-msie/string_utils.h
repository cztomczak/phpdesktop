// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <string>

void Utf8ToWide(const char* charString, wchar_t* wideString, int wideSize);
std::wstring Utf8ToWide(const char* utf8String);
std::wstring Utf8ToWide(const std::string& utf8String);
void WideToUtf8(const wchar_t* wideString, char* utf8String, int utf8Size);
std::string WideToUtf8(const wchar_t* wideString);
std::string WideToUtf8(const std::wstring& wideString);
std::string IntToString(long number);
std::string BoolToString(bool value);
