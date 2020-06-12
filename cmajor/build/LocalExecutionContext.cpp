// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/LocalExecutionContext.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <stdexcept>

namespace cmajor { namespace build {

void RunSocketServer(SocketServer* server)
{
    try
    {
        server->Run();
    }
    catch (const std::exception& ex)
    {
        if (server->GetLog())
        {
            server->GetLog()->Write("RunSocketServer got exception '" + std::string(ex.what()) + "'", nullptr);
        }
    }
}

LocalExecutionContext::LocalExecutionContext() : ExecutionContext()
{
    socketServer.reset(new SocketServer(GetLog(), "local"));
    serverThread = std::thread{ RunSocketServer, socketServer.get() };
    socketServer->WaitForRunning();
    socketClient.reset(new SocketClient(GetLog(), "local"));
    socketClient->Run();
}

LocalExecutionContext::~LocalExecutionContext()
{
    try
    {
        Connection* connection = socketClient->GetConnection();
        if (connection)
        {
            if (connection->ServerAlive())
            {
                connection->SetServerAlive(false);
                if (connection->GetLog())
                {
                    connection->GetLog()->Write("closing connection\n", nullptr);
                }
                CloseConnectionRequest closeConnectionRequest;
                closeConnectionRequest.SendTo(*connection);
            }
        }
        serverThread.join();
    }
    catch (const std::exception&)
    {
    }
}

BuildClient* LocalExecutionContext::GetClient()
{
    return socketClient->GetClient();
}

Connection* LocalExecutionContext::GetConnection()
{
    return socketClient->GetConnection();
}

} } // namespace cmajor::build
