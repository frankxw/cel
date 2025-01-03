#pragma once

#include <stddef.h>

namespace cel
{
    /**
     * Wrapper for messages that will be sent over the network.
     * For the most part a subclass needs to supply the raw buffer and size.
     * Subclasses are also in charge of the memory management of the buffer.
     */
    class Message
    {
    public:
        Message() {};
        virtual ~Message() {};

    public:
        virtual char* GetBuffer() = 0;
        virtual unsigned int GetBufferSize() = 0;

        virtual void Destroy() = 0;
    };
}
