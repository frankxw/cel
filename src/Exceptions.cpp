#include "Exceptions.h"
#include "Logging.h"

using namespace cel;

void cel::Exceptions::DoAppFatal(const char* format, ...)
{
    // TODO: for now just logging but set up some kind of crash handler
    va_list args;
    va_start(args, format);
    Log(LogType::Err, LogLevel::HIGHEST, format, args);
    va_end(args);
}

void cel::Exceptions::DoCheckFailed(LogLevel level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Log(LogType::Err, level, format, args);
    va_end(args);
}

void cel::Exceptions::DoAssertFailed(const char* format, ...)
{
    // TODO: for now just logging but set up some kind of crash handler
    va_list args;
    va_start(args, format);
    Log(LogType::Err, LogLevel::HIGHEST, format, args);
    va_end(args);
}
