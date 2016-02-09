// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include <string>

std::string GetFileContents(std::string file);
std::string GetRealPath(std::string path);
std::string GetAbsolutePath(std::string path);
bool DirectoryExists(std::string directory);
bool AnsiDirectoryExists(std::string directory);
std::string GetDirectoryPath(const std::string& path);
bool IsDirectoryWritable(std::string directory);
