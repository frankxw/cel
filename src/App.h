#pragma once

#include <uv.h>
#include "ClassUtils.h"
#include "TimeKeeper.h"

namespace cel
{
    class Idler;
    class Server;

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
    public:
        App();
        ~App();

        NO_COPY(App)
        NO_MOVE(App)

    public:
        void Initialize(Server* server, Idler* idler);

        static App& GetInstance()
        {
            static App inst;
            return inst;
        }

        int Run();

        TimeKeeper& GetTimeKeeper() { return m_timeKeeper; };
        uv_loop_t* GetUVLoop() const { return m_loop; };

    private:
        bool m_initialized;
        bool m_running;
        uv_loop_t* m_loop;

        Server* m_server;
        Idler* m_idler;
        TimeKeeper m_timeKeeper;
    };

}
