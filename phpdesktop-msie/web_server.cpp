// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include <windows.h>
#include <Shellapi.h>
#include <wchar.h>

#include "executable.h"
#include "web_server.h"

SHELLEXECUTEINFO g_phpShell;

bool StartWebServer() {
    wchar_t shellParams[2048];
    wchar_t wwwDirectory[4096];
    GetExecutableDirectory(wwwDirectory, _countof(wwwDirectory));
    swprintf_s(wwwDirectory, _countof(wwwDirectory), L"%s\\www", wwwDirectory);
    swprintf_s(shellParams, _countof(shellParams), L"-S %s -t %s", 
               L"127.0.0.1:54007", wwwDirectory);

    // Run php 5.4 built-in webserver
    // Server must be ready before creating window.
    // Must use ShellExecuteEx() to execute command synchronously.

    g_phpShell.cbSize = sizeof(SHELLEXECUTEINFO);
    // SEE_MASK_NOCLOSEPROCESS - so "g_phpShell.hProcess" is set.
    g_phpShell.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
    g_phpShell.hwnd = NULL;
    g_phpShell.lpVerb = L"open";

    wchar_t executableDirectory[4096];
    wchar_t phpExecutable[4096];
    GetExecutableDirectory(executableDirectory, _countof(executableDirectory));
    swprintf_s(phpExecutable, _countof(phpExecutable), L"%s\\php\\php.exe",
            executableDirectory);

    g_phpShell.lpFile = phpExecutable;
    g_phpShell.lpParameters = shellParams;
    g_phpShell.lpDirectory = NULL;
    g_phpShell.nShow = SW_HIDE;
    g_phpShell.hInstApp = NULL;
    if (!ShellExecuteEx(&g_phpShell))
        return false;
    return true;
}
void TerminateWebServer() {
    TerminateProcess(g_phpShell.hProcess, 0);
}
