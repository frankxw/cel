#include <stdlib.h>
#include "App.h"
#include "Exceptions.h"
#include "Logging.h"
#include "Server.h"

using namespace cel;

static void idlerHook(uv_idle_t* handle);

App::App()
    : m_running(false)
    , m_server(nullptr)
{
    m_loop = uv_default_loop();
}

App::~App()
{
    m_uvIdler.data = nullptr;

    if(m_loop) {
        uv_loop_close(m_loop);
        m_loop = nullptr;
    }
}

int App::Run()
{
    CEL_ASSERT(!m_running, "App is already running, cannot run again.\n");

    m_running = true;

    if(m_idlerTickCallback) {
        uv_idle_init(m_loop, &m_uvIdler);
        uv_idle_start(&m_uvIdler, idlerHook);
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

void App::SetServer(Server* server)
{
    CEL_ASSERT(!m_running, "Trying to call SetServer when the app is already running.\n");
    m_server = server;
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

void App::OnIdlerTick(IdlerTickCallback callback)
{
    CEL_CHECK(!m_running, return;, LogLevel::Normal, "Trying to call OnIdlerTick when the app is already running.\n");
    m_idlerTickCallback = callback;
}

void App::HandleIdlerTick()
{
    CEL_CHECK(m_idlerTickCallback, return;, LogLevel::Normal, "HandleIdlerTick: missing idler tick callback.\n");
    m_idlerTickCallback();
}

static void idlerHook(uv_idle_t* handle)
{
    App& app = App::GetInstance();
    app.HandleIdlerTick();
}
