#include <cstring>
#include "App.h"
#include "Client.h"
#include "Exceptions.h"
#include "Server.h"

using namespace cel;

Client::Client()
    : m_uvClient(nullptr)
    , m_port(0)
{
    std::memset(m_ip, 0, INET6_ADDRSTRLEN);
}

bool Client::Initialize(uv_tcp_t* uvClient)
{
    CEL_CHECK(uvClient != nullptr, return false;, LogLevel::Normal, "getClientInfo: passed in null client\n");
    m_uvClient = uvClient;

    struct sockaddr_storage addr;
    int nameLen = sizeof(addr);

    const int getInfo = uv_tcp_getpeername(uvClient, (struct sockaddr *)&addr, &nameLen);
    CEL_CHECK(getInfo >= 0, return false;, LogLevel::Normal, "Get client info error %s\n", uv_err_name(getInfo));

    if(addr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in *)&addr;
        m_port = ntohs(s->sin_port);
        uv_inet_ntop(AF_INET, &s->sin_addr, m_ip, sizeof(m_ip));
    }
    else {
        struct sockaddr_in6* s = (struct sockaddr_in6 *)&addr;
        m_port = ntohs(s->sin6_port);
        uv_inet_ntop(AF_INET6, &s->sin6_addr, m_ip, sizeof(m_ip));
    }

    return true;
}

void Client::SendMessage(uv_buf_t* wrbuf)
{
    Server* appServer = App::GetInstance().GetServer();
    CEL_ASSERT(appServer != nullptr, "SendMessage error: Missing global App Server\n");
    appServer->SendMessage((uv_stream_t*) m_uvClient, wrbuf);
}
