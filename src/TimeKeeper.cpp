#include <stdlib.h>
#include "App.h"
#include "Logging.h"
#include "TimeKeeper.h"
#include "TimerCallback.h"

using namespace cel;

void timerCallback(uv_timer_t* handle);

int TimeKeeper::SetTimeout(uint64_t timeout, TimerCallback* callback)
{
    return CreateTimer(timeout, 0, callback);
}

void TimeKeeper::ClearTimeout(int timerId)
{
    return RemoveTimer(timerId);
}

int TimeKeeper::SetInterval(uint64_t timeout, uint64_t repeat, TimerCallback* callback)
{
    return CreateTimer(timeout, repeat, callback);
}

void TimeKeeper::ClearInterval(int timerId)
{
    return RemoveTimer(timerId);
}

int TimeKeeper::CreateTimer(uint64_t timeout, uint64_t repeat, TimerCallback* callback)
{
    App& app = App::GetInstance();

    const int key = GenerateTimerId();
    if(key == 0) {
        return 0;
    }

    m_timers[key] = uv_timer_t();

    auto elt = m_timers.find(key);
    if(elt == m_timers.end()) {
        LogErr(LogLevel::Normal, "Failed to insert timer.\n");
        return 0;
    }

    uv_timer_init(app.GetLoop(), &elt->second);
    elt->second.data = callback;
    uv_timer_start(&elt->second, timerCallback, timeout, repeat);

    return key;
}

void TimeKeeper::RemoveTimer(int key)
{
    auto elt = m_timers.find(key);
    if(elt == m_timers.end()) {
        return;
    }

    uv_timer_stop(&elt->second);
    m_timers.erase(elt);
}

int TimeKeeper::GenerateTimerId()
{
    int key = m_timers.size();

    bool passedZero = false;
    while(true) {
        key++;
        if(key == 0) {
            if(passedZero) {
                LogErr(LogLevel::Normal, "Ran out of timer ids.\n");
                return 0;
            }
            else {
                passedZero = true;
                continue;
            }
        }

        if(m_timers.find(key) == m_timers.end()) {
            return key;
        }
    }
}

void timerCallback(uv_timer_t* handle)
{
    TimerCallback *cb = static_cast<TimerCallback*>(handle->data);
    if(!cb) {
        return;
    }

    cb->Callback();
}
