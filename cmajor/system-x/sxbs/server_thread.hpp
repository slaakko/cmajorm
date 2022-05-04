// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SXBS_SERVER_THREAD_INCLUDED
#define SXBS_SERVER_THREAD_INCLUDED
#include <soulng/util/Socket.hpp>
#include <stdint.h>
#include <string>

namespace sxbs {

class ServerThread
{
public:
    ServerThread(soulng::util::TcpSocket&& socket_);
    void Stop();
    void Run();
    void SendLogLine(const std::string& logLine);
private:
    soulng::util::TcpSocket socket;
    std::exception_ptr ex;
};

void RunServerThread(ServerThread* serverThread);

} // sxbs

#endif // SXBS_SERVER_THREAD_INCLUDED
