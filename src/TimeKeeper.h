#pragma once

#include <unordered_map>
#include <uv.h>

using TimerMap = std::unordered_map<int, uv_timer_t>;

namespace cel
{
    class App;
    class TimerCallback;

    /**
     * Interface for time operations.
     */
    class TimeKeeper
    {
    friend class App;

    private:
        TimeKeeper() {};
        ~TimeKeeper() {};

    public:
        int SetTimeout(uint64_t timeout, TimerCallback* callback);
        void ClearTimeout(int timerId);

        int SetInterval(uint64_t timeout, uint64_t repeat, TimerCallback* callback);
        void ClearInterval(int timerId);

    private:
        int CreateTimer(uint64_t timeout, uint64_t repeat, TimerCallback* callback);
        void RemoveTimer(int key);
        int GenerateTimerId();

    private:
        TimerMap m_timers;
    };

}
