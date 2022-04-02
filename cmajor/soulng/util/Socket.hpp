// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_SOCKET_INCLUDED
#define SOULNG_UTIL_SOCKET_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>
#include <stdint.h>

namespace soulng { namespace util {

enum class ShutdownMode 
{
    receive = 0, send = 1, both = 2
};

UTIL_API int64_t CreateSocket();
UTIL_API void BindSocket(int64_t socketHandle, int port);
UTIL_API void ListenSocket(int64_t socketHandle, int backLog);
UTIL_API int64_t AcceptSocket(int64_t socketHandle);
UTIL_API void CloseSocket(int64_t socketHandle);
UTIL_API void ShutdownSocket(int64_t socketHandle, ShutdownMode mode);
UTIL_API int64_t ConnectSocket(const std::string& node, const std::string& service);
UTIL_API int SendSocket(int64_t socketHandle, const uint8_t* buf, int len, int flags);
UTIL_API int ReceiveSocket(int64_t socketHandle, uint8_t* buf, int len, int flags);
UTIL_API void InitSocket();
UTIL_API void DoneSocket();

class UTIL_API TcpSocket
{
public:
    TcpSocket();
    TcpSocket(const std::string& node, const std::string& service);
    TcpSocket(int64_t handle_) noexcept;
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;
    TcpSocket(TcpSocket&& that) noexcept;
    TcpSocket& operator=(TcpSocket&& that) noexcept;
    ~TcpSocket();
    void Close();
    void Connect(const std::string& node, const std::string& service);
    void Bind(int port);
    void Listen(int backlog);
    TcpSocket Accept();
    void Shutdown(ShutdownMode mode);
    void Send(const uint8_t* buffer, int count);
    int Receive(uint8_t* buffer, int count);
private:
    int64_t handle;
    bool connected;
    bool shutdown;
};

UTIL_API void Write(TcpSocket& socket, const std::string& s);
UTIL_API std::string ReadStr(TcpSocket& socket);

} } // namespace soulng::util

#endif // SOULNG_UTIL_SOCKET_INCLUDED
