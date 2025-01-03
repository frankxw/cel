#pragma once

#include <uv.h>
#include "ClassUtils.h"

namespace cel
{
    class Message;

    /**
     * Represents an active connection to the server.
     */
    class Client
    {
    friend class Server;

    public:
        Client();

    public:
        const char* GetIp() const { return m_ip; }
        int GetPort() const { return m_port; }

        void SendMessage(Message& message);

    private:
        bool Initialize(uv_tcp_t* uvClient);

    private:
        uv_tcp_t* m_uvClient;
        char m_ip[INET6_ADDRSTRLEN];
        int m_port;
    };
}
