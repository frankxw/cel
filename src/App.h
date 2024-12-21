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
        NO_COPY(App)
        NO_MOVE(App)

        static App& GetInstance() {
            static App inst;
            return inst;
        }

        int Run();

        uv_loop_t* GetLoop() { return m_loop; };
        TimeKeeper& GetTimeKeeper() { return m_timeKeeper; };

        void SetIdler(Idler* idler);
        void SetServer(Server* server);

    public:
        App();
        ~App();

        bool m_running;
        uv_loop_t* m_loop;
        Idler* m_idler;
        Server* m_server;
        TimeKeeper m_timeKeeper;
    };

}
