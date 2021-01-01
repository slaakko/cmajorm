// =================================
// Copyright (c) 2021 Seppo Laakko
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
    SocketServer(Log* log_, const std::string& serverName, bool continuous_);
    const std::string& Name() const override;
    Log* GetLog() const { return log; }
    void WaitForRunning();
    void Run(const std::string& prompt);
    bool Exiting() const { return exiting; }
    void Exit() override;
    void ExitContinuous();
private:
    bool exiting;
    bool continuous;
    std::mutex mtx;
    std::condition_variable running;
    bool isRunning;
    Log* log;
    std::string serverName;
    std::string name;
    TcpSocket socket;
    std::unique_ptr<SocketConnection> connection;
    std::unique_ptr<BuildServer> buildServer;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SOCKET_SERVER_INCLUDED
