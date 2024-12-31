#include <stdlib.h>
#include "App.h"
#include "Exceptions.h"
#include "Logging.h"
#include "TimeKeeper.h"

using namespace cel;

void timerCallback(uv_timer_t* handle);

int TimeKeeper::SetTimeout(uint64_t timeout, TimerCallback callback)
{
    return CreateTimer(timeout, 0, callback);
}

int TimeKeeper::SetInterval(uint64_t timeout, uint64_t repeat, TimerCallback callback)
{
    return CreateTimer(timeout, repeat, callback);
}

void TimeKeeper::CancelTimer(int timerId)
{
    RemoveTimer(timerId);
}

int TimeKeeper::CreateTimer(uint64_t timeout, uint64_t repeat, TimerCallback callback)
{
    App& app = App::GetInstance();

    const int key = GenerateTimerId();
    if(key == 0) {
        return 0;
    }

    m_timers[key] = {callback, uv_timer_t(), key, repeat > 0};

    auto it = m_timers.find(key);
    CHECK(it != m_timers.end(), return 0;, LogLevel::Normal, "Failed to insert timer.\n");
    TimerInternalCallback& timerData = it->second;

    uv_timer_init(app.GetUVLoop(), &timerData.m_uvTimerHandle);
    timerData.m_uvTimerHandle.data = &timerData;
    uv_timer_start(&timerData.m_uvTimerHandle, timerCallback, timeout, repeat);

    return key;
}

void TimeKeeper::RemoveTimer(int key)
{
    auto it = m_timers.find(key);
    if(it == m_timers.end()) {
        return;
    }

    uv_timer_stop(&it->second.m_uvTimerHandle);
    m_timers.erase(it);
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

void TimeKeeper::CancelAllTimers()
{
    for(auto it : m_timers)
    {
        uv_timer_stop(&it.second.m_uvTimerHandle);
    }
    m_timers.clear();
}

void timerCallback(uv_timer_t* handle)
{
    CHECK(handle != nullptr, return;, LogLevel::Normal, "timerCallback error: null handle.\n");

    App& app = App::GetInstance();
    TimerInternalCallback* cbData = static_cast<TimerInternalCallback*>(handle->data);
    CHECK(cbData != nullptr, return;, LogLevel::Normal, "timerCallback error: failed to parse timer context.\n");
    cbData->m_callback();
    if(!cbData->m_repeating) {
        app.CancelTimer(cbData->m_key);
    }
}
