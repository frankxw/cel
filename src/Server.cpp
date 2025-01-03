#include <stdlib.h>
#include "App.h"
#include "Exceptions.h"
#include "Logging.h"
#include "Memory.h"
#include "Message.h"
#include "ScopeGuard.h"
#include "Server.h"

using namespace cel;

constexpr int DEFAULT_SERVER_BACKLOG = 128;

static void allocBuffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
static void onWrite(uv_write_t* req, int status);
static void onRead(uv_stream_t* uvClient, ssize_t nread, const uv_buf_t* buf);
static void onNewConnection(uv_stream_t* server, int status);

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

void Server::HandleNewUVStreamConnection(uv_stream_t* server, int status)
{
    App& app = App::GetInstance();

    CEL_CHECK(status == 0, return;, LogLevel::Normal, "New connection error %s\n", uv_strerror(status));

    uv_tcp_t* uvClient = static_cast<uv_tcp_t*>(cel::AllocUVClient(sizeof(uv_tcp_t)));
    uv_tcp_init(app.GetUVLoop(), uvClient);
    uvClient->data = this;
    if(uv_accept(server, (uv_stream_t*) uvClient) != 0) {
        uv_close((uv_handle_t*) uvClient, NULL);
        return;
    }

    if(!MakeNewClient(uvClient)) {
        LogErr(LogLevel::Normal, "HandleNewUVStreamConnection: failed to create new client object.\n");
        uv_close((uv_handle_t*) uvClient, NULL);
        return;
    }

    Client* client = GetClient(uvClient);
    CEL_ASSERT(client != nullptr, "Could not find client that was just created.\n");
    ClientConnected(*client);

    uv_read_start((uv_stream_t*) uvClient, allocBuffer, onRead);
}

void Server::HandleUVStreamRead(uv_stream_t* uvClient, ssize_t nread, const uv_buf_t* buf)
{
    auto onExit = scope_exit{[buf]() {
        // We must always free this (see https://docs.libuv.org/en/v1.x/stream.html#c.uv_read_cb)
        if(buf->base) {
            cel::FreeUVReadBuffer(buf->base);
        }
    }};

    Client* client = GetClient((uv_tcp_t*) uvClient);
    CEL_CHECK(client != nullptr, return;, LogLevel::Normal, "onRead error: Missing client for uvClient %p\n", uvClient);

    if(nread < 0) {
        CEL_CHECK(nread == UV_EOF, ;, LogLevel::Normal, "onRead error: %s\n", uv_err_name(nread));
        ClientDisconnected(*client);
        RemoveClient((uv_tcp_t*) uvClient);
    }
    else if(nread > 0) {
        ClientMessage(*client, buf->base, static_cast<size_t>(nread));
    }
}

void Server::AllocUVReadBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf)
{
    buf->base = static_cast<char*>(cel::AllocUVReadBuffer(suggestedSize));
    buf->len = suggestedSize;
}

void Server::HandleUVStreamWrite(uv_write_t* req, int status)
{
    CEL_ASSERT(req != nullptr, "HandleUVStreamWrite error: null req\n");
    CEL_CHECK(status == 0, ;, LogLevel::Normal, "Write error %s\n", uv_strerror(status));

    write_req_t* writeReq = (write_req_t*) req;
    CEL_ASSERT(writeReq->m_message != nullptr, "HandleUVStreamWrite error: req has a null message\n");
    writeReq->m_message->Destroy();
    cel::FreeUVWriteBuffer(writeReq);
}

void Server::Start(uv_loop_t* loop)
{
    uv_tcp_init(loop, &m_uvServer);
    m_uvServer.data = this;

    struct sockaddr_in addr;

    uv_ip4_addr("0.0.0.0", m_port, &addr);

    uv_tcp_bind(&m_uvServer, (const struct sockaddr*)&addr, 0);
    const int r = uv_listen((uv_stream_t*) &m_uvServer, m_backlog, onNewConnection);
    CEL_CHECK(r == 0, return;, LogLevel::Normal, "Listen error %s\n", uv_strerror(r));
}

bool Server::MakeNewClient(uv_tcp_t* uvClient)
{
    CEL_CHECK(m_clients.find(uvClient) == m_clients.end(), return false;, LogLevel::Normal, "Trying to create duplicate client object for uvClient %p\n", uvClient);

    m_clients[uvClient] = {};
    if(!m_clients[uvClient].Initialize(uvClient)) {
        LogErr(LogLevel::Normal, "MakeNewClient: failed to initialize new client object.\n");
        m_clients.erase(uvClient);
        return false;
    }

    return true;
}

Client* Server::GetClient(uv_tcp_t* uvClient)
{
    auto it = m_clients.find(uvClient);
    if(it == m_clients.end())
        return nullptr;

    return &it->second;
}

void Server::RemoveClient(uv_tcp_t* uvClient)
{
    CEL_CHECK(uvClient != nullptr, return;, LogLevel::Normal, "Trying to remove a null client.\n");
    uv_close((uv_handle_t*) uvClient, NULL);
    m_clients.erase(uvClient);
}

void Server::SendMessage(uv_stream_t* uvClient, Message& message)
{
    write_req_t* req = static_cast<write_req_t*>(cel::AllocUVWriteBuffer(sizeof(write_req_t)));
    req->m_buffer = uv_buf_init(message.GetBuffer(), message.GetBufferSize());
    req->m_message = &message;
    uv_write((uv_write_t*) req, uvClient, &req->m_buffer, 1, onWrite);
}

static void allocBuffer(uv_handle_t* handle, size_t suggestedSize, uv_buf_t* buf)
{
    Server* appServer = App::GetInstance().GetServer();
    CEL_ASSERT(appServer != nullptr, "allocBuffer error: Missing global App Server\n");
    appServer->AllocUVReadBuffer(handle, suggestedSize, buf);
}

static void onWrite(uv_write_t* req, int status)
{
    Server* appServer = App::GetInstance().GetServer();
    // Note: if we change the assert below to something less fatal, we'll have to deal with req not being freed.
    CEL_ASSERT(appServer != nullptr, "onWrite error: Missing Server context (client->data)\n");
    appServer->HandleUVStreamWrite(req, status);
}

static void onRead(uv_stream_t* uvClient, ssize_t nread, const uv_buf_t* buf)
{
    Server* appServer = static_cast<Server*>(uvClient->data);
    // Note: if we change the assert below to something less fatal, we'll have to deal with buf->base not being freed.
    CEL_ASSERT(appServer != nullptr, "onRead error: Missing Server context (client->data)\n");
    appServer->HandleUVStreamRead(uvClient, nread, buf);
}

static void onNewConnection(uv_stream_t* server, int status)
{
    Server* appServer = static_cast<Server*>(server->data);
    CEL_CHECK(appServer != nullptr, return;, LogLevel::Normal, "onNewConnection error: Missing Server context (server->data)\n");
    appServer->HandleNewUVStreamConnection(server, status);
}
