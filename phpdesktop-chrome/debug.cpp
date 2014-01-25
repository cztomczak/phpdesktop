// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include <stdio.h>

char* WCHAR_TO_CHAR(wchar_t* wide) {
	int asciisize = WideCharToMultiByte(CP_UTF8, 0, wide, -1, 0, 0, 0, 0);
	char* ascii = (char*)malloc(asciisize * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, wide, -1, ascii, asciisize, 0, 0);
	return ascii;
}
void GUID_TO_CHAR(const GUID* guid, char* outString, size_t outStringSize)
{
    sprintf_s(outString, outStringSize,
              "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
              guid->Data1, guid->Data2, guid->Data3,
              guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
              guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
}
