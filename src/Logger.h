#pragma once

#include "ClassUtils.h"
#include "Logging.h"

namespace cel
{
    /**
     * Interface for common logging methods.
     * It is expected that users of cel will supply this interface
     * and call SetLogger(logger*) at the start of the program.
     *
     * The core logging system will check the log level for you,
     * so you don't have to worry about that in all the Log() overrides.
     */
    class Logger
    {
    public:
        Logger() : m_logLevel(LogLevel::Normal) {}
        Logger(LogLevel level) : m_logLevel(level) {}

        NO_COPY(Logger)
        NO_MOVE(Logger)

    public:
        virtual void Log(LogType type, LogLevel level, const char* message) const = 0;
        virtual void Log(LogType type, LogLevel level, const char* format, va_list args) const = 0;

        void SetLogLevel(LogLevel level);
        LogLevel GetLogLevel() const { return m_logLevel; }

    protected:
        LogLevel m_logLevel;
    };
}
