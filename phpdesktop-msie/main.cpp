// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#define RELEASE_MODE 0

#pragma comment(linker, "/manifestdependency:\"type='win32' "\
    "name='Microsoft.Windows.Common-Controls' version='6.0.0.0' "\
    "processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#define WINVER          0x0500
#define _WIN32_WINNT    0x0501
#define _WIN32_IE       _WIN32_IE_IE60SP2
#define _RICHEDIT_VER   0x0200

#include <atlbase.h>
#include <atlctl.h>
#include <atlapp.h>
#include <atlframe.h>

#include "resource.h"

#include "debug.h"
#include "executable.h"
#include "log.h"
#include "main_frame.h"
#include "msie/internet_features.h"
#include "web_server.h"

CAppModule g_appModule;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    CMessageLoop theLoop;
    g_appModule.AddMessageLoop(&theLoop);

    wchar_t executableName[1024];
    GetExecutableName(executableName, 1024);

    SetInternetFeatures();
    if (!StartWebServer()) {
        MessageBox(NULL, L"Could not start PHP built-in webserver",
                executableName, MB_ICONERROR);
        exit(-1);
    }

    RECT rc = {0, 0, 1024, 768};
    MainFrame mainFrame;
    if(mainFrame.CreateEx(NULL, rc) == NULL) {
        ATLTRACE(_T("Main window creation failed!\n"));
        return 0;
    }

    // Center window.
    mainFrame.CenterWindow();
    // Window title.
    mainFrame.SetWindowTextW(executableName);

    mainFrame.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();
    g_appModule.RemoveMessageLoop();

    return nRet;
}

void InitLogging() {
#if RELEASE_MODE
    FILELog::ReportingLevel() = logINFO;
    FILE* pFile;
    if (0 == fopen_s(&pFile, "debug.log", "a"))
        Output2FILE::Stream() = pFile;
#else
    AllocConsole();
    FILE* freopen_file;
    freopen_s(&freopen_file, "CONIN$", "rb", stdin);
    freopen_s(&freopen_file, "CONOUT$", "wb", stdout);
    freopen_s(&freopen_file, "CONOUT$", "wb", stderr);
    
    FILELog::ReportingLevel() = logDEBUG4;
    FILE* pFile;
    
    wchar_t debug_file[1024];
    GetExecutableDir(debug_file, 1024);
    swprintf_s(debug_file, 1024, L"%s\\debug.log", debug_file);

    if (0 == _wfopen_s(&pFile, debug_file, L"a"))
        Output2FILE::Stream() = pFile;
#endif
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
    InitLogging();
    LOG(logINFO) << "--------------------------------------------------------";
    LOG(logINFO) << "Started application";

    HRESULT hRes = ::CoInitialize(NULL);
    ATLASSERT(SUCCEEDED(hRes));

    // This resolves ATL window thunking problem when Microsoft Layer
    // for Unicode (MSLU) is used.
    ::DefWindowProc(NULL, 0, 0, 0L);

    // Support for some of the common controls.
    AtlInitCommonControls(ICC_BAR_CLASSES); 

    hRes = g_appModule.Init(NULL, hInstance);
    ATLASSERT(SUCCEEDED(hRes));

    AtlAxWinInit();
    int nRet = Run(lpstrCmdLine, nCmdShow);
    g_appModule.Term();
    
    ::CoUninitialize();
    LOG(logINFO) << "Ended application";
    LOG(logINFO) << "--------------------------------------------------------";

    return nRet;
}
