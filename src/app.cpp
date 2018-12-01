// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "app.h"
#include "include/wrapper/cef_helpers.h"

App::App() {
}

void App::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
}
