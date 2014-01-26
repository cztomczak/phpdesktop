// Copyright (c) 2012-2014 The PHP Desktop authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma once

#include "defines.h"
#include "json.h"
#include <string>

json_value* GetApplicationSettings();
std::string GetApplicationSettingsError();
