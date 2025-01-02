#include <stdio.h>
#include <uv.h>
#include "App.h"
#include "Exceptions.h"
#include "Idler.h"
#include "Logger.h"
#include "Memory.h"
#include "Server.h"

class BasicServer : public cel::Server
{
public:
    BasicServer(int port) : cel::Server(port) {};

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

    void PrintTestMsg()
    {
        cel::LogOut(cel::LogLevel::Normal, "This is a test message\n");
    }

    void PrintTestMsg2()
    {
        cel::LogOut(cel::LogLevel::Normal, "This is the SECOND message\n");
    }
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

class TestMemoryManager : public cel::MemoryManager
{
public:
    TestMemoryManager() : cel::MemoryManager() {}

    void* AllocUVWriteBuffer(size_t bufferStructSize)
    {
        return malloc(bufferStructSize);
    }

    void* AllocUVReadBuffer(size_t suggestedSize)
    {
        return malloc(suggestedSize);
    }

    void* AllocUVClient(size_t bufferStructSize)
    {
        return malloc(bufferStructSize);
    }
};

int main()
{
    TestLogger logger(cel::LogLevel::Debug);
    cel::SetLogger(&logger);

    TestMemoryManager memoryManager;
    cel::SetMemoryManager(&memoryManager);

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
