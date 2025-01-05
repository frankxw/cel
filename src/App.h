#pragma once

#include <functional>
#include <uv.h>
#include "ClassUtils.h"
#include "CommonTypes.h"
#include "TimeKeeper.h"

namespace cel
{
    class Idler;
    class Server;

    using IdlerTickCallback = std::function<void()>;

    /**
     * Main server app.
     * This is the equivalent of the main event loop.
     *
     * This class is a Singleton because it is a wrapper around uv_default_loop,
     * so you shouldn't make multiple instances of it.  It's also nice to be able
     * to globally access this, since all of the core systems will be rooted here.
     */
    class App
    {
    friend class Server;
    friend class TimeKeeper;

    public:
        App();
        ~App();

        NO_COPY(App)
        NO_MOVE(App)

    public:
        static App& GetInstance()
        {
            static App inst;
            return inst;
        }

        int Run();
        void Quit();

        void SetServer(Server* server);
        Server* GetServer() const { return m_server; }

        int SetTimeout(uint64_t timeout, TimerCallback callback);
        int SetInterval(uint64_t timeout, uint64_t repeat, TimerCallback callback);
        void CancelTimer(int timerId);

        void OnIdlerTick(IdlerTickCallback callback);

    public:
        void HandleIdlerTick();

    private:
        uv_loop_t* GetUVLoop() const { return m_loop; };

    private:
        bool m_initialized;
        bool m_running;
        uv_loop_t* m_loop;
        uv_idle_t m_uvIdler;

        Server* m_server;
        TimeKeeper m_timeKeeper;
        std::function<void()> m_idlerTickCallback;
    };
}
