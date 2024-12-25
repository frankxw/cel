#include <stdarg.h>
#include "Logger.h"
#include "Logging.h"

#include <stdio.h>

using namespace cel;

static Logger* s_logger = nullptr;

void cel::SetLogger(Logger* logger)
{
    s_logger = logger;
}

void cel::Log(LogType type, LogLevel level, const char* message)
{
    if(!s_logger)
        return;
    s_logger->Log(type, level, message);
}

void cel::Log(LogType type, LogLevel level, const char* format, va_list args)
{
    if(!s_logger)
        return;
    s_logger->Log(type, level, format, args);
}

void cel::Log(LogType type, LogLevel level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Log(type, level, format, args);
    va_end(args);
}

void cel::LogOut(LogLevel level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Log(LogType::Out, level, format, args);
    va_end(args);
}

void cel::LogErr(LogLevel level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Log(LogType::Err, level, format, args);
    va_end(args);
}
