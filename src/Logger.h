#pragma once

#include "ClassUtils.h"
#include "Logging.h"

#define CHECK_LOG_LEVEL(level) if(level < m_logLevel) return;

namespace cel
{
    /**
     * Interface for common logging methods.
     * It is expected that users of cel will supply this interface.
     */
    class Logger
    {
    public:
        NO_COPY(Logger)
        NO_MOVE(Logger)

        Logger() : m_logLevel(LogLevel::Normal) {}
        Logger(LogLevel level) : m_logLevel(level) {}

        virtual void Log(LogType type, LogLevel level, const char* message) const = 0;
        virtual void Log(LogType type, LogLevel level, const char* format, va_list args) const = 0;

        void SetLogLevel(LogLevel level);

    protected:
        LogLevel m_logLevel;
    };

}
