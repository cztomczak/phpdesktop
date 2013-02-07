// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// This application is for running php-desktop-msie.exe
// over and over again until it freezes. Trying to debug
// mongoose web-server freezing problem. 

// The index.php sets timeout for 200ms that redirects to 
// test_window.php and there a 200ms timeout is set after 
// which application is closed. The total time for launching
// application, loading these two webpages and closing app 
// takes on average 2 seconds, let's assume that if it takes
// more than 4 seconds then it means that mongoose web-server
// freezed.

// See this topic on mongoose-users group:
// https://groups.google.com/d/topic/mongoose-users/9h9470U9jXY/discussion

#include "defines.h"
#include <windows.h>
#include <iostream>
#include "executable.h"
#include "string_utils.h"

#define MIN_TIME_COUNTED_AS_FREEZE 4
const wchar_t g_exeFile[] = L"c:\\phpdesktop\\phpdesktop-src\\"
        L"phpdesktop-msie\\Debug\\php-desktop-msie.exe";

int main() {
    std::cout << "Mongoose test started" << std::endl;
    double app_time = 0;
    double tickStart = 0;
    int testNo = 0;
    while (app_time < MIN_TIME_COUNTED_AS_FREEZE) {
        testNo++;
        SHELLEXECUTEINFO seInfo = {};
        seInfo.cbSize = sizeof(seInfo);
        seInfo.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
        seInfo.lpVerb = L"open";
        seInfo.lpFile = g_exeFile;
        seInfo.nShow = SW_SHOW;
        tickStart = GetTickCount();
        if (ShellExecuteEx(&seInfo)) {
            std::cout << testNo << " Executing php-desktop-msie.exe" << std::endl;
            if (WaitForSingleObject(seInfo.hProcess, INFINITE) 
                    == WAIT_OBJECT_0) {
                app_time = (GetTickCount() - tickStart) / 1000;
                std::cout << "Execution time = " << app_time << std::endl;
            } else {
                std::cout << "WaitForSingleObject() failed" << std::endl;
                break;
            }
        } else {
            std::cout << "ShellExecuteEx() failed" << std::endl;
            break;
        }
    }
    if (app_time >= MIN_TIME_COUNTED_AS_FREEZE) {
        std::cout << "Application freezed for more than " 
                << MIN_TIME_COUNTED_AS_FREEZE 
                << " seconds, ending test"
                << std::endl;
    }
    system("pause");
    return 0;
}
