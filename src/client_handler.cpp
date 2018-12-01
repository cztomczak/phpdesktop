// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

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
