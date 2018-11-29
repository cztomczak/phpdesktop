#include "client_handler.h"
#include "include/cef_app.h"

namespace {

ClientHandler* g_instance = NULL;

} // namespace

ClientHandler::ClientHandler()
{
    DCHECK(!g_instance);
    g_instance = this;
}

ClientHandler::~ClientHandler()
{
    g_instance = NULL;
}

// static
ClientHandler* ClientHandler::GetInstance()
{
    if( g_instance )
    {
        return g_instance;
    }
    else
    {
        return new ClientHandler();
    }

}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CefQuitMessageLoop();
}
