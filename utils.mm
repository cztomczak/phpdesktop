// Copyright (c) 2024 Czarek Tomczak, PHP Desktop.

#include "utils.h"
#include <filesystem>
#include <limits.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#import <Foundation/Foundation.h>
#include <mach-o/dyld.h>

std::string RealPath(std::string path)
{
    char buffer[PATH_MAX];
    char* result = realpath(path.c_str(), buffer);
    if (result) {
        return buffer;
    } else {
        return path;
    }
}

std::string GetResourcesDir()
{
    static bool am_i_bundled = [[[NSBundle mainBundle] bundlePath] hasSuffix:@".app"];
    std::string dir = GetExecutableDir();
    if (am_i_bundled) {
        dir.append("/../Resources");
        return RealPath(dir);
    }
    dir.append("/Resources");
    return dir;
}

std::string GetExecutableDir()
{
    // Directory in which executable resides.
    char app_path[PATH_MAX] = {};
    uint32_t bufsize = PATH_MAX;
    if (_NSGetExecutablePath(app_path, &bufsize) != 0) {
        return "";
    }
    return RealPath(std::filesystem::path{app_path}.parent_path());
}

std::string GetFileContents(std::string file)
{
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

std::string GetFullPath(std::string path)
{
    if (path.at(0) == '/') {
        return path;
    }
    std::string new_path = GetExecutableDir();
    new_path.append("/");
    new_path.append(path);
    return new_path;
}

unsigned long Mix(unsigned long a, unsigned long b, unsigned long c)
{
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

int Random(unsigned int min, unsigned int max, int recursion_level)
{
    /* Returns a semi-open interval [min, max) */
    static bool srand_initialized = false;
    if (!srand_initialized) {
        srand_initialized = true;
        unsigned long seed = Mix(clock(), time(NULL), getpid());
        srand(seed);
    }
    if (recursion_level > 100) {
        // A fallback.
        int ret = min + (rand() % (int)(max - min));
        return ret;
    }
    int base_random = rand(); /* in [0, RAND_MAX] */
    if (RAND_MAX == base_random) {
        return Random(min, max, recursion_level + 1);
    }
    /* now guaranteed to be in [0, RAND_MAX) */
    int range     = max - min,
        remainder = RAND_MAX % range,
        bucket    = RAND_MAX / range;
    /* There are range buckets, plus one smaller interval
        within remainder of RAND_MAX */
    if (base_random < RAND_MAX - remainder) {
        return min + base_random/bucket;
    } else {
        return Random(min, max, recursion_level + 1);
    }
}
