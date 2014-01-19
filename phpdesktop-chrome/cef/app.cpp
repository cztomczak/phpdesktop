// Copyright (c) 2012-2014 PHP Desktop Authors. All rights reserved.
// License: New BSD License.
// Website: http://code.google.com/p/phpdesktop/

#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")

#include "app.h"
#include <string>
#include "util.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "app.h"
#include "../log.h"
#include "client_handler.h"

App::App() {
}

void App::OnContextInitialized() {
  REQUIRE_UI_THREAD();
  LOG_DEBUG << "App::OnContextInitialized()";
}
