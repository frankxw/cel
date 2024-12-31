#pragma once

#include <functional>
#include "ClassUtils.h"

namespace cel
{
    /**
     * Temporary replacement for std::experimental::scope_exit.
     * Create on the stack to have a lambda called when the object goes out of scope.
     *
     * Example:
     *   auto guard = scope_exit([]() { // do some cleanup code });
     */
    class scope_exit
    {
    public:
        scope_exit() = delete;
        scope_exit(std::function<void()> _fn) : fn(_fn) {}
        ~scope_exit() { fn(); }
        NO_COPY(scope_exit)
        NO_MOVE(scope_exit)

    private:
        std::function<void()> fn;
    };
}

using scope_exit = cel::scope_exit; // Someday this will be replaced when scope_exit is no longer experimental
