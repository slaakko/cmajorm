// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Socket.hpp>
#include <cmajor/rt/Io.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/rt/InitDone.hpp>
#include <cmajor/util/Error.hpp>
#include <cmajor/util/TextUtils.hpp>
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
#include <gnutls/gnutls.h>

namespace cmajor { namespace rt {

using namespace cmajor::util;

struct SocketData
{
    SocketData() : socket(INVALID_SOCKET), session(), xcred(), tlsSession(false) {}
    SocketData(SOCKET socket_) : socket(socket_), session(), xcred(), tlsSession(false) {}
    SOCKET socket;
    bool tlsSession;
    gnutls_session_t session;
    gnutls_certificate_credentials_t xcred;
};

class SocketTable
{
public:
    static void Init();
    static void Done();
    static SocketTable& Instance() { Assert(instance, "socket table not initialized"); return *instance; }
    ~SocketTable();
    int32_t CreateSocket();
    int32_t BindSocket(int32_t socketHandle, int32_t port);
    int32_t ListenSocket(int32_t socketHandle, int32_t backlog);
    int32_t AcceptSocket(int32_t socketHandle);
    int32_t CloseSocket(int32_t socketHandle);
    int32_t ShutdownSocket(int32_t socketHandle, ShutdownMode mode);
    int32_t ConnectSocket(const std::string& node, const std::string& service, ConnectOptions options);
    int32_t SendSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags);
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
    char buf[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, buf, sizeof(buf), NULL);
    return std::string(buf);
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
        RtWrite(2, (const uint8_t*)errorMessage.c_str(), errorMessage.length());
        RtExit(exitCodeSocketInitializationFailed);
    }
#endif
    int result = gnutls_global_init();
    if (result < 0)
    {
        std::string errorMessage = "gnutls_global_init failed with error code " + std::to_string(result) + ": " + gnutls_strerror(result);
        RtWrite(2, (const uint8_t*)errorMessage.c_str(), errorMessage.length());
        RtExit(exitCodeSocketInitializationFailed);
    }
}

