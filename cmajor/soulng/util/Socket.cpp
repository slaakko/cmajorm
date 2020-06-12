// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Socket.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Unicode.hpp>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <unordered_map>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>    
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#define SOCKET int
#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#define INVALID_SOCKET -1
#endif

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

namespace soulng { namespace util {

using namespace soulng::unicode;

struct SocketData
{
    SocketData() : socket(INVALID_SOCKET)  {}
    SocketData(SOCKET socket_) : socket(socket_) {}
    SOCKET socket;
};

class SocketTable
{
public:
    static void Init();
    static void Done();
    static SocketTable& Instance() { Assert(instance, "socket table not initialized"); return *instance; }
    ~SocketTable();
    int32_t CreateSocket();
    void BindSocket(int32_t socketHandle, int32_t port);
    void ListenSocket(int32_t socketHandle, int32_t backlog);
    int32_t AcceptSocket(int32_t socketHandle);
    void CloseSocket(int32_t socketHandle);
    void ShutdownSocket(int32_t socketHandle, ShutdownMode mode);
    int32_t ConnectSocket(const std::string& node, const std::string& service);
    int32_t SendSocket(int32_t socketHandle, const uint8_t* buf, int32_t len, int32_t flags);
    int32_t ReceiveSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags);
private:
    static std::unique_ptr<SocketTable> instance;
    const int32_t maxNoLockSocketHandles = 256;
    std::vector<std::unique_ptr<SocketData>> sockets;
    std::unordered_map<int32_t, std::unique_ptr<SocketData>> socketMap;
    std::atomic<int32_t> nextSocketHandle;
    std::mutex mtx;
    SocketTable();
};

std::unique_ptr<SocketTable> SocketTable::instance;

void SocketTable::Init()
{
    instance.reset(new SocketTable());
}

void SocketTable::Done()
{
    instance.reset();
}

#ifdef _WIN32

std::string GetSocketErrorMessage(int errorCode)
{
    char16_t buf[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, (LPWSTR)(&buf[0]), sizeof(buf), NULL);
    return ToUtf8(std::u16string(buf));
}

int GetLastSocketError()
{
    return WSAGetLastError();
}

#else

std::string GetSocketErrorMessage(int errorCode)
{
    return strerror(errorCode);
}

int GetLastSocketError()
{
    return errno;
}

#endif

SocketTable::SocketTable() : nextSocketHandle(1)
{
    sockets.resize(maxNoLockSocketHandles);
#ifdef _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(ver, &wsaData) != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = "socket initialization failed with error code " + std::to_string(errorCode) + ": " + GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
#endif
}

SocketTable::~SocketTable()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

int32_t SocketTable::CreateSocket()
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
    int32_t socketHandle = nextSocketHandle++;
    if (socketHandle < maxNoLockSocketHandles)
    {
        sockets[socketHandle] = std::unique_ptr<SocketData>(new SocketData(s));
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        socketMap[socketHandle] = std::unique_ptr<SocketData>(new SocketData(s));
    }
    return socketHandle;
}

