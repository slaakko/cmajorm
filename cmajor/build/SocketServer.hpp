// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_SOCKET_SERVER_INCLUDED
#define CMAJOR_BUILD_SOCKET_SERVER_INCLUDED
#include <cmajor/build/BuildServer.hpp>
#include <cmajor/build/SocketConnection.hpp>
#include <cmajor/build/Host.hpp>
#include <condition_variable>
 
namespace cmajor { namespace build {

class BUILD_API SocketServer : public Host
{
public:
    SocketServer(Log* log_, const std::string& serverName);
    const std::string& Name() const override;
    Log* GetLog() const { return log; }
    void WaitForRunning();
    void Run();
    void Exit() override;
private:
    std::mutex mtx;
    std::condition_variable running;
    Log* log;
    std::string serverName;
    TcpSocket socket;
    std::unique_ptr<SocketConnection> connection;
    std::unique_ptr<BuildServer> buildServer;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SOCKET_SERVER_INCLUDED
