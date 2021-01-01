// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Socket.hpp>
#include <cmajor/rts/Io.hpp>
#include <cmajor/rts/Error.hpp>
#include <cmajor/rts/InitDone.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/TextUtils.hpp>
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

namespace cmajor { namespace rt {

using namespace soulng::util;
using namespace soulng::unicode;

struct SocketData
{
    SocketData(SOCKET socket_) : socket(socket_)
    {
    }
    SOCKET socket;
};

struct Sockets
{
    static void Init();
    static void Done();
};

#ifdef _WIN32

std::string GetSocketErrorMessage(int errorCode)
{
    char16_t buf[1024];
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

void* CreateSocket(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return nullptr;
    }
    return new SocketData(s);
}

bool BindSocket(void* socketHandle, int32_t port, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!socketHandle)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
    SocketData* socketData = static_cast<SocketData*>(socketHandle);
    SOCKET s = socketData->socket;
    if (s == INVALID_SOCKET)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);
    int result = bind(s, (struct sockaddr*) &addr, sizeof(addr));
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return false;
    }
    return true;
}

bool ListenSocket(void* socketHandle, int32_t backlog, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!socketHandle)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
    SocketData* socketData = static_cast<SocketData*>(socketHandle);
    SOCKET s = socketData->socket;
    if (s == INVALID_SOCKET)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
    int result = listen(s, backlog);
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return false;
    }
    return true;
}

void* AcceptSocket(void* socketHandle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!socketHandle)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return nullptr;
    }
    SocketData* socketData = static_cast<SocketData*>(socketHandle);
    SOCKET s = socketData->socket;
    if (s == INVALID_SOCKET)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return nullptr;
    }
    SOCKET a = a = accept(s, NULL, NULL);
    if (a == INVALID_SOCKET)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return nullptr;
    }
    void* acceptedSocketHandle = new SocketData(a);
    return acceptedSocketHandle;
}

bool CloseSocket(void* socketHandle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!socketHandle)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
    SocketData* socketData = static_cast<SocketData*>(socketHandle);
    SOCKET s = socketData->socket;
    if (s == INVALID_SOCKET)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
#ifdef _WIN32
    int result = closesocket(s);
#else
    int result = close(s);
#endif
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return false;
    }
    return true;
}

void RtDestroySocket(void* socketHandle)
{
    if (socketHandle)
    {
        SocketData* socketData = static_cast<SocketData*>(socketHandle);
        delete (socketData);
    }
}

bool ShutdownSocket(void* socketHandle, ShutdownMode mode, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!socketHandle)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
    SocketData* socketData = static_cast<SocketData*>(socketHandle);
    SOCKET s = socketData->socket;
    if (s == INVALID_SOCKET)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return false;
    }
    int how = SD_RECEIVE;
    switch (mode)
    {
        case ShutdownMode::receive: how = SD_RECEIVE; break;
        case ShutdownMode::send: how = SD_SEND; break;
        case ShutdownMode::both: how = SD_BOTH; break;
    }
    int result = shutdown(s, how);
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return false;
    }
    return true;
}

void* ConnectSocket(const std::string& node, const std::string& service, ConnectOptions options, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
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
        errorStringHandle = InstallError(errorMessage);
        return nullptr;
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
                return new SocketData(s);
            }
            else
            {
                freeaddrinfo(res);
                int errorCode = GetLastSocketError();
                std::string errorMessage = GetSocketErrorMessage(errorCode);
                errorStringHandle = InstallError(errorMessage);
                return nullptr;
            }
        }
    }
    std::string errorMessage = "could not connect";
    errorStringHandle = InstallError(errorMessage);
    return nullptr;
}

int32_t SendSocket(void* socketHandle, uint8_t* buf, int32_t len, int32_t flags, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!socketHandle)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return -1;
    }
    SocketData* socketData = static_cast<SocketData*>(socketHandle);
    SOCKET s = socketData->socket;
    if (s == INVALID_SOCKET)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return -1;
    }
    int32_t result = send(s, (const char*)buf, len, flags);
    if (result < 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return -1;
    }
    return result;
}

int32_t ReceiveSocket(void* socketHandle, uint8_t* buf, int32_t len, int32_t flags, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!socketHandle)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return -1;
    }
    SocketData* socketData = static_cast<SocketData*>(socketHandle);
    SOCKET s = socketData->socket;
    if (s == INVALID_SOCKET)
    {
        errorStringHandle = InstallError("invalid socket handle");
        return -1;
    }
    int32_t result = recv(s, (char*)buf, len, flags);
    if (result < 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        errorStringHandle = InstallError(errorMessage);
        return -1;
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

} }  // namespace cmajor::rt

void* RtCreateSocket(int32_t& errorStringHandle)
{
    return cmajor::rt::CreateSocket(errorStringHandle);
}

bool RtBindSocket(void* socketHandle, int32_t port, int32_t& errorStringHandle)
{
    return cmajor::rt::BindSocket(socketHandle, port, errorStringHandle);
}

bool RtListenSocket(void* socketHandle, int32_t backLog, int32_t& errorStringHandle)
{
    return cmajor::rt::ListenSocket(socketHandle, backLog, errorStringHandle);
}

void* RtAcceptSocket(void* socketHandle, int32_t& errorStringHandle)
{
    return cmajor::rt::AcceptSocket(socketHandle, errorStringHandle);
}

bool RtCloseSocket(void* socketHandle, int32_t& errorStringHandle)
{
    return cmajor::rt::CloseSocket(socketHandle, errorStringHandle);
}

bool RtShutdownSocket(void* socketHandle, ShutdownMode mode, int32_t& errorStringHandle)
{
    return cmajor::rt::ShutdownSocket(socketHandle, mode, errorStringHandle);
}

void* RtConnectSocket(const char* node, const char* service, ConnectOptions options, int32_t& errorStringHandle)
{
    return cmajor::rt::ConnectSocket(node, service, options, errorStringHandle);
}

int32_t RtSendSocket(void* socketHandle, uint8_t* buf, int32_t len, int32_t flags, int32_t& errorStringHandle)
{
    return cmajor::rt::SendSocket(socketHandle, buf, len, flags, errorStringHandle);
}

int32_t RtReceiveSocket(void* socketHandle, uint8_t* buf, int32_t len, int32_t flags, int32_t& errorStringHandle)
{
    return cmajor::rt::ReceiveSocket(socketHandle, buf, len, flags, errorStringHandle);
}

void RtDestroySocket(void* socketHandle)
{
    cmajor::rt::RtDestroySocket(socketHandle);
}
