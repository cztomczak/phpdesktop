// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

// TODO: ASSERT_EXIT() should terminate php server?

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "debug.h"
#include "resource.h"

#include "MainFrame.h"

#include <Shellapi.h>
#include "srand2.h"
#include "msie/internet_features.h"

#include "executable.h"

CAppModule g_Module;

SHELLEXECUTEINFO g_phpshell;
wchar_t g_addressport[] = L"127.0.0.1:54007";
wchar_t g_httpaddressport[] = L"http://127.0.0.1:54007";

void TerminatePHPShell()
{
    TerminateProcess(g_phpshell.hProcess, 0);
}

wchar_t* GetHttpAddressPort()
{
    return g_httpaddressport;
}

void StartPHPServer()
{
    wchar_t shellparams[2048]; // shellparams must be calculated before mainFrame.CreateEx() is called.
    wchar_t wwwDir[1024];
    GetExecutableDir(wwwDir, 1024);
    swprintf_s(wwwDir, 1024, L"%s\\www", wwwDir);
    swprintf_s(shellparams, 2048, L"-S %s -t %s", g_addressport, wwwDir);

    // Run php 5.4 built-in webserver
    // Server must be ready before creating window.
    // Must use ShellExecuteEx() to execute command synchronously.

    g_phpshell.cbSize = sizeof(SHELLEXECUTEINFO);
    // SEE_MASK_NOCLOSEPROCESS - so "g_phpshell.hProcess" is set.
    g_phpshell.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
    g_phpshell.hwnd = NULL;
    g_phpshell.lpVerb = L"open";

    wchar_t executableDir[1024];
    wchar_t phpExecutable[1024];
    GetExecutableDir(executableDir, 1024);
    swprintf_s(phpExecutable, 1024, L"%s\\php\\php.exe", executableDir);

    g_phpshell.lpFile = phpExecutable;
    g_phpshell.lpParameters = shellparams;
    g_phpshell.lpDirectory = NULL;
    g_phpshell.nShow = SW_HIDE;
    g_phpshell.hInstApp = NULL;
    BOOL bshell = ShellExecuteEx(&g_phpshell);
    ASSERT_EXIT(bshell, "ShellExecuteEx(php-builtin-webserver) failed");
}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    g_Module.AddMessageLoop(&theLoop);

    SetInternetFeatures();
    StartPHPServer();

    RECT rc = {0, 0, 1024, 768};
    MainFrame mainFrame;
    if(mainFrame.CreateEx(NULL, rc) == NULL) {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }

    // Center window.
    mainFrame.CenterWindow();

    // Window title.
    wchar_t executableName[1024];
    GetExecutableName(executableName, 1024);
    mainFrame.SetWindowTextW(executableName);

    mainFrame.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();
    g_Module.RemoveMessageLoop();

    return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    HRESULT hRes = ::CoInitialize(NULL);
    ATLASSERT(SUCCEEDED(hRes));

    // This resolves ATL window thunking problem when Microsoft Layer
    // for Unicode (MSLU) is used.
    ::DefWindowProc(NULL, 0, 0, 0L);

    // Support for some of the common controls.
    AtlInitCommonControls(ICC_BAR_CLASSES); 

    hRes = g_Module.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    AtlAxWinInit();
    int nRet = Run(lpstrCmdLine, nCmdShow);
    g_Module.Term();
    
    ::CoUninitialize();

    return nRet;
}
