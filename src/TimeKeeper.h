#pragma once

#include <unordered_map>
#include <uv.h>
#include "CommonTypes.h"

namespace cel
{
    class App;

    struct TimerInternalCallback
    {
    public:
        TimerInternalCallback() = default; // This is stored in a map, so we need a default ctor
        TimerInternalCallback(TimerCallback callback, uv_timer_t timerHandle, int key, bool repeating)
            : m_callback(callback), m_uvTimerHandle(timerHandle), m_key(key), m_repeating(repeating) {}

        TimerCallback m_callback;
        uv_timer_t m_uvTimerHandle;
        int m_key;
        bool m_repeating;
    };

    using TimerMap = std::unordered_map<int, TimerInternalCallback>;

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
        int SetTimeout(uint64_t timeout, TimerCallback callback);
        int SetInterval(uint64_t timeout, uint64_t repeat, TimerCallback callback);
        void CancelTimer(int timerId);

    private:
        int CreateTimer(uint64_t timeout, uint64_t repeat, TimerCallback callback);
        void RemoveTimer(int key);
        int GenerateTimerId();
        void CancelAllTimers();

    private:
        TimerMap m_timers;
    };
}
