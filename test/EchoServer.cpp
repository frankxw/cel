#include <unordered_map>
#include <uv.h>
#include "App.h"
#include "Exceptions.h"
#include "Logger.h"
#include "Server.h"

class EchoServer : public cel::Server
{
public:
    EchoServer(int port) : cel::Server(port) {};

    void ClientConnected(cel::Client& client) override
    {
        cel::LogOut(cel::LogLevel::Debug, "Client connected: %s:%d\n", client.GetIp(), client.GetPort());
    }

    void ClientDisconnected(cel::Client& client) override
    {
        cel::LogOut(cel::LogLevel::Debug, "Client disconnected: %s:%d\n", client.GetIp(), client.GetPort());
    }

    void ClientMessage(cel::Client& client, ssize_t nread, const uv_buf_t* buf) override
    {
        cel::LogOut(cel::LogLevel::Debug, "Incoming message from %s:%d...\n", client.GetIp(), client.GetPort());
        uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        cel::LogOut(cel::LogLevel::Debug, "Echo: %s\n", wrbuf.base);
        client.SendMessage(&wrbuf);
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
    EchoServer server(8070);
    app.Initialize(&server, nullptr);

    app.Run();
}
