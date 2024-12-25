#include <stdlib.h>
#include "App.h"
#include "Logging.h"
#include "Server.h"

using namespace cel;

void allocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void onWrite(uv_write_t* req, int status);
void onRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
void onNewConnection(uv_stream_t* server, int status);

Server::Server(int port, int backlog)
    : m_port(port)
    , m_backlog(backlog)
{

}

Server::~Server()
{
    m_uvServer.data = nullptr;
}

void Server::SendMessage(uv_stream_t* client, uv_buf_t* wrbuf)
{
    uv_write_t* req = (uv_write_t *) malloc(sizeof(uv_write_t));
    uv_write(req, client, wrbuf, 1, onWrite);
}

void Server::Start(uv_loop_t* loop)
{
    uv_tcp_init(loop, &m_uvServer);
    m_uvServer.data = this;

    struct sockaddr_in addr;

    uv_ip4_addr("0.0.0.0", m_port, &addr);

    uv_tcp_bind(&m_uvServer, (const struct sockaddr*)&addr, 0);
    const int r = uv_listen((uv_stream_t*) &m_uvServer, m_backlog, onNewConnection);
    if(r) {
        LogErr(LogLevel::Normal, "Listen error %s\n", uv_strerror(r));
        return;
    }
}

bool cel::getClientInfo(uv_tcp_t* client, client_info& info)
{
    if(!client) {
        LogErr(LogLevel::Normal, "getClientInfo: passed in null client\n");
        return false;
    }

    struct sockaddr_storage addr;
    int nameLen = sizeof addr;

    const int getInfo = uv_tcp_getpeername(client, (struct sockaddr *)&addr, &nameLen);
    if(getInfo < 0) {
        LogErr(LogLevel::Normal, "Get client info error %s\n", uv_err_name(getInfo));
        return false;
    }

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
    buf->base = (char*) malloc(suggestedSize);
    buf->len = suggestedSize;
}

void onWrite(uv_write_t* req, int status)
{
    if(status) {
        LogErr(LogLevel::Normal, "Write error %s\n", uv_strerror(status));
    }
    free(req);
}

void onRead(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
    Server* server = static_cast<Server*>(client->data);
    if(!server) {
        LogErr(LogLevel::Normal, "onRead error: Missing Server context (client->data)\n");
        // TODO: free buf->base?
        return;
    }

    if(nread < 0) {
        if(nread != UV_EOF) {
            LogErr(LogLevel::Normal, "onRead error: %s\n", uv_err_name(nread));
        }

        server->ClientDisconnected((uv_tcp_t*) client);
        uv_close((uv_handle_t*) client, NULL);
    }
    else if(nread > 0) {
        server->ClientMessage(client, nread, buf);
    }

    if(buf->base) {
        free(buf->base);
    }
}

void onNewConnection(uv_stream_t* server, int status)
{
    if(status < 0) {
        LogErr(LogLevel::Normal, "New connection error %s\n", uv_strerror(status));
        return;
    }

    App& app = App::GetInstance();
    Server* appServer = static_cast<Server*>(server->data);
    if(!appServer) {
        LogErr(LogLevel::Normal, "onNewConnection error: Missing Server context (server->data)\n");
        return;
    }

    uv_tcp_t* client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(app.GetLoop(), client);
    client->data = appServer;
    if(uv_accept(server, (uv_stream_t*) client) != 0) {
        uv_close((uv_handle_t*) client, NULL);
        return;
    }

    appServer->ClientConnected(client);
    uv_read_start((uv_stream_t*) client, allocBuffer, onRead);
}
