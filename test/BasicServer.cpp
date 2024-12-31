#include <stdio.h>
#include <uv.h>
#include "App.h"
#include "Exceptions.h"
#include "Idler.h"
#include "Logger.h"
#include "Server.h"

using Client = struct Client
{
    cel::client_info info;
};

using ClientMap = std::unordered_map<uv_tcp_t*, Client>;

class BasicServer : public cel::Server
{
public:
    BasicServer(int port) : cel::Server(port) {};

    void ClientConnected(uv_tcp_t* uvClient)
    {
        if(!uvClient) {
            return;
        }

        if(m_clients.find(uvClient) != m_clients.end()) {
            return;
        }

        Client client;
        if(!cel::getClientInfo(uvClient, client.info)) {
            fprintf(stderr, "ClientConnected: Failed to get client info\n");
            return;
        }

        m_clients[uvClient] = client;

        cel::LogOut(cel::LogLevel::Debug, "Client connected: %s:%d\n", client.info.ip, client.info.port);
    };

    void ClientDisconnected(uv_tcp_t* uvClient)
    {
        auto client = m_clients.find(uvClient);
        if(client == m_clients.end()) {
            return;
        }

        cel::LogOut(cel::LogLevel::Debug, "Client disconnected: %s:%d\n", client->second.info.ip, client->second.info.port);
        m_clients.erase(client);
    }

    void ClientMessage(uv_stream_t* uvClient, ssize_t nread, const uv_buf_t* buf)
    {
        auto client = m_clients.find((uv_tcp_t*) uvClient);
        if(client == m_clients.end()) {
            return;
        }

        cel::LogOut(cel::LogLevel::Debug, "Incoming message from %s:%d...\n", client->second.info.ip, client->second.info.port);
        uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        cel::LogOut(cel::LogLevel::Debug, "Echo: %s\n", wrbuf.base);
        SendMessage(uvClient, &wrbuf);
    };

    void PrintTestMsg()
    {
        cel::LogOut(cel::LogLevel::Normal, "This is a test message\n");
    }

    void PrintTestMsg2()
    {
        cel::LogOut(cel::LogLevel::Normal, "This is the SECOND message\n");
    }

private:
    ClientMap m_clients;
};

class Cruncher : public cel::Idler
{
public:
    void Execute()
    {
        static int a = 0;
        if(a++ < 300) {
            printf("Idle :) %d\n", a);
        }
    }
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

    BasicServer server(8070);
    Cruncher crunch;

    app.Initialize(&server, &crunch);

    app.SetTimeout(3'000, [&server]() {
        server.PrintTestMsg();
    });

    app.SetTimeout(10'000, [&app]() {
        cel::LogOut(cel::LogLevel::Normal, "Quitting\n");
        app.Quit();
    });

    app.SetInterval(8'000, 2'000, [&server]() {
        server.PrintTestMsg2();
    });

    app.Run();
    cel::LogOut(cel::LogLevel::Debug, "End\n");
}
