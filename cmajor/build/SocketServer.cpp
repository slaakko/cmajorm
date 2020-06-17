// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/SocketServer.hpp>
#include <cmajor/build/ServerConfig.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Log.hpp>

namespace cmajor { namespace build {

using namespace cmajor::symbols;
using namespace soulng::util;

SocketServer::SocketServer(Log* log_, const std::string& serverName_) : log(log_), serverName(serverName_), name("socket server: '" + serverName + "'"), socket(), running(), isRunning(false)
{
    ServerInfo* serverInfo = ServerConfig::Instance().GetServerInfo(serverName, true, true);
    if (serverInfo)
    {
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(-1, "socket server: binding server '" + serverName + "' to port " + std::to_string(serverInfo->Port()) + "...");
        }
        socket.Bind(serverInfo->Port());
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(-1, "socket server: server '" + serverName + "' listening...");
        }
        socket.Listen(10);
    }
    else
    {
        throw std::runtime_error("socket server: server name '" + serverName + "' not found");
    }
}

const std::string& SocketServer::Name() const
{
    return name;
}

void SocketServer::WaitForRunning()
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "socket server: '" + serverName + "' wait for running...");
    }
    {
        std::unique_lock<std::mutex> lock(mtx);
        running.wait(lock, [this]{ return isRunning; });
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "socket server: '" + serverName + "' running");
    }
}

void SocketServer::Run()
{
    isRunning = true;
    running.notify_all();
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "socket server: '" + serverName + "' waiting for client connection...");
    }
    TcpSocket connectedSocket = socket.Accept();
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "socket server: '" + serverName + "' accepted a client connection...");
    }
    connection.reset(new SocketConnection(log, this, std::move(connectedSocket)));
    buildServer.reset(new BuildServer(connection.get()));
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "socket server: '" + serverName + "' running build server...");
    }
    buildServer->Run();
}

void SocketServer::Exit()
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "socket server '" + serverName + "' exiting...");
    }
    buildServer->Exit();
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "socket server '" + serverName + "' exited");
    }
}

} } // namespace cmajor::build
