#pragma once

#include <unordered_map>
#include <uv.h>
#include "Client.h"

namespace cel
{
    class App;

    /**
     * Main interface for networking.
     * Make a subclass and implement the virtual methods to be
     * notified of new connections.
     */
    class Server
    {
    friend class App;
    friend class Client;

    public:
        Server() = delete;
        Server(int port);
        Server(int port, int backlog);
        NO_COPY(Server)
        NO_MOVE(Server)
    protected:
        ~Server();

    public:
        int GetPort() { return m_port; };
        int GetBacklog() { return m_backlog; };

        virtual void ClientConnected(Client& client) {};
        virtual void ClientDisconnected(Client& client) {};
        virtual void ClientMessage(Client& client, ssize_t nread, const uv_buf_t* buf) {};

        void HandleNewUVStreamConnection(uv_stream_t* server, int status);
        void HandleUVStreamRead(uv_stream_t* uvClient, ssize_t nread, const uv_buf_t* buf);
        void AllocUVReadBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf);
        void HandleUVStreamWrite(uv_write_t* req, int status);

    private:
        // Should only be called by App
        void Start(uv_loop_t* loop);

        bool MakeNewClient(uv_tcp_t* uvClient);
        Client* GetClient(uv_tcp_t* uvClient);
        void RemoveClient(uv_tcp_t* uvClient);

        void SendMessage(uv_stream_t* uvClient, uv_buf_t* wrbuf);

    protected:
        int m_port;
        int m_backlog;
        uv_tcp_t m_uvServer;
        std::unordered_map<uv_tcp_t*, Client> m_clients;
    };
}
