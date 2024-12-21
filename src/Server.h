#pragma once

#include <uv.h>

namespace cel
{
    class App;

    using client_info = struct client_info {
        char ip[INET6_ADDRSTRLEN];
        int port;
    };

    /**
     * Main interface for networking.
     * Make a subclass and implement the virtual methods to be
     * notified of new connections.
     */
    class Server
    {
    friend class App;

    public:
        Server() = delete;
        Server(int port, int backlog);

        int GetPort() { return m_port; };
        int GetBacklog() { return m_backlog; };

        virtual void ClientConnected(uv_tcp_t* client) {};
        virtual void ClientDisconnected(uv_tcp_t* client) {};
        virtual void ClientMessage(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {};
        void SendMessage(uv_stream_t* client, uv_buf_t* wrbuf);

    protected:
        ~Server();

        int m_port;
        int m_backlog;
        uv_tcp_t m_uvServer;

    private:
        void Start(uv_loop_t* loop);
    };

    bool getClientInfo(uv_tcp_t* client, client_info& info);
}
