// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "defines.h"
#include <string>

bool StartWebServer();
void StopWebServer();
int GetWebServerPort();
std::string GetWebServerIpAddress();
std::string GetWebServerUrl();
std::string GetWwwDirectory();
std::string GetCgiInterpreter();
