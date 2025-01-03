#pragma once

#include <stddef.h>
#include "ClassUtils.h"

namespace cel
{
    struct write_req_t;

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
        virtual void FreeUVWriteBuffer(write_req_t* buffer) = 0;
        virtual void* AllocUVReadBuffer(size_t suggestedSize) = 0;
        virtual void FreeUVReadBuffer(char* buffer) = 0;
        virtual void* AllocUVClient(size_t bufferStructSize) = 0;
    };

    void SetMemoryManager(MemoryManager* manager);

    void* AllocUVWriteBuffer(size_t bufferStructSize);
    void FreeUVWriteBuffer(write_req_t* buffer);
    void* AllocUVReadBuffer(size_t suggestedSize);
    void FreeUVReadBuffer(char* buffer);
    void* AllocUVClient(size_t bufferStructSize);
}