SocketTable::~SocketTable()
{
    gnutls_global_deinit();
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
        return InstallError(errorMessage);
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

int32_t SocketTable::BindSocket(int32_t socketHandle, int32_t port)
{
    int result = 0;
    if (socketHandle <= 0)
    {
        return InstallError("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        if (!sockets[socketHandle])
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = sockets[socketHandle]->socket;
        if (s == INVALID_SOCKET)
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(port);
        result = bind(s, (struct sockaddr*) &addr, sizeof(addr));
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = socketMap.find(socketHandle);
        if (it != socketMap.cend())
        {
            if (!it->second)
            {
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = it->second->socket;
            if (s == INVALID_SOCKET)
            {
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            addr.sin_port = htons(port);
            result = bind(s, (struct sockaddr*) &addr, sizeof(addr));
        }
        else
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        return InstallError(errorMessage);
    }
    return 0;
}

int32_t SocketTable::ListenSocket(int32_t socketHandle, int32_t backlog)
{
    int result = 0;
    if (socketHandle <= 0)
    {
        return InstallError("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        if (!sockets[socketHandle])
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = sockets[socketHandle]->socket;
        if (s == INVALID_SOCKET)
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
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
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = it->second->socket;
            if (s == INVALID_SOCKET)
            {
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            result = listen(s, backlog);
        }
        else
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        return InstallError(errorMessage);
    }
    return 0;
}

int32_t SocketTable::AcceptSocket(int32_t socketHandle)
{
    SOCKET a = 0;
    if (socketHandle <= 0)
    {
        return InstallError("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        if (!sockets[socketHandle])
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        SOCKET s = sockets[socketHandle]->socket;
        if (s == INVALID_SOCKET)
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        a = accept(s, NULL, NULL);
        if (a == INVALID_SOCKET)
        {
            int errorCode = GetLastSocketError();
            std::string errorMessage = GetSocketErrorMessage(errorCode);
            return InstallError(errorMessage);
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
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            SOCKET s = it->second->socket;
            if (s == INVALID_SOCKET)
            {
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            a = accept(s, NULL, NULL);
            if (a == INVALID_SOCKET)
            {
                int errorCode = GetLastSocketError();
                std::string errorMessage = GetSocketErrorMessage(errorCode);
                return InstallError(errorMessage);
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
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
    }
}

int32_t SocketTable::CloseSocket(int32_t socketHandle)
{
    int result = 0;
    if (socketHandle <= 0)
    {
        return InstallError("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        if (socketData->tlsSession)
        {
            gnutls_bye(socketData->session, GNUTLS_SHUT_RDWR);
        }
        SOCKET s = socketData->socket;
#ifdef _WIN32
        result = closesocket(s);
#else
        result = close(s);
#endif
        if (socketData->tlsSession)
        {
            gnutls_deinit(socketData->session);
            gnutls_certificate_free_credentials(socketData->xcred);
        }
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
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            if (socketData->tlsSession)
            {
                gnutls_bye(socketData->session, GNUTLS_SHUT_RDWR);
            }
            SOCKET s = socketData->socket;
#ifdef _WIN32
            result = closesocket(s);
#else
            result = close(s);
#endif
            if (socketData->tlsSession)
            {
                gnutls_deinit(socketData->session);
                gnutls_certificate_free_credentials(socketData->xcred);
            }
        }
        else
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        return InstallError(errorMessage);
    }
    return 0;
}

int32_t SocketTable::ShutdownSocket(int32_t socketHandle, ShutdownMode mode)
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
        return InstallError("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        if (socketData->tlsSession)
        {
            if (how == SD_BOTH)
            {
                gnutls_bye(socketData->session, GNUTLS_SHUT_RDWR);
            }
            else
            {
                gnutls_bye(socketData->session, GNUTLS_SHUT_WR);
            }
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
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            if (socketData->tlsSession)
            {
                if (how == SD_BOTH)
                {
                    gnutls_bye(socketData->session, GNUTLS_SHUT_RDWR);
                }
                else
                {
                    gnutls_bye(socketData->session, GNUTLS_SHUT_WR);
                }
            }
            SOCKET s = socketData->socket;
            result = shutdown(s, how);
        }
        else
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result != 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        return InstallError(errorMessage);
    }
    return 0;
}

int32_t SocketTable::ConnectSocket(const std::string& node, const std::string& service, ConnectOptions options)
{
    std::unique_ptr<SocketData> socketData(new SocketData());
    bool createTlsSession = false;
    if ((ToLower(service) == "https") || (service == std::to_string(443)) || ((options & ConnectOptions::useTls) != ConnectOptions::none))
    {
        createTlsSession = true;
        int result = gnutls_certificate_allocate_credentials(&socketData->xcred);
        if (result < 0)
        {
            std::string errorMessage = "gnutls_certificate_allocate_credentials failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
            return InstallError(errorMessage);
        }
        result = gnutls_certificate_set_x509_system_trust(socketData->xcred);
        if (result < 0)
        {
            std::string errorMessage = "gnutls_certificate_set_x509_system_trust failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
            return InstallError(errorMessage);
        }
        result = gnutls_init(&socketData->session, GNUTLS_CLIENT);
        if (result < 0)
        {
            std::string errorMessage = "gnutls_init failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
            return InstallError(errorMessage);
        }
        result = gnutls_server_name_set(socketData->session, GNUTLS_NAME_DNS, node.c_str(), node.length());
        if (result < 0)
        {
            std::string errorMessage = "gnutls_server_name_set failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
            return InstallError(errorMessage);
        }
        result = gnutls_set_default_priority(socketData->session);
        if (result < 0)
        {
            std::string errorMessage = "gnutls_set_default_priority failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
            return InstallError(errorMessage);
        }
        result = gnutls_credentials_set(socketData->session, GNUTLS_CRD_CERTIFICATE, socketData->xcred);
        if (result < 0)
        {
            std::string errorMessage = "gnutls_credentials_set failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
            return InstallError(errorMessage);
        }
#if GNUTLS_VERSION_NUMBER >= 0x030600
        gnutls_session_set_verify_cert(socketData->session, node.c_str(), 0);
#endif
    }
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
        return InstallError(errorMessage);
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
                    if (createTlsSession)
                    {
                        socketData->socket = s;
                        sockets[connectedSocketHandle] = std::move(socketData);
                        gnutls_transport_set_int(sockets[connectedSocketHandle]->session, s);
                        gnutls_handshake_set_timeout(sockets[connectedSocketHandle]->session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);
                        do
                        {
                            result = gnutls_handshake(sockets[connectedSocketHandle]->session);
                        } 
                        while (result < 0 && gnutls_error_is_fatal(result) == 0);
                        if (result < 0)
                        {
                            std::string errorMessage = "gnutls_handshake failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
                            return InstallError(errorMessage);
                        }
                        sockets[connectedSocketHandle]->tlsSession = true;
                    }
                    else
                    {
                        sockets[connectedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(s));
                    }
                }
                else
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    if (createTlsSession)
                    {
                        socketData->socket = s;
                        socketMap[connectedSocketHandle] = std::move(socketData);
                        gnutls_transport_set_int(socketMap[connectedSocketHandle]->session, s);
                        gnutls_handshake_set_timeout(socketMap[connectedSocketHandle]->session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);
                        do
                        {
                            result = gnutls_handshake(socketMap[connectedSocketHandle]->session);
                        } 
                        while (result < 0 && gnutls_error_is_fatal(result) == 0);
                        if (result < 0)
                        {
                            std::string errorMessage = "gnutls_handshake failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
                            return InstallError(errorMessage);
                        }
                        socketMap[connectedSocketHandle]->tlsSession = true;
                    }
                    else
                    {
                        socketMap[connectedSocketHandle] = std::unique_ptr<SocketData>(new SocketData(s));
                    }
                }
                return connectedSocketHandle;
            }
            else
            {
                freeaddrinfo(res);
                int errorCode = GetLastSocketError();
                std::string errorMessage = GetSocketErrorMessage(errorCode);
                return InstallError(errorMessage);
            }
        }
    }
    std::string errorMessage = "could not connect";
    return InstallError(errorMessage);
}

int32_t SocketTable::SendSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags)
{
    int32_t result = 0;
    if (socketHandle <= 0)
    {
        return InstallError("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        if (socketData->tlsSession)
        {
            result = gnutls_record_send(socketData->session, reinterpret_cast<const void*>(buf), len);
            if (result < 0)
            {
                std::string errorMessage = "gnutls_record_send failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
                return InstallError(errorMessage);
            }
        }
        else
        {
            SOCKET s = socketData->socket;
            result = send(s, (const char*)buf, len, flags);
        }
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
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            if (socketData->tlsSession)
            {
                result = gnutls_record_send(socketData->session, reinterpret_cast<const void*>(buf), len);
                if (result < 0)
                {
                    std::string errorMessage = "gnutls_record_send failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
                    return InstallError(errorMessage);
                }
            }
            else
            {
                SOCKET s = socketData->socket;
                result = send(s, (const char*)buf, len, flags);
            }
        }
        else
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result < 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        return InstallError(errorMessage);
    }
    return result;
}

int32_t SocketTable::ReceiveSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags)
{
    int32_t result = 0;
    if (socketHandle <= 0)
    {
        return InstallError("invalid socket handle " + std::to_string(socketHandle));
    }
    else if (socketHandle < maxNoLockSocketHandles)
    {
        SocketData* socketData = sockets[socketHandle].get();
        if (!socketData)
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
        if (socketData->tlsSession)
        {
            result = gnutls_record_recv(socketData->session, reinterpret_cast<void*>(buf), len);
            if (result < 0)
            {
                std::string errorMessage = "gnutls_record_recv failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
                return InstallError(errorMessage);
            }
        }
        else
        {
            SOCKET s = socketData->socket;
            result = recv(s, (char*)buf, len, flags);
        }
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
                return InstallError("invalid socket handle " + std::to_string(socketHandle));
            }
            if (socketData->tlsSession)
            {
                result = gnutls_record_recv(socketData->session, reinterpret_cast<void*>(buf), len);
                if (result < 0)
                {
                    std::string errorMessage = "gnutls_record_recv failed with error code " + ToString(result) + " : " + gnutls_strerror(result);
                    return InstallError(errorMessage);
                }
            }
            else
            {
                SOCKET s = socketData->socket;
                result = recv(s, (char*)buf, len, flags);
            }
        }
        else
        {
            return InstallError("invalid socket handle " + std::to_string(socketHandle));
        }
    }
    if (result < 0)
    {
        int errorCode = GetLastSocketError();
        std::string errorMessage = GetSocketErrorMessage(errorCode);
        return InstallError(errorMessage);
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

} }  // namespace cmajor::rt

