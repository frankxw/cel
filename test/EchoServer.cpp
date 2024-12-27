#include <unordered_map>
#include <uv.h>
#include "App.h"
#include "Exceptions.h"
#include "Logger.h"
#include "Server.h"

class Client
{
public:
    Client(const cel::client_info& info) : clientInfo(info) {}

    const cel::client_info& GetInfo() { return clientInfo; };

private:
    cel::client_info clientInfo;
};

class EchoServer : public cel::Server
{
public:
    EchoServer(int port, int backlog) : cel::Server(port, backlog) {};

    void ClientConnected(uv_tcp_t* uvClient) override
    {
        if(!uvClient) {
            return;
        }

        if(m_clients.find(uvClient) != m_clients.end()) {
            return;
        }

        cel::client_info info;
        if(!cel::getClientInfo(uvClient, info)) {
            cel::LogErr(cel::LogLevel::Normal, "ClientConnected: Failed to get client info\n");
            return;
        }

        cel::LogOut(cel::LogLevel::Debug, "Client connected: %s:%d\n", info.ip, info.port);

        m_clients.emplace(uvClient, Client(info));
    }

    void ClientDisconnected(uv_tcp_t* uvClient) override
    {
        auto it = m_clients.find(uvClient);
        if(it == m_clients.end()) {
            return;
        }

        auto& client = it->second;
        auto& info = client.GetInfo();
        cel::LogOut(cel::LogLevel::Debug, "Client disconnected: %s:%d\n", info.ip, info.port);

        m_clients.erase(it);
    }

    void ClientMessage(uv_stream_t* uvClient, ssize_t nread, const uv_buf_t* buf) override
    {
        auto it = m_clients.find((uv_tcp_t*) uvClient);
        if(it == m_clients.end()) {
            return;
        }

        auto& client = it->second;
        auto& info = client.GetInfo();

        cel::LogOut(cel::LogLevel::Debug, "Incoming message from %s:%d...\n", info.ip, info.port);
        uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        cel::LogOut(cel::LogLevel::Debug, "Echo: %s\n", wrbuf.base);
        SendMessage(uvClient, &wrbuf);
    }

private:
    std::unordered_map<uv_tcp_t*, Client> m_clients;
};

class TestLogger : public cel::Logger
{
public:
    TestLogger(cel::LogLevel level) : cel::Logger(level) {}

    void Log(cel::LogType type, cel::LogLevel level, const char* message) const override
    {
        fprintf(type == cel::LogType::Err ? stderr : stdout, "%s", message);
    }

    void Log(cel::LogType type, cel::LogLevel level, const char* format, va_list args) const override
    {
        vfprintf(type == cel::LogType::Err ? stderr : stdout, format, args);
    }
};

int main()
{
    TestLogger logger(cel::LogLevel::Debug);
    cel::SetLogger(&logger);

    cel::App& app = cel::App::GetInstance();
    EchoServer server(8070, 128);
    app.Initialize(&server, nullptr);

    app.Run();
}
