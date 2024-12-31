#pragma once

#include <functional>

namespace cel
{
    using TimerCallback = std::function<void()>;
    using TimerHandle = int;
}
