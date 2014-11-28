// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#include "logging.h"
#include <Windows.h>
#include <io.h>
#include <Fcntl.h>

#include "log.h"
#include "executable.h"
#include "string_utils.h"

HANDLE g_logFileHandle = NULL;

void InitializeLogging(bool show_console, std::string log_level,
                 std::string log_file) {
    if (show_console) {
        AllocConsole();
        FILE* freopen_file;
        freopen_s(&freopen_file, "CONIN$", "rb", stdin);
        freopen_s(&freopen_file, "CONOUT$", "wb", stdout);
        freopen_s(&freopen_file, "CONOUT$", "wb", stderr);
    }

#ifdef DEBUG
    // Debugging mongoose web server.
    FILE* mongoose_file;
    freopen_s(&mongoose_file,
            GetExecutableDirectory().append("\\debug-mongoose.log").c_str(),
            "ab", stdout);
#endif

    if (log_level.length())
        FILELog::ReportingLevel() = FILELog::FromString(log_level);
    else
        FILELog::ReportingLevel() = logINFO;

    if (log_file.length()) {
        // The log file needs to be opened in shared mode so that
        // CEF subprocesses can also append to this file.
        // Converting HANDLE to FILE*:
        // http://stackoverflow.com/a/7369662/623622
        // Remember to call ShutdownLogging() to close the g_logFileHandle.
        g_logFileHandle = CreateFile(
                    Utf8ToWide(log_file).c_str(),
                    GENERIC_WRITE,
                    FILE_SHARE_WRITE,
                    NULL,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
        if (g_logFileHandle == INVALID_HANDLE_VALUE) {
            g_logFileHandle = NULL;
            LOG_ERROR << "Opening log file for appending failed";
            return;
        }
        int fd = _open_osfhandle((intptr_t)g_logFileHandle, _O_APPEND | _O_RDONLY);
        if (fd == -1) {
            LOG_ERROR << "Opening log file for appending failed, "
                      << "_open_osfhandle() failed";
            return;
        }
        FILE* pFile = _fdopen(fd, "a+");
        if (pFile == 0) {
            _close(fd);
            LOG_ERROR << "Opening log file for appending failed, "
                      << "_fdopen() failed";
            return;
        }
        // TODO: should we call fclose(pFile) later?
        Output2FILE::Stream() = pFile;
    }
}
void ShutdownLogging() {
    if (g_logFileHandle) {
        CloseHandle(g_logFileHandle);
    }
}
