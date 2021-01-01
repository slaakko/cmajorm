// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/SocketClient.hpp>
#include <cmajor/build/ServerConfig.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Log.hpp>

namespace cmajor { namespace build {

using namespace cmajor::symbols;
using namespace soulng::util;

SocketClient::SocketClient(Log* log_, const std::string& serverName_) : log(log_), name("socket client"), serverName(serverName_), socket()
{
}

const std::string& SocketClient::Name() const
{
    return name;
}

void SocketClient::Run()
{
    ServerInfo* serverInfo = ServerConfig::Instance().GetServerInfo(serverName, true, true);
    if (serverInfo)
    {
        std::string node = "localhost";
        std::string service = std::to_string(serverInfo->Port());
        std::string hostPortStr;
        if (!serverInfo->Host().empty() && serverInfo->Host() != "localhost" && serverInfo->Host() != "127.0.0.1")
        {
            node = serverInfo->Host();
            hostPortStr = "host " + serverInfo->Host() + " port " + std::to_string(serverInfo->Port());
        }
        else
        {
            hostPortStr = "port " + std::to_string(serverInfo->Port());
        }
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(-1, "socket client: connecting to server '" + serverName + "' in " + hostPortStr + "...");
        }
        socket.Connect(node, service);
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(-1, "socket client: connected");
        }
    }
    else
    {
        throw std::runtime_error("socket client: server name '" + serverName + "' not found");
    }
    connection.reset(new SocketConnection(log, this, std::move(socket)));
    connection->SetServerAlive(true);
    buildClient.reset(new BuildClient(connection.get()));
    buildClient->SetDefaultToolChain(serverInfo->DefaultToolChain());
}

void SocketClient::Exit()
{
}

} } // namespace cmajor::build
