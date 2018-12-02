// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "utils.h"
#include <limits.h>
#include <unistd.h>

std::string executable_dir() {
    // Directory in which executable resides.
    char app_path[PATH_MAX] = {};
    ssize_t pplen = readlink("/proc/self/exe", app_path, sizeof(app_path)-1);
    if (pplen != -1) {
        app_path[pplen] = '\0';
    }
    do {
        pplen -= 1;
        app_path[pplen+1] = '\0';
    } while (app_path[pplen] != '/' && pplen > 0);
    // No slash at the end.
    if (pplen > 0 && app_path[pplen] == '/') {
        app_path[pplen] = '\0';
    }
    return app_path;
}
