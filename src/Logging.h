#pragma once

namespace cel
{
    enum class LogLevel
    {
        Debug   = 1,
        Normal  = 2,
        Strict  = 3,
    };

    enum class LogType
    {
        Out,
        Err,
    };

    void SetLogger(class Logger* logger);

    void Log(LogType type, LogLevel level, const char* message);
    void Log(LogType type, LogLevel level, const char* format, va_list args);
    void Log(LogType type, LogLevel level, const char* format, ...);
    void LogOut(LogLevel level, const char* format, ...);
    void LogErr(LogLevel level, const char* format, ...);
}
