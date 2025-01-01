#include <stdlib.h>
#include "App.h"
#include "Exceptions.h"
#include "Logging.h"
#include "Memory.h"
#include "ScopeGuard.h"
#include "Server.h"

using namespace cel;

constexpr int DEFAULT_SERVER_BACKLOG = 128;

void allocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void onWrite(uv_write_t* req, int status);
void onRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
void onNewConnection(uv_stream_t* server, int status);

Server::Server(int port, int backlog)
    : m_port(port)
    , m_backlog(backlog)
{

}

Server::Server(int port)
    : m_port(port)
    , m_backlog(DEFAULT_SERVER_BACKLOG)
{

}

Server::~Server()
{
    m_uvServer.data = nullptr;
}

void Server::SendMessage(uv_stream_t* client, uv_buf_t* wrbuf)
{
    uv_write_t* req = static_cast<uv_write_t*>(cel::AllocUVWriteBuffer(sizeof(uv_write_t)));
    uv_write(req, client, wrbuf, 1, onWrite);
}

void Server::HandleNewUVStreamConnection(uv_stream_t* server, int status)
{
    App& app = App::GetInstance();

    CHECK(status == 0, return;, LogLevel::Normal, "New connection error %s\n", uv_strerror(status));

    uv_tcp_t* client = static_cast<uv_tcp_t*>(cel::AllocUVClient(sizeof(uv_tcp_t)));
    uv_tcp_init(app.GetUVLoop(), client);
    client->data = this;
    if(uv_accept(server, (uv_stream_t*) client) != 0) {
        uv_close((uv_handle_t*) client, NULL);
        return;
    }

    ClientConnected(client);

    uv_read_start((uv_stream_t*) client, allocBuffer, onRead);
}

void Server::HandleUVStreamRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
    auto onExit = scope_exit{[buf]() {
        // We always want to free this
        if(buf->base) {
            free(buf->base);
        }
    }};

    if(nread < 0) {
        CHECK(nread == UV_EOF, ;, LogLevel::Normal, "onRead error: %s\n", uv_err_name(nread));
        ClientDisconnected((uv_tcp_t*) client);
        uv_close((uv_handle_t*) client, NULL);
    }
    else if(nread > 0) {
        ClientMessage(client, nread, buf);
    }
}

void Server::AllocUVReadBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf)
{
    buf->base = static_cast<char*>(cel::AllocUVReadBuffer(suggestedSize));
    buf->len = suggestedSize;
}

void Server::HandleUVStreamWrite(uv_write_t* req, int status)
{
    CHECK(status == 0, ;, LogLevel::Normal, "Write error %s\n", uv_strerror(status));
    free(req);
}

void Server::Start(uv_loop_t* loop)
{
    uv_tcp_init(loop, &m_uvServer);
    m_uvServer.data = this;

    struct sockaddr_in addr;

    uv_ip4_addr("0.0.0.0", m_port, &addr);

    uv_tcp_bind(&m_uvServer, (const struct sockaddr*)&addr, 0);
    const int r = uv_listen((uv_stream_t*) &m_uvServer, m_backlog, onNewConnection);
    CHECK(r == 0, return;, LogLevel::Normal, "Listen error %s\n", uv_strerror(r));
}

bool cel::getClientInfo(uv_tcp_t* client, client_info& info)
{
    CHECK(client != nullptr, return false;, LogLevel::Normal, "getClientInfo: passed in null client\n");

    struct sockaddr_storage addr;
    int nameLen = sizeof addr;

    const int getInfo = uv_tcp_getpeername(client, (struct sockaddr *)&addr, &nameLen);
    CHECK(getInfo >= 0, return false;, LogLevel::Normal, "Get client info error %s\n", uv_err_name(getInfo));

    if(addr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in *)&addr;
        info.port = ntohs(s->sin_port);
        uv_inet_ntop(AF_INET, &s->sin_addr, info.ip, sizeof info.ip);
    }
    else {
        struct sockaddr_in6* s = (struct sockaddr_in6 *)&addr;
        info.port = ntohs(s->sin6_port);
        uv_inet_ntop(AF_INET6, &s->sin6_addr, info.ip, sizeof info.ip);
    }

    return true;
}

void allocBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf)
{
    Server* appServer = App::GetInstance().GetServer();
    ASSERT(appServer != nullptr, "allocBuffer error: Missing global App Server\n");
    appServer->AllocUVReadBuffer(handle, suggestedSize, buf);
}

void onWrite(uv_write_t* req, int status)
{
    Server* appServer = App::GetInstance().GetServer();
    // Note: if we change the assert below to something less fatal, we'll have to deal with req not being freed.
    ASSERT(appServer != nullptr, "onWrite error: Missing Server context (client->data)\n");
    appServer->HandleUVStreamWrite(req, status);
}

void onRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
    Server* appServer = static_cast<Server*>(client->data);
    // Note: if we change the assert below to something less fatal, we'll have to deal with buf->base not being freed.
    ASSERT(appServer != nullptr, "onRead error: Missing Server context (client->data)\n");
    appServer->HandleUVStreamRead(client, nread, buf);
}

void onNewConnection(uv_stream_t* server, int status)
{
    Server* appServer = static_cast<Server*>(server->data);
    CHECK(appServer != nullptr, return;, LogLevel::Normal, "onNewConnection error: Missing Server context (server->data)\n");
    appServer->HandleNewUVStreamConnection(server, status);
}
