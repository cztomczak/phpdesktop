// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "utils.h"
#include <limits.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

std::string get_executable_dir() {
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

std::string get_file_contents(std::string file) {
    std::ifstream in_file;
    in_file.open(file.c_str(), std::ios::in);
    if (!in_file)
        return "";
    std::string contents = "";
    in_file.seekg(0, std::ios::end);
    contents.resize(static_cast<unsigned int>(in_file.tellg()));
    in_file.seekg(0, std::ios::beg);
    in_file.read(&contents[0], contents.size());
    in_file.close();
    return contents;
}

std::string get_full_path(std::string path) {
    if (path.at(0) == '/') {
        return path;
    }
    std::string new_path = get_executable_dir();
    new_path.append("/");
    new_path.append(path);
    return new_path;
}
