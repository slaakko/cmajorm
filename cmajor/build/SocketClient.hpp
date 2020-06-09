// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_SOCKET_CLIENT_INCLUDED
#define CMAJOR_BUILD_SOCKET_CLIENT_INCLUDED
#include <cmajor/build/BuildClient.hpp>
#include <cmajor/build/Host.hpp>
#include <cmajor/build/SocketConnection.hpp>

namespace cmajor { namespace build {

class BUILD_API SocketClient : public Host
{
public:
    SocketClient(Log* log_, const std::string& serverName_);
    const std::string& Name() const override;
    void Run();
    void Exit() override;
    BuildClient* GetClient() const { return buildClient.get(); }
    Connection* GetConnection() const { return connection.get(); }
private:
    Log* log;
    std::string name;
    std::string serverName;
    TcpSocket socket;
    std::unique_ptr<SocketConnection> connection;
    std::unique_ptr<BuildClient> buildClient;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SOCKET_CLIENT_INCLUDED
