#pragma once

#include <stddef.h>
#include "ClassUtils.h"

namespace cel
{
    /**
     * Wrapper for various methods that need to allocate memory.
     */
    class MemoryManager
    {
    public:
        MemoryManager() {}
        ~MemoryManager() {}
        NO_COPY(MemoryManager)
        NO_MOVE(MemoryManager)

    public:
        virtual void* AllocUVWriteBuffer(size_t bufferStructSize) = 0;
        virtual void* AllocUVReadBuffer(size_t suggestedSize) = 0;
        virtual void* AllocUVClient(size_t bufferStructSize) = 0;
    };

    void SetMemoryManager(MemoryManager* manager);

    void* AllocUVWriteBuffer(size_t bufferStructSize);
    void* AllocUVReadBuffer(size_t suggestedSize);
    void* AllocUVClient(size_t bufferStructSize);
}
