// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "defines.h"
#include <string>

std::string GetFileContents(std::string file);
std::string GetRealPath(std::string path);
std::string GetAbsolutePath(std::string path);
bool DirectoryExists(std::string directory);
std::string GetDirectoryPath(const std::string& path);
