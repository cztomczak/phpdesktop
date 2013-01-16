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

#include <atlbase.h>
#include <atlctl.h>
#include <atlapp.h>
#include <atlframe.h>

#include <iostream>
#include <fstream>

#include "resource.h"

#include "debug.h"
#include "executable.h"
#include "json.h"
#include "log.h"
#include "main_frame.h"
#include "msie/internet_features.h"
#include "string_utils.h"
#include "web_server.h"

CAppModule g_appModule;
void InitLogging();
int Run(LPTSTR, int);
json_value* GetApplicationSettings();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPTSTR lpstrCmdLine, int nCmdShow) {
    InitLogging();
    LOG(logINFO) << "--------------------------------------------------------";
    LOG(logINFO) << "Started application";
    
    json_value* settings = GetApplicationSettings();
    const char* log_level = 
            (*settings)["application"]["debugging"]["log_level"];
    const char* log_file = 
            (*settings)["application"]["debugging"]["log_file"];
    if (log_file && log_file[0] != 0)
        LOG(logINFO) << "Logging to " << log_file;
    else
        LOG(logINFO) << "No logging file set";
    LOG(logINFO) << "Log level = "\
            << FILELog::ToString(FILELog::ReportingLevel());

    HRESULT hRes = ::CoInitialize(NULL);
    ATLASSERT(SUCCEEDED(hRes));

    // This resolves ATL window thunking problem when Microsoft 
    // Layer for Unicode (MSLU) is used.
    ::DefWindowProc(NULL, 0, 0, 0L);

    // Support some of the common controls.
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
int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT) {
    CMessageLoop theLoop;
    g_appModule.AddMessageLoop(&theLoop);

    json_value* settings = GetApplicationSettings();
    
    const char* main_window_title = (*settings)["main_window"]["title"];
    wchar_t main_window_title_w[128];
    Utf8ToWide(main_window_title, &main_window_title_w[0],
               _countof(main_window_title_w));
    if (main_window_title_w[0] == 0)
        GetExecutableName(main_window_title_w, _countof(main_window_title_w));
    
    long default_width = (*settings)["main_window"]["default_size"][0];
    long default_height = (*settings)["main_window"]["default_size"][1];
    bool disable_maximize_button = 
            (*settings)["main_window"]["disable_maximize_button"];
    bool center_on_screen = (*settings)["main_window"]["center_on_screen"];

    SetInternetFeatures();
    if (!StartWebServer()) {
        MessageBox(NULL, L"Could not start internal web-server",
                   main_window_title_w, MB_ICONERROR);
        exit(-1);
    }

    if (!default_width || !default_height) {
        default_width = 1024;
        default_height = 768;
    }

    RECT rc = {0, 0, default_width, default_height};
    MainFrame mainFrame;
    if(mainFrame.CreateEx(NULL, rc) == NULL) {
        ATLTRACE(L"Main window creation failed!\n");
        return 0;
    }

    mainFrame.SetWindowTextW(main_window_title_w);
    if (disable_maximize_button) {
        mainFrame.SetWindowLongW(GWL_STYLE, 
                mainFrame.GetWindowLongW(GWL_STYLE) & ~WS_MAXIMIZEBOX);
    }
    if (center_on_screen)
        mainFrame.CenterWindow();

    mainFrame.ShowWindow(nCmdShow);
    int nRet = theLoop.Run();
    g_appModule.RemoveMessageLoop();

    return nRet;
}
void InitLogging() {
    json_value* settings = GetApplicationSettings();
    const bool show_console = 
            (*settings)["application"]["debugging"]["show_console"];
    const char* log_level = 
            (*settings)["application"]["debugging"]["log_level"];
    const char* log_file = 
            (*settings)["application"]["debugging"]["log_file"];

    if (show_console) {
        AllocConsole();
        FILE* freopen_file;
        freopen_s(&freopen_file, "CONIN$", "rb", stdin);
        freopen_s(&freopen_file, "CONOUT$", "wb", stdout);
        freopen_s(&freopen_file, "CONOUT$", "wb", stderr);
    }
    
    if (log_level && log_level[0] != 0)
        FILELog::ReportingLevel() = FILELog::FromString(log_level);
    else
        FILELog::ReportingLevel() = logINFO;

    if (log_file && log_file[0] != 0) {
        FILE* pFile;    
        wchar_t debug_file[1024];
        GetExecutableDirectory(debug_file, _countof(debug_file));
        swprintf_s(debug_file, _countof(debug_file), L"%s\\debug.log", debug_file);
        if (0 == _wfopen_s(&pFile, debug_file, L"a"))
            Output2FILE::Stream() = pFile;
    }
}
json_value* GetApplicationSettings() {
    static json_value* ret = new json_value();
    static bool settings_fetched = false;
    if (settings_fetched)
        return ret;
    settings_fetched = true;
    LOG(logDEBUG) << "Fetching settings from settings.json file";

    wchar_t settingsFile[1024];
    GetExecutableDirectory(settingsFile, _countof(settingsFile));
    swprintf_s(settingsFile, _countof(settingsFile), L"%s\\settings.json",
               settingsFile);

    std::ifstream inFile;
    inFile.open(settingsFile, std::ios::in);
    if (!inFile) {
        LOG(logWARNING) << "Error while opening settings.json file";
        return ret;
    }
    std::string json_data;
    inFile.seekg(0, std::ios::end);
    json_data.resize(inFile.tellg());
    inFile.seekg(0, std::ios::beg);
    inFile.read(&json_data[0], json_data.size());
    inFile.close();

    json_settings settings;
    memset(&settings, 0, sizeof(json_settings));
    char error[256];
    json_value* json_parsed = json_parse_ex(&settings, json_data.c_str(), 
            &error[0]);
    if (json_parsed == 0) {
        LOG(logWARNING) << "Error while parsing settings.json file: " << error;
        return ret;
    }
    ret = json_parsed;
    return ret;
}
