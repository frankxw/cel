#include <stdlib.h>
#include "App.h"
#include "Exceptions.h"
#include "Idler.h"
#include "Logging.h"
#include "Server.h"

using namespace cel;

App::App()
    : m_initialized(false)
    , m_running(false)
    , m_loop(nullptr)
    , m_server(nullptr)
    , m_idler(nullptr)
{

}

App::~App()
{
    if(m_loop) {
        uv_loop_close(m_loop);
        m_loop = nullptr;
    }
}

void App::Initialize(Server* server, Idler* idler)
{
    CEL_ASSERT(!m_initialized, "Trying to initialize app again.\n");

    m_initialized = true;
    m_loop = uv_default_loop();

    m_server = server;
    m_idler = idler;
}

int App::Run()
{
    CEL_ASSERT(m_initialized, "Trying to run app without initializing it.\n");
    CEL_ASSERT(!m_running, "App is already running, cannot run again.\n");

    m_running = true;

    if(m_idler) {
        m_idler->Start(m_loop);
    }

    if(m_server) {
        m_server->Start(m_loop);
    }

    return uv_run(m_loop, UV_RUN_DEFAULT);
}

void App::Quit()
{
    m_timeKeeper.CancelAllTimers();
    uv_stop(m_loop);
    m_running = false;
}

int App::SetTimeout(uint64_t timeout, TimerCallback callback)
{
    return m_timeKeeper.SetTimeout(timeout, callback);
}

int App::SetInterval(uint64_t timeout, uint64_t repeat, TimerCallback callback)
{
    return m_timeKeeper.SetInterval(timeout, repeat, callback);
}

void App::CancelTimer(int timerId)
{
    m_timeKeeper.CancelTimer(timerId);
}
