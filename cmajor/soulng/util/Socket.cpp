// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Socket.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/MemoryReader.hpp>
#include <soulng/util/MemoryWriter.hpp>
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

#ifdef _WIN32

std::string GetSocketErrorMessage(int errorCode)
{
    char16_t buf[2048];
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, (LPWSTR)(&buf[0]), sizeof(buf) / 2, NULL);
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

class Sockets
{
public:
    static void Init();
    static void Done();
};

void Sockets::Init()
{
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

void Sockets::Done()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

int64_t CreateSocket()
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
    return static_cast<int64_t>(s);
}

void BindSocket(int64_t socketHandle, int port)
{
    int result = 0;
    SOCKET s = static_cast<SOCKET>(socketHandle);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);
    result = bind(s, (struct sockaddr*)&addr, sizeof(addr));
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

void ListenSocket(int64_t socketHandle, int backlog)
{
    int result = 0;
    SOCKET s = static_cast<SOCKET>(socketHandle);
    result = listen(s, backlog);
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

int64_t AcceptSocket(int64_t socketHandle)
{
    SOCKET s = static_cast<SOCKET>(socketHandle);
    SOCKET a = accept(s, NULL, NULL);
    if (a == INVALID_SOCKET)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
    return static_cast<int64_t>(a);
}

void CloseSocket(int64_t socketHandle)
{
    int result = 0;
    SOCKET s = static_cast<SOCKET>(socketHandle);
#ifdef _WIN32
    result = closesocket(s);
#else
    result = close(s);
#endif
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

void ShutdownSocket(int64_t socketHandle, ShutdownMode mode)
{
    int result = 0;
    int how = SD_RECEIVE;
    switch (mode)
    {
        case ShutdownMode::receive: how = SD_RECEIVE; break;
        case ShutdownMode::send: how = SD_SEND; break;
        case ShutdownMode::both: how = SD_BOTH; break;
    }
    SOCKET s = static_cast<SOCKET>(socketHandle);
    result = shutdown(s, how);
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
}

int64_t ConnectSocket(const std::string& node, const std::string& service)
{
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
                return static_cast<int64_t>(s);
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

int SendSocket(int64_t socketHandle, const uint8_t* buf, int len, int flags)
{
    int result = 0;
    SOCKET s = static_cast<SOCKET>(socketHandle);
    result = send(s, (const char*)buf, len, flags);
    if (result < 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        throw std::runtime_error(errorMessage);
    }
    return result;
}

int ReceiveSocket(int64_t socketHandle, uint8_t* buf, int len, int flags)
{
    int result = 0;
    SOCKET s = static_cast<SOCKET>(socketHandle);
    result = recv(s, (char*)buf, len, flags);
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
    Sockets::Init();
}

void DoneSocket()
{
    Sockets::Done();
}

TcpSocket::TcpSocket() : handle(CreateSocket()), connected(false), shutdown(false)
{
}

TcpSocket::TcpSocket(const std::string& node, const std::string& service) : handle(ConnectSocket(node.c_str(), service.c_str())), connected(true), shutdown(false)
{
}

TcpSocket::TcpSocket(int64_t handle_) noexcept : handle(handle_), connected(true), shutdown(false)
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
    if (handle != -1)
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
    if (handle != -1)
    {
        if (connected && !shutdown)
        {
            Shutdown(ShutdownMode::both);
        }
        int64_t s = handle;
        handle = -1;
        connected = false;
        CloseSocket(s);
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
    int64_t acceptedHandle = AcceptSocket(handle);
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
        int32_t result = SendSocket(handle, buffer + offset, count, 0);
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

void Write(TcpSocket& socket, const std::string& s)
{
    int32_t size = s.length();
    uint8_t buffer[sizeof(size)];
    MemoryWriter writer(&buffer[0], sizeof(size));
    writer.Write(size);
    socket.Send(&buffer[0], sizeof(size));
    socket.Send(reinterpret_cast<const uint8_t*>(s.c_str()), size);
}

std::string ReadStr(TcpSocket& socket)
{
    int32_t size = 0;
    uint8_t buffer[sizeof(size)];
    int offset = 0;
    int bytesToReceive = sizeof(size);
    int bytesReceived = socket.Receive(&buffer[offset], bytesToReceive);
    if (bytesReceived == 0)
    {
        return std::string();
    }
    bytesToReceive = bytesToReceive - bytesReceived;
    offset = offset + bytesReceived;
    while (bytesToReceive > 0)
    {
        bytesReceived = socket.Receive(&buffer[offset], bytesToReceive);
        if (bytesReceived == 0)
        {
            return std::string();
        }
        bytesToReceive = bytesToReceive - bytesReceived;
        offset = offset + bytesReceived;
    }
    MemoryReader reader(&buffer[0], sizeof(size));
    size = reader.ReadInt();
    if (size == 0)
    {
        return std::string();
    }
    std::unique_ptr<uint8_t[]> mem(new uint8_t[size]);
    offset = 0;
    bytesToReceive = size;
    bytesReceived = socket.Receive(mem.get() + offset, bytesToReceive);
    if (bytesReceived == 0)
    {
        return std::string();
    }
    bytesToReceive = bytesToReceive - bytesReceived;
    offset = offset + bytesReceived;
    while (bytesToReceive > 0)
    {
        bytesReceived = socket.Receive(mem.get() + offset, bytesToReceive);
        if (bytesReceived == 0)
        {
            return std::string();
        }
        bytesToReceive = bytesToReceive - bytesReceived;
        offset = offset + bytesReceived;
    }
    std::string str(reinterpret_cast<const char*>(mem.get()), size);
    return str;
}

} } // namespace soulng::util
