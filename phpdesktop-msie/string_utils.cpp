// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include <windows.h>

void Utf8ToWide(const char* charString, wchar_t* wideString, int wideSize) {
    int copiedChars = MultiByteToWideChar(CP_UTF8, 0, charString, -1, 
            wideString, wideSize);
}
void WideToUtf8(const wchar_t* wideString, char* utf8String, int utf8Size) {
    int copiedBytes = WideCharToMultiByte(CP_UTF8, 0, wideString, -1,
            utf8String, utf8Size, NULL, NULL);
    if (copiedBytes == 0)
        utf8String[0] = '\0';
}

