// Copyright (c) 2012-2013 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include <windows.h>
#include <Shellapi.h>
#include <stdio.h>
#include <wchar.h>

#include "executable.h"
#include "log.h"
#include "settings.h"
#include "string_utils.h"
#include "web_server.h"

SHELLEXECUTEINFO g_phpShell;
std::string g_webServerUrl;

bool StartWebServer() {
    // Run php 5.4 built-in webserver
    // Server must be ready before creating window.
    // Must use ShellExecuteEx() to execute command synchronously.

    LOG(logINFO) << "Starting web-server";
    json_value* settings = GetApplicationSettings();

    // Web-server url from settings.
    std::string ipAddress = (*settings)["web_server"]["listen_on"][0];
    long port = (*settings)["web_server"]["listen_on"][1];
    if (ipAddress.empty() || !port) {
        ipAddress = "127.0.0.1";
        port = 80;
    }
    g_webServerUrl = "http://" + ipAddress + ":" + IntToString(port) + "/";
    LOG(logINFO) << "Web-server url: " << g_webServerUrl;

    // WWW directory from settings.
    std::string wwwDirectory = (*settings)["web_server"]["www_directory"];
    if (wwwDirectory.empty())
        wwwDirectory = "www";
    wwwDirectory = GetExecutableDirectory().append("\\").append(wwwDirectory);
    LOG(logINFO) << "WWW directory: " << wwwDirectory;

    // PHP executable from settings.
    std::string phpExecutable = (*settings)["web_server"]["php_executable"];
    if (phpExecutable.empty())
        phpExecutable = "php\\php.exe";
    phpExecutable.insert(0, GetExecutableDirectory() + "\\");
    LOG(logINFO) << "PHP executable: " << phpExecutable;

    // Shell params.
    std::string shellParams;
    shellParams = "-S " + ipAddress + ":" + IntToString(port) + " -t "
                  + wwwDirectory;
    LOG(logDEBUG) << "Shell params: " << shellParams;

    g_phpShell.cbSize = sizeof(SHELLEXECUTEINFO);
    // SEE_MASK_NOCLOSEPROCESS - so "g_phpShell.hProcess" is set.
    g_phpShell.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
    g_phpShell.hwnd = NULL;
    g_phpShell.lpVerb = L"open";

    // Lifetime of c_str() must outlast until ShellExecuteEx() is called.
    std::wstring phpExecutableWide = Utf8ToWide(phpExecutable);
    std::wstring shellParamsWide = Utf8ToWide(shellParams);

    g_phpShell.lpFile = phpExecutableWide.c_str();
    g_phpShell.lpParameters = shellParamsWide.c_str();
    g_phpShell.lpDirectory = NULL;
    g_phpShell.nShow = SW_HIDE;
    g_phpShell.hInstApp = NULL;
    
    if (ShellExecuteEx(&g_phpShell))
        return true;
    return false;
}
void TerminateWebServer() {
    TerminateProcess(g_phpShell.hProcess, 0);
}
