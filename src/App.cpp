#include <stdlib.h>
#include "App.h"
#include "Idler.h"
#include "Logging.h"
#include "Server.h"

using namespace cel;

App::App()
    : m_running(false)
    , m_idler(nullptr)
    , m_server(nullptr)
{
    m_loop = uv_default_loop();
}

App::~App()
{
    m_idler = nullptr;
    m_server = nullptr;

    if(m_loop) {
        uv_loop_close(m_loop);
        m_loop = nullptr;
    }
}

int App::Run()
{
    if(m_running) {
        LogErr(LogLevel::Normal, "App is already running, cannot run again.\n");
        return 0;
    }

    m_running = true;

    if(m_idler) {
        m_idler->Start(m_loop);
    }

    if(m_server) {
        m_server->Start(m_loop);
    }

    return uv_run(m_loop, UV_RUN_DEFAULT);
}

void App::SetIdler(Idler *idler)
{
    if(m_running) {
        LogErr(LogLevel::Normal, "SetIdler error: Cannot set idler when app is already running.\n");
        return;
    }

    m_idler = idler;
}

void App::SetServer(Server *server)
{
    if(m_running) {
        LogErr(LogLevel::Normal, "SetServer error: Cannot set server when app is already running.\n");
        return;
    }

    m_server = server;
}