extern "C" RT_API int32_t RtCreateSocket()
{
    return cmajor::rt::SocketTable::Instance().CreateSocket();
}

extern "C" RT_API int32_t RtBindSocket(int32_t socketHandle, int32_t port)
{
    return cmajor::rt::SocketTable::Instance().BindSocket(socketHandle, port);
}

extern "C" RT_API int32_t RtListenSocket(int32_t socketHandle, int32_t backLog)
{
    return cmajor::rt::SocketTable::Instance().ListenSocket(socketHandle, backLog);
}

extern "C" RT_API int32_t RtAcceptSocket(int32_t socketHandle)
{
    return cmajor::rt::SocketTable::Instance().AcceptSocket(socketHandle);
}

extern "C" RT_API int32_t RtCloseSocket(int32_t socketHandle)
{
    return cmajor::rt::SocketTable::Instance().CloseSocket(socketHandle);
}

extern "C" RT_API int32_t RtShutdownSocket(int32_t socketHandle, ShutdownMode mode)
{
    return cmajor::rt::SocketTable::Instance().ShutdownSocket(socketHandle, mode);
}

extern "C" RT_API int32_t RtConnectSocket(const char* node, const char* service, ConnectOptions options)
{
    return cmajor::rt::SocketTable::Instance().ConnectSocket(node, service, options);
}

extern "C" RT_API int32_t RtSendSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags)
{
    return cmajor::rt::SocketTable::Instance().SendSocket(socketHandle, buf, len, flags);
}

extern "C" RT_API int32_t RtReceiveSocket(int32_t socketHandle, uint8_t* buf, int32_t len, int32_t flags)
{
    return cmajor::rt::SocketTable::Instance().ReceiveSocket(socketHandle, buf, len, flags);
}
