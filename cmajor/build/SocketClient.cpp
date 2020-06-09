// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/SocketClient.hpp>
#include <cmajor/build/ServerConfig.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Log.hpp>

namespace cmajor { namespace build {

using namespace cmajor::symbols;
using namespace soulng::util;

SocketClient::SocketClient(Log* log_, const std::string& serverName_) : log(log_), name("client"), serverName(serverName_), socket()
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
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(-1, "connecting to server '" + serverName + "' in port " + std::to_string(serverInfo->Port()) + "...");
        }
        socket.Connect("localhost", std::to_string(serverInfo->Port()));
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(-1, "connected");
        }
    }
    else
    {
        throw std::runtime_error("server name '" + serverName + "' not found");
    }
    connection.reset(new SocketConnection(log, this, std::move(socket)));
    buildClient.reset(new BuildClient(connection.get()));
}

void SocketClient::Exit()
{
}

} } // namespace cmajor::build
