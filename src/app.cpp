// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "app.h"
#include "include/wrapper/cef_helpers.h"
#include "print_handler_gtk.h"

App::App() {
}

void App::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
    print_handler_ = new ClientPrintHandlerGtk();
}

CefRefPtr<CefPrintHandler> App::GetPrintHandler() {
    return print_handler_;
}
