#pragma once

#include <stdint.h>

namespace cel
{
    enum class LogLevel : uint8_t
    {
        Debug   = 1,
        Normal  = 2,
        Strict  = 3,

        HIGHEST = static_cast<uint8_t>(-1)
    };

    enum class LogType : uint8_t
    {
        Out,
        Err,
    };

    // Should be called before anything else to hook up a global custom logger at program startup.
    // By default there is no logger hooked up.
    void SetLogger(class Logger* logger);

    // Global methods for logging (they are global so they can be used in random callbacks and whatnot).
    void Log(LogType type, LogLevel level, const char* message);
    void Log(LogType type, LogLevel level, const char* format, va_list args);
    void Log(LogType type, LogLevel level, const char* format, ...);
    void LogOut(LogLevel level, const char* format, ...);
    void LogErr(LogLevel level, const char* format, ...);
}
