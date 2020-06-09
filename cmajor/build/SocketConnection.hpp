// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_SOCKET_CONNECTION_INCLUDED
#define CMAJOR_BUILD_SOCKET_CONNECTION_INCLUDED
#include <cmajor/build/Connection.hpp>
#include <soulng/util/Socket.hpp>

namespace cmajor { namespace build {

class Host;

class BUILD_API SocketConnection : public Connection
{
public:
    SocketConnection(Log* log, Host* host_, TcpSocket&& socket_);
    void DoSend(JsonObject* messageObject) override;
    std::unique_ptr<JsonObject> DoReceive() override;
    void DoClose() override;
    const std::string& GetActor() const override;
private:
    Host* host;
    TcpSocket socket;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SOCKET_CONNECTION_INCLUDED
