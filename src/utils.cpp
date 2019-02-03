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

unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

int random(unsigned int min, unsigned int max, int recursion_level) {
    /* Returns a semi-open interval [min, max) */
    static bool srand_initialized = false;
    if (!srand_initialized) {
        srand_initialized = true;
        unsigned long seed = mix(clock(), time(NULL), getpid());
        srand(seed);
    }
    if (recursion_level > 100) {
        // A fallback.
        int ret = min + (rand() % (int)(max - min));
        return ret;
    }
    int base_random = rand(); /* in [0, RAND_MAX] */
    if (RAND_MAX == base_random)
        return random(min, max, recursion_level + 1);
    /* now guaranteed to be in [0, RAND_MAX) */
    int range     = max - min,
        remainder = RAND_MAX % range,
        bucket    = RAND_MAX / range;
    /* There are range buckets, plus one smaller interval
        within remainder of RAND_MAX */
    if (base_random < RAND_MAX - remainder) {
        return min + base_random/bucket;
    } else {
        return random(min, max, recursion_level + 1);
    }
}
