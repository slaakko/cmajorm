// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/ServerExecutionContext.hpp>

namespace cmajor { namespace build {

void RunContinuousSocketServer(SocketServer* socketServer)
{
    try
    {
        socketServer->Run("type 'exit <ENTER>' to stop and exit server");
    }
    catch (const std::exception& ex)
    {
        if (socketServer->GetLog())
        {
            socketServer->GetLog()->Write("RunContinuousSocketServer got exception '" + std::string(ex.what()) + "'", nullptr);
        }
    }
}

ServerExecutionContext::ServerExecutionContext(const std::string& serverName) 
{
    socketServer.reset(new SocketServer(GetLog(), serverName, true));
    serverThread = std::thread{ RunContinuousSocketServer, socketServer.get() };
    socketServer->WaitForRunning();
}

ServerExecutionContext::~ServerExecutionContext()
{
    try
    {
        serverThread.join();
    }
    catch (const std::exception&)
    {
    }
}

void ServerExecutionContext::ExitServer()
{
    return socketServer->ExitContinuous();
}

} } // namespace cmajor::build
