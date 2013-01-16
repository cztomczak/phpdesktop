// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#define CONCAT_NX(s1, s2) s1 ## s2
#define CONCAT(s1, s2) CONCAT_NX(s1, s2)

#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

#define ASSERT(expr, message)\
    if (!expr)\
        MessageBoxA(NULL, CONCAT(message,\
                CONCAT("\n\nFile: ",\
                    CONCAT(__FILE__,\
                        CONCAT("\nLine: ",\
                            CONCAT(STRINGIZE(__LINE__),\
                                CONCAT("\nFunc: ", __FUNCTION__)))))),\
        "Assertion failed", MB_ICONERROR);

#define ASSERT_EXIT(expr, message)\
    if (!expr) {\
        MessageBoxA(NULL, CONCAT(message,\
                CONCAT("\n\nFile: ",\
                    CONCAT(__FILE__,\
                        CONCAT("\nLine: ",\
                            CONCAT(STRINGIZE(__LINE__),\
                                CONCAT("\nFunc: ", __FUNCTION__)))))),\
        "Assertion failed - exiting", MB_ICONERROR);\
        exit(-1);\
    }

#define FATAL_ERROR(message)\
    {\
        MessageBoxA(NULL, CONCAT(message,\
                CONCAT("\n\nFile: ",\
                    CONCAT(__FILE__,\
                        CONCAT("\nLine: ",\
                            CONCAT(STRINGIZE(__LINE__),\
                                CONCAT("\nFunc: ", __FUNCTION__)))))),\
        "Fatal error", MB_ICONERROR);\
        exit(-1);\
    }

#define DEBUG_INT(A)\
    {\
        char tmpstr[4096];\
        sprintf_s(tmpstr, 4096, "%i\n\nFile: %s\nLine: %s\nFunc: %s()",\
                A, __FILE__, STRINGIZE(__LINE__), __FUNCTION__);\
        MessageBoxA(NULL, tmpstr, "DEBUG int", MB_ICONINFORMATION);\
    }

#define DEBUG_INT_4(A, B, C, D)\
    {\
        char tmpstr[4096];\
        sprintf_s(tmpstr, 4096,\
                "%i,%i,%i,%i\n\nFile: %s\nLine: %s\nFunc: %s()",\
                A, B, C, D, __FILE__, STRINGIZE(__LINE__), __FUNCTION__);\
        MessageBoxA(NULL, tmpstr, "DEBUG_INT_4", MB_ICONINFORMATION);\
    }

#define DEBUG_DWORD(A)\
    {\
        char tmpstr[4096];\
        sprintf_s(tmpstr, 4096, "%lu\n\nFile: %s\nLine: %s\nFunc: %s()",\
                A, __FILE__, STRINGIZE(__LINE__), __FUNCTION__);\
        MessageBoxA(NULL, tmpstr, "DEBUG_DWORD", MB_ICONINFORMATION);\
    }

#define DEBUG_CHAR(A)\
    {\
        char tmpstr[4096];\
        sprintf_s(tmpstr, 4096, "%s\n\nFile: %s\nLine: %s\nFunc: %s()",\
                A, __FILE__, STRINGIZE(__LINE__), __FUNCTION__);\
        MessageBoxA(NULL, tmpstr, "DEBUG", MB_ICONINFORMATION);\
    }

#define DEBUG_CHAR_2(A, B)\
    {\
        char tmpstr[4096];\
        sprintf_s(tmpstr, 4096, "%s\n%s\n\nFile: %s\nLine: %s\nFunc: %s()",\
                A, B, __FILE__, STRINGIZE(__LINE__), __FUNCTION__);\
        MessageBoxA(NULL, tmpstr, "DEBUG2", MB_ICONINFORMATION);\
    }

char* WCHAR_TO_CHAR(wchar_t* wide) {
	int asciisize = WideCharToMultiByte(CP_UTF8, 0, wide, -1, 0, 0, 0, 0);
	char* ascii = (char*)malloc(asciisize * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, wide, -1, ascii, asciisize, 0, 0);
	return ascii;
}

#define DEBUG_WCHAR(A)\
    {\
        char tmpstr[4096];\
        sprintf_s(tmpstr, 4096, "%s\n\nFile: %s\nLine: %s\nFunc: %s()",\
                WCHAR_TO_CHAR(A), __FILE__, STRINGIZE(__LINE__),\
                __FUNCTION__);\
        MessageBoxA(NULL, tmpstr, "DEBUG", MB_ICONINFORMATION);\
    }

#define DEBUG_WCHAR_2(A, B)\
    {\
        char tmpstr[4096];\
        sprintf_s(tmpstr, 4096, "%s\n%s\n\nFile: %s\nLine: %s\nFunc: %s()",\
                WCHAR_TO_CHAR(A), WCHAR_TO_CHAR(B), __FILE__,\
                STRINGIZE(__LINE__), __FUNCTION__);\
        MessageBoxA(NULL, tmpstr, "DEBUG", MB_ICONINFORMATION);\
    }

void GUID_TO_CHAR(const GUID* guid, char* outString, size_t outStringSize)
{
    sprintf_s(outString, outStringSize, 
              "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
              guid->Data1, guid->Data2, guid->Data3,
              guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3],
              guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
}
