#include <stdio.h>
#include <vector>
#include "App.h"
#include "Exceptions.h"
#include "Idler.h"
#include "Logger.h"
#include "Memory.h"
#include "Message.h"
#include "Server.h"

class SimpleMessage : public cel::Message
{
public:
    SimpleMessage() : cel::Message(), m_buffer(nullptr), m_bufferSize(0) {}
    ~SimpleMessage()
    {
        if(m_buffer)
            free(m_buffer);
    }

public:
    void CopyBuffer(const char* other, size_t len)
    {
        if(m_buffer != nullptr) {
            cel::LogErr(cel::LogLevel::Normal, "Failed to copy buffer because our buffer is already created.\n");
            return;
        }

        m_buffer = (char*) malloc(len);
        std::memcpy(m_buffer, other, len);
        m_bufferSize = len;
    }

    char* GetBuffer() override
    {
        return m_buffer;
    }

    unsigned int GetBufferSize() override
    {
        return m_bufferSize;
    }

    void Destroy() override
    {
        delete this;
    }

    static SimpleMessage* MakeMessage()
    {
        SimpleMessage* newMsg = new SimpleMessage();
        return newMsg;
    }

private:
    char* m_buffer;
    unsigned int m_bufferSize;
};

class BasicServer : public cel::Server
{
public:
    BasicServer(int port) : cel::Server(port) {};

    void ClientConnected(cel::Client& client) override
    {
        cel::LogOut(cel::LogLevel::Debug, "Client connected: %s:%d\n", client.GetIp(), client.GetPort());

        constexpr char welcomeMsg[] = "Welcome to the BasicServer!\n";
        SimpleMessage* msg = SimpleMessage::MakeMessage();
        msg->CopyBuffer(welcomeMsg, sizeof(welcomeMsg));
        client.SendMessage(*msg);
    }

    void ClientDisconnected(cel::Client& client) override
    {
        cel::LogOut(cel::LogLevel::Debug, "Client disconnected: %s:%d\n", client.GetIp(), client.GetPort());
    }

    void ClientMessage(cel::Client& client, const char* buf, size_t bufSize) override
    {
        cel::LogOut(cel::LogLevel::Debug, "Incoming message from %s:%d...\n", client.GetIp(), client.GetPort());

        SimpleMessage* msg = SimpleMessage::MakeMessage();
        msg->CopyBuffer(buf, bufSize);
        cel::LogOut(cel::LogLevel::Debug, "Echo: %s\n", msg->GetBuffer());
        client.SendMessage(*msg);
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
    void Execute() override
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

    void* AllocUVWriteBuffer(size_t bufferStructSize) override
    {
        return malloc(bufferStructSize);
    }

    void FreeUVWriteBuffer(cel::write_req_t* buffer) override
    {
        free(buffer);
    }

    void* AllocUVReadBuffer(size_t suggestedSize) override
    {
        return malloc(suggestedSize);
    }

    void FreeUVReadBuffer(char* buffer) override
    {
        free(buffer);
    }

    void* AllocUVClient(size_t bufferStructSize) override
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
