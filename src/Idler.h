#pragma once

#include <uv.h>
#include "ClassUtils.h"

namespace cel
{
    class App;

    /**
     * The idler allows you to opt in to a callback each event loop tick.
     * You should not do long-running operations, but instead
     * spawn an asynchronous operation. For the most part, you'll
     * want to use the timer system instead of the Idler.
     */
    class Idler
    {
    friend class App;

    public:
        Idler() {};
        NO_COPY(Idler)
        NO_MOVE(Idler)
    protected:
        ~Idler();

    public:
        virtual void Execute() {};

    private:
        // Should only be called by App
        void Start(uv_loop_t* loop);

    private:
        uv_idle_t m_uvIdler;
    };

}
