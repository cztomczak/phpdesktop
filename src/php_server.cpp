// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "defines.h"
#include <windows.h>
#include <Shellapi.h>
#include "settings.h"
#include "log.h"
#include "executable.h"
#include "file_utils.h"

SHELLEXECUTEINFO g_phpShell;
std::string g_webServerUrl;

bool StartWebServer() {
    LOG_INFO << "Starting PHP built-in web server";
    json_value* settings = GetApplicationSettings();

    // Web server url from settings.
    std::string ipAddress = (*settings)["web_server"]["listen_on"][0];
    std::string port = (*settings)["web_server"]["listen_on"][1];
    long portInt = (*settings)["web_server"]["listen_on"][1];
    if (portInt) {
        port = IntToString(portInt);
    }
    if (ipAddress.empty() || port.empty()) {
        ipAddress = "127.0.0.1";
        port = "54007";
    }
    g_webServerUrl = "http://" + ipAddress + ":" + port + "/";
    LOG_INFO << "Web server url: " << g_webServerUrl;

    // WWW directory from settings.
    std::string wwwDirectory = (*settings)["web_server"]["www_directory"];
    if (wwwDirectory.empty())
        wwwDirectory = "www";
    wwwDirectory = GetExecutableDirectory().append("\\").append(wwwDirectory);
    // Mongoose won't accept "..\\" in a path, need a real path.
    wwwDirectory = GetRealPath(wwwDirectory);
    LOG_INFO << "WWW directory: " << wwwDirectory;

    // PHP executable from settings.
    std::string phpExecutable = (*settings)["web_server"]["php_executable"];
    if (phpExecutable.empty())
        phpExecutable = "php\\php.exe";
    phpExecutable.insert(0, GetExecutableDirectory() + "\\");
    phpExecutable = GetRealPath(phpExecutable);
    LOG_INFO << "PHP executable: " << phpExecutable;

    // PHP shell parameters.
    std::string shellParams;
    shellParams.append("-S ");
    shellParams.append(ipAddress);
    shellParams.append(":");
    shellParams.append(port);
    shellParams.append(" -t ");
    shellParams.append(wwwDirectory);
    LOG(logDEBUG) << "PHP shell parameters: " << shellParams;

    memset(&g_phpShell, 0, sizeof(g_phpShell));
    g_phpShell.cbSize = sizeof(g_phpShell);
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

    if (ShellExecuteEx(&g_phpShell)) {
        return true;
    }
    return false;
}
void StopWebServer() {
    LOG_INFO << "Stopping PHP built-in web server";
    TerminateProcess(g_phpShell.hProcess, EXIT_SUCCESS);
}
