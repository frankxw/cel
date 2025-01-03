#include <unordered_map>
#include <vector>
#include "App.h"
#include "Exceptions.h"
#include "Logger.h"
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

    void ClientMessage(cel::Client& client, const char* buf, size_t bufSize) override
    {
        cel::LogOut(cel::LogLevel::Debug, "Incoming message from %s:%d...\n", client.GetIp(), client.GetPort());

        SimpleMessage* msg = SimpleMessage::MakeMessage();
        msg->CopyBuffer(buf, bufSize);
        cel::LogOut(cel::LogLevel::Debug, "Echo: %s\n", msg->GetBuffer());
        client.SendMessage(*msg);
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
