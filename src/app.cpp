#include "app.h"
#include "include/wrapper/cef_helpers.h"

App::App() {
}

void App::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();
}
