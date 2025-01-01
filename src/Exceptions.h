#pragma once

#include <stdarg.h>
#include "Logging.h"

// Call when you want to crash because of a fatal condition failure.
#define CEL_FATAL(...) { cel::Exceptions::DoAppFatal(__VA_ARGS__); }
// Call to check a condition and then output the command (a return false; maybe?)
#define CEL_CHECK(condition, command, logLevel, ...) { if(!(condition)) { cel::Exceptions::DoCheckFailed(logLevel, __VA_ARGS__); command } }
// Call to assert a condition is true and crash if not.
#define CEL_ASSERT(condition, ...) { if(!(condition)) cel::Exceptions::DoAssertFailed(__VA_ARGS__); }

namespace cel
{
    namespace Exceptions
    {
        void DoAppFatal(const char* format, ...);
        void DoCheckFailed(LogLevel level, const char* format, ...);
        void DoAssertFailed(const char* format, ...);
    }
}