void SocketTable::BindSocket(int32_t socketHandle, int32_t port)
{
    int result = 0;
    if (socketHandle <= 0)
    {
        throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        if (!sockets[socketHandle])
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = sockets[socketHandle]->socket;
        if (s == INVALID_SOCKET)
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(port);
        result = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            if (!it->second)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = it->second->socket;
            if (s == INVALID_SOCKET)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            addr.sin_port = htons(port);
            result = bind(s, (struct sockaddr*)&addr, sizeof(addr));
        }
        else
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

void SocketTable::ListenSocket(int32_t socketHandle, int32_t backlog)
{
    int result = 0;
    if (socketHandle <= 0)
    {
        throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        if (!sockets[socketHandle])
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = sockets[socketHandle]->socket;
        if (s == INVALID_SOCKET)
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        result = listen(s, backlog);
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            if (!it->second)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = it->second->socket;
            if (s == INVALID_SOCKET)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            result = listen(s, backlog);
        }
        else
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

int32_t SocketTable::AcceptSocket(int32_t socketHandle)
{
    SOCKET a = 0;
    if (socketHandle <= 0)
    {
        throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        if (!sockets[socketHandle])
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = sockets[socketHandle]->socket;
        if (s == INVALID_SOCKET)
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        a = accept(s, NULL, NULL);
        if (a == INVALID_SOCKET)
        {
            int errorCode = GetLastSocketError();
            std::string errorMessage = GetSocketErrorMessage(errorCode);
            throw std::runtime_error(errorMessage);
        }
        int32_t acceptedSocketHandle = nextSocketHandle++;
        if (acceptedSocketHandle < maxNoLockSocketHandles)
        {
            sockets[acceptedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(a));
        }
        else
        {
            std::lock_guard<std::mutex> lock(mtx);
            socketMap[acceptedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(a));
        }
        return acceptedSocketHandle;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            if (!it->second)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = it->second->socket;
            if (s == INVALID_SOCKET)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            a = accept(s, NULL, NULL);
            if (a == INVALID_SOCKET)
            {
                int errorCode = GetLastSocketError();
                std::string errorMessage = GetSocketErrorMessage(errorCode);
                throw std::runtime_error(errorMessage);
            }
            int32_t acceptedSocketHandle = nextSocketHandle++;
            if (acceptedSocketHandle < maxNoLockSocketHandles)
            {
                sockets[acceptedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(a));
            }
            else
            {
                socketMap[acceptedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(a));
            }
            return acceptedSocketHandle;
        }
        else
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
    }
}

void SocketTable::CloseSocket(int32_t socketHandle)
{
    int result = 0;
    if (socketHandle <= 0)
    {
        throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = socketData->socket;
#ifdef _WIN32
        result = closesocket(s);
#else
        result = close(s);
#endif
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            SocketData* socketData = it->second.get();
            if (!socketData)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = socketData->socket;
#ifdef _WIN32
            result = closesocket(s);
#else
            result = close(s);
#endif
        }
        else
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

void SocketTable::ShutdownSocket(int32_t socketHandle, ShutdownMode mode)
{
    int result = 0;
    int how = SD_RECEIVE;
    switch (mode)
    {
        case ShutdownMode::receive: how = SD_RECEIVE; break;
        case ShutdownMode::send: how = SD_SEND; break;
        case ShutdownMode::both: how = SD_BOTH; break;
    }
    if (socketHandle <= 0)
    {
        throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = socketData->socket;
        result = shutdown(s, how);
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            SocketData* socketData = it->second.get();
            if (!socketData)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = socketData->socket;
            result = shutdown(s, how);
        }
        else
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

int32_t SocketTable::ConnectSocket(const std::string& node, const std::string& service)
{
    std::unique_ptr<SocketData> socketData(new SocketData());
    struct addrinfo hint;
    struct addrinfo* rp;
    struct addrinfo* res;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_flags = 0;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;
    hint.ai_addrlen = 0;
    hint.ai_addr = 0;
    hint.ai_canonname = 0;
    hint.ai_next = 0;
    int result = getaddrinfo(node.c_str(), service.c_str(), &hint, &res);
    if (result != 0)
    {
#ifdef _WIN32
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
#else
        std::string errorMessage = gai_strerror(result);
#endif
        throw std::runtime_error(errorMessage);
    }
    else
    {
        for (rp = res; rp != 0; rp = rp->ai_next)
        {
            SOCKET s = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (s == -1)
            {
                continue;
            }
            int result = connect(s, rp->ai_addr, (int)rp->ai_addrlen);
            if (result == 0)
            {
                freeaddrinfo(res);
                int32_t connectedSocketHandle = nextSocketHandle++;
                if (connectedSocketHandle < maxNoLockSocketHandles)
                {
                    sockets[connectedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(s));
                }
                else
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    socketMap[connectedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(s));
                }
                return connectedSocketHandle;
            }
            else
            {
                freeaddrinfo(res);
                int errorCode = GetLastSocketError();
                std::string errorMessage = GetSocketErrorMessage(errorCode);
                throw std::runtime_error(errorMessage);
            }
        }
    }
    std::string errorMessage = "could not connect to " + node + ":" + service;
    throw std::runtime_error(errorMessage);
}

int32_t SocketTable::SendSocket(int32_t socketHandle, const uint8_t* buf, int32_t len, int32_t flags)
{
    int32_t result = 0;
    if (socketHandle <= 0)
    {
        throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = socketData->socket;
        result = send(s, (const char*)buf, len, flags);
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            SocketData* socketData = it->second.get();
            if (!socketData)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = socketData->socket;
            result = send(s, (const char*)buf, len, flags);
        }
        else
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result < 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
    return result;
}

int32_t SocketTable::ReceiveSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags)
{
    int32_t result = 0;
    if (socketHandle <= 0)
    {
        throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = socketData->socket;
        result = recv(s, (char*)buf, len, flags);
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            SocketData* socketData = it->second.get();
            if (!socketData)
            {
                throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = socketData->socket;
            result = recv(s, (char*)buf, len, flags);
        }
        else
        {
            throw std::runtime_error("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result < 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
    return result;
}

void InitSocket()
{
    SocketTable::Init();
}

void DoneSocket()
{
    SocketTable::Done();
}

int32_t CreateSocket()
{
    return SocketTable::Instance().CreateSocket();
}

void BindSocket(int32_t socketHandle, int32_t port)
{
    SocketTable::Instance().BindSocket(socketHandle, port);
}

void ListenSocket(int32_t socketHandle, int32_t backLog)
{
    SocketTable::Instance().ListenSocket(socketHandle, backLog);
}

int32_t AcceptSocket(int32_t socketHandle)
{
    return SocketTable::Instance().AcceptSocket(socketHandle);
}

void CloseSocket(int32_t socketHandle)
{
    SocketTable::Instance().CloseSocket(socketHandle);
}

void ShutdownSocket(int32_t socketHandle, ShutdownMode mode)
{
    SocketTable::Instance().ShutdownSocket(socketHandle, mode);
}

int32_t ConnectSocket(const char* node, const char* service)
{
    return SocketTable::Instance().ConnectSocket(node, service);
}

int32_t SendSocket(int32_t socketHandle, const uint8_t* buf, int32_t len, int32_t flags)
{
    return SocketTable::Instance().SendSocket(socketHandle, buf, len, flags);
}

int32_t ReceiveSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags)
{
    return SocketTable::Instance().ReceiveSocket(socketHandle, buf, len, flags);
}

TcpSocket::TcpSocket() : handle(CreateSocket()), connected(false), shutdown(false)
{
}

TcpSocket::TcpSocket(const std::string& node, const std::string& service) : handle(ConnectSocket(node.c_str(), service.c_str())), connected(true), shutdown(false)
{
}

TcpSocket::TcpSocket(int handle_) noexcept : handle(handle_), connected(true), shutdown(false)
{
}

TcpSocket::TcpSocket(TcpSocket&& that) noexcept : handle(that.handle), connected(that.connected), shutdown(that.shutdown) 
{
    that.handle = -1;
    that.connected = false;
    that.shutdown = false;
}

TcpSocket& TcpSocket::operator=(TcpSocket&& that) noexcept
{
    std::swap(handle, that.handle);
    std::swap(connected, that.connected);
    std::swap(shutdown, that.shutdown);
    return *this;
}

TcpSocket::~TcpSocket()
{
    if (handle > 0)
    {
        if (connected && !shutdown)
        {
            try
            {
                Shutdown(ShutdownMode::both);
            }
            catch (...)
            {
            }
        }
        try
        {
            CloseSocket(handle);
        }
        catch (...)
        {
        }
    }
}

void TcpSocket::Close()
{
    if (handle > 0)
    {
        if (connected && !shutdown)
        {
            Shutdown(ShutdownMode::both);
        }
        CloseSocket(handle);
        handle = -1;
        connected = false;
    }
}

void TcpSocket::Connect(const std::string& node, const std::string& service)
{
    Close();
    handle = ConnectSocket(node.c_str(), service.c_str());
    connected = true;
}

void TcpSocket::Bind(int port)
{
    BindSocket(handle, port);
}

void TcpSocket::Listen(int backlog)
{
    ListenSocket(handle, backlog);
}

TcpSocket TcpSocket::Accept()
{
    int acceptedHandle = AcceptSocket(handle);
    return TcpSocket(acceptedHandle);
}

void TcpSocket::Shutdown(ShutdownMode mode)
{
    shutdown = true;
    ShutdownSocket(handle, mode);
}

void TcpSocket::Send(const uint8_t* buffer, int count)
{
    int offset = 0;
    int bytesToSend = count;
    while (bytesToSend > 0)
    {
        int result = SendSocket(handle, buffer + offset, count, 0);
        if (result >= 0)
        {
            bytesToSend = bytesToSend - result;
            offset = offset + result;
        }
    }
}

int TcpSocket::Receive(uint8_t* buffer, int count)
{
    int result = ReceiveSocket(handle, buffer, count, 0);
    return result;
}

} } // namespace soulng::util
