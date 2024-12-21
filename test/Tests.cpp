#include <unordered_map>
#include <uv.h>
#include "App.h"
#include "Server.h"

class Client
{
public:
    Client(const cel::client_info& info) : clientInfo(info) {}

    const cel::client_info& GetInfo() { return clientInfo; };

private:
    cel::client_info clientInfo;
};

class ApiServer : public cel::Server
{
public:
    ApiServer(int port, int backlog) : cel::Server(port, backlog) {};

    void ClientConnected(uv_tcp_t* client) override
    {
        if(!client) {
            return;
        }

        if(m_clients.find(client) != m_clients.end()) {
            return;
        }

        cel::client_info info;
        if(!cel::getClientInfo(client, info)) {
            fprintf(stderr, "ClientConnected: Failed to get client info\n");
            return;
        }

        printf("Client connected: %s:%d\n", info.ip, info.port);

        m_clients.emplace(client, Client(info));
    }

    void ClientDisconnected(uv_tcp_t* client) override
    {
        auto it = m_clients.find(client);
        if(it == m_clients.end()) {
            return;
        }

        auto& client = it->second;
        auto& info = client.GetInfo();
        printf("Client disconnected: %s:%d\n", info.ip, info.port);

        m_clients.erase(it);
    }

    void ClientMessage(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) override
    {
        auto it = m_clients.find((uv_tcp_t*) client);
        if(it == m_clients.end()) {
            return;
        }

        auto& client = it->second;
        auto& info = client.GetInfo();

        printf("Incoming message from %s:%d...\n", info.ip, info.port);
        uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        printf("Echo: %s\n", wrbuf.base);
        SendMessage(client, &wrbuf);
    }

private:
    std::unordered_map<uv_tcp_t*, Client> m_clients;
};

int main()
{
    cel::App& app = cel::App::GetInstance();

    ApiServer server(8070, 128);
    app.SetServer(&server);

    app.Run();
}
