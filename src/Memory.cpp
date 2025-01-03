#include <stdlib.h>
#include "Exceptions.h"
#include "Memory.h"
#include "Server.h"

using namespace cel;

static MemoryManager* s_memoryManager;

void cel::SetMemoryManager(MemoryManager* manager)
{
    s_memoryManager = manager;
}

void* cel::AllocUVWriteBuffer(size_t bufferStructSize)
{
    CEL_ASSERT(bufferStructSize > 0, "AllocUVWriteBuffer: Invalid alloc size.\n");

    if(s_memoryManager) {
        return s_memoryManager->AllocUVWriteBuffer(bufferStructSize);
    }

    return malloc(bufferStructSize);
}

void cel::FreeUVWriteBuffer(write_req_t* buffer)
{
    if(s_memoryManager) {
        s_memoryManager->FreeUVWriteBuffer(buffer);
        return;
    }

    free(buffer);
}

void* cel::AllocUVReadBuffer(size_t suggestedSize)
{
    CEL_ASSERT(suggestedSize > 0, "AllocUVReadBuffer: Invalid alloc size.\n");

    if(s_memoryManager) {
        return s_memoryManager->AllocUVReadBuffer(suggestedSize);
    }

    return malloc(suggestedSize);
}

void cel::FreeUVReadBuffer(char* buffer)
{
    if(s_memoryManager) {
        s_memoryManager->FreeUVReadBuffer(buffer);
        return;
    }

    free(buffer);
}

void* cel::AllocUVClient(size_t bufferStructSize)
{
    CEL_ASSERT(bufferStructSize > 0, "AllocUVClient: Invalid alloc size.\n");

    if(s_memoryManager) {
        return s_memoryManager->AllocUVClient(bufferStructSize);
    }

    return malloc(bufferStructSize);
}
