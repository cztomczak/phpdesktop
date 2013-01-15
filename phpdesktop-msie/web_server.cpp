// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include <windows.h>
#include <Shellapi.h>
#include <wchar.h>

#include "executable.h"
#include "web_server.h"

SHELLEXECUTEINFO g_phpShell;

bool StartWebServer()
{
    wchar_t shellparams[2048]; // shellparams must be calculated before mainFrame.CreateEx() is called.
    wchar_t wwwDir[1024];
    GetExecutableDir(wwwDir, 1024);
    swprintf_s(wwwDir, 1024, L"%s\\www", wwwDir);
    swprintf_s(shellparams, 2048, L"-S %s -t %s", L"127.0.0.1:54007", wwwDir);

    // Run php 5.4 built-in webserver
    // Server must be ready before creating window.
    // Must use ShellExecuteEx() to execute command synchronously.

    g_phpShell.cbSize = sizeof(SHELLEXECUTEINFO);
    // SEE_MASK_NOCLOSEPROCESS - so "g_phpShell.hProcess" is set.
    g_phpShell.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
    g_phpShell.hwnd = NULL;
    g_phpShell.lpVerb = L"open";

    wchar_t executableDir[1024];
    wchar_t phpExecutable[1024];
    GetExecutableDir(executableDir, 1024);
    swprintf_s(phpExecutable, 1024, L"%s\\php\\php.exe", executableDir);

    g_phpShell.lpFile = phpExecutable;
    g_phpShell.lpParameters = shellparams;
    g_phpShell.lpDirectory = NULL;
    g_phpShell.nShow = SW_HIDE;
    g_phpShell.hInstApp = NULL;
    if (!ShellExecuteEx(&g_phpShell))
        return false;
    return true;
}

void TerminateWebServer() 
{
    TerminateProcess(g_phpShell.hProcess, 0);
}
