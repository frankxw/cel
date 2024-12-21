#pragma once

namespace cel
{
    /**
     * Construct these to receive a callback when a timer fires.
     * Fill in the context to pass custom data to the callback;
     */
    class TimerCallback {
    public:
        TimerCallback(void* context) : m_context(context) {};
        virtual ~TimerCallback() {};

        virtual void Callback() {};

    protected:
        void* m_context;
    };

}
