// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "defines.h"
#include <string>

void InitializeLogging(bool show_console, std::string log_level,
                 std::string log_file);
void ShutdownLogging();
