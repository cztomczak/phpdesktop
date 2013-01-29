// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma comment(linker, "/manifestdependency:\"type='win32' "\
    "name='Microsoft.Windows.Common-Controls' version='6.0.0.0' "\
    "processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#define WINVER          0x0500
#define _WIN32_WINNT    0x0501
#define _WIN32_IE       _WIN32_IE_IE60SP2
#define _RICHEDIT_VER   0x0200

#include <comdef.h> // CComBSTR
#include <atlbase.h>
#include <atlctl.h>
#include <atlapp.h>
#include <atlframe.h>

#include "resource.h"

#include "debug.h"
#include "executable.h"
#include "fatal_error.h"
#include "file_utils.h"
#include "log.h"
#include "main_frame.h"
#include "msie/internet_features.h"
#include "settings.h"
#include "single_instance_application.h"
#include "string_utils.h"
#include "web_server.h"

CAppModule g_appModule;
SingleInstanceApplication g_singleInstanceApplication;
wchar_t* g_singleInstanceApplicationGuid = 0;

void InitLogging(bool show_console, std::string log_level, 
                 std::string log_file);
int Run(LPTSTR lpstrCmdLine, int nCmdShow, std::string main_window_title);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPTSTR lpstrCmdLine, int nCmdShow) {
    json_value* settings = GetApplicationSettings();
    // Debugging options.    
    bool show_console = (*settings)["debugging"]["show_console"];
    std::string log_level = (*settings)["debugging"]["log_level"];
    std::string log_file = (*settings)["debugging"]["log_file"];
    if (log_file.length()) {
        log_file = GetExecutableDirectory() + "\\" + log_file;
        log_file = GetRealPath(log_file);
    }

    InitLogging(show_console, log_level, log_file);
    LOG(logINFO) << "--------------------------------------------------------";
    LOG(logINFO) << "Started application";    
    
    if (log_file.length())
        LOG(logINFO) << "Logging to: " << log_file;
    else
        LOG(logINFO) << "No logging file set";
    LOG(logINFO) << "Log level = "
            << FILELog::ToString(FILELog::ReportingLevel());

    // Main window title option.
    std::string main_window_title = (*settings)["main_window"]["title"];
    if (main_window_title.empty())
        main_window_title = GetExecutableName();

    // Single instance guid option.
    const char* single_instance_guid = 
            (*settings)["application"]["single_instance_guid"];
    if (single_instance_guid && single_instance_guid[0] != 0) {
        int guidSize = strlen(single_instance_guid) + 1;
        g_singleInstanceApplicationGuid = new wchar_t[guidSize];
        Utf8ToWide(single_instance_guid, g_singleInstanceApplicationGuid,
                   guidSize);
    }
    if (g_singleInstanceApplicationGuid 
            && g_singleInstanceApplicationGuid[0] != 0) {
        g_singleInstanceApplication.Initialize(
                g_singleInstanceApplicationGuid);
	    if (g_singleInstanceApplication.IsRunning()) {
            HWND hwnd = FindWindow(g_singleInstanceApplicationGuid, NULL);
            if (hwnd) {
                if (IsIconic(hwnd))
                    ShowWindow(hwnd, SW_RESTORE);
                SetForegroundWindow(hwnd);
                return 0;
            }
        }
    }

    HRESULT hRes = ::OleInitialize(NULL);
    _ASSERT(SUCCEEDED(hRes));

    // This resolves ATL window thunking problem when Microsoft 
    // Layer for Unicode (MSLU) is used.
    ::DefWindowProc(NULL, 0, 0, 0L);

    // Support some of the common controls.
    AtlInitCommonControls(ICC_BAR_CLASSES); 

    hRes = g_appModule.Init(NULL, hInstance);
    _ASSERT(SUCCEEDED(hRes));

    AtlAxWinInit();
    int nRet = Run(lpstrCmdLine, nCmdShow, main_window_title);
    g_appModule.Term();
    
    ::OleUninitialize();
    LOG(logINFO) << "Ended application";
    LOG(logINFO) << "--------------------------------------------------------";

    return nRet;
}
int Run(LPTSTR lpstrCmdLine, int nCmdShow, std::string main_window_title) {
    CMessageLoop theLoop;
    g_appModule.AddMessageLoop(&theLoop);

    json_value* settings = GetApplicationSettings();
    
    long default_width = (*settings)["main_window"]["default_size"][0];
    long default_height = (*settings)["main_window"]["default_size"][1];
    bool disable_maximize_button = 
            (*settings)["main_window"]["disable_maximize_button"];
    bool center_on_screen = (*settings)["main_window"]["center_on_screen"];

    SetInternetFeatures();
    if (!StartWebServer()) {
        FatalError(NULL, "Could not start internal web-server.\n"
                   "Exiting application.");
    }

    if (!default_width || !default_height) {
        default_width = 1024;
        default_height = 768;
    }

    MainFrame mainFrame;
    if(mainFrame.CreateEx(NULL, NULL) == NULL) {
        ATLTRACE(L"Main window creation failed!\n");
        return 0;
    }

    if (default_width && default_height) {
        mainFrame.SetWidth(default_width);
        mainFrame.SetHeight(default_height);
    }

    mainFrame.SetWindowText(Utf8ToWide(main_window_title).c_str());
    if (disable_maximize_button) {
        mainFrame.SetWindowLong(GWL_STYLE, 
                mainFrame.GetWindowLongW(GWL_STYLE) & ~WS_MAXIMIZEBOX);
    }
    if (center_on_screen)
        mainFrame.CenterWindow();

    mainFrame.ShowWindow(nCmdShow);
    int nRet = theLoop.Run();
    g_appModule.RemoveMessageLoop();

    return nRet;
}
void InitLogging(bool show_console, std::string log_level, 
                 std::string log_file) {
    if (show_console) {
        AllocConsole();
        FILE* freopen_file;
        freopen_s(&freopen_file, "CONIN$", "rb", stdin);
        freopen_s(&freopen_file, "CONOUT$", "wb", stdout);
        freopen_s(&freopen_file, "CONOUT$", "wb", stderr);
    }
    
    if (log_level.length())
        FILELog::ReportingLevel() = FILELog::FromString(log_level);
    else
        FILELog::ReportingLevel() = logINFO;

    if (log_file.length()) {
        FILE* pFile;
        if (0 == _wfopen_s(&pFile, Utf8ToWide(log_file).c_str(), L"a"))
            Output2FILE::Stream() = pFile;
        else
            LOG(logINFO) << "Opening log file for writing failed";
    }
}
