// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include <string>

std::string GetResourcesDir();
std::string GetExecutableDir();
std::string GetFileContents(std::string file);
std::string GetFullPath(std::string path);
int Random(unsigned int min, unsigned int max, int recursion_level=0);
