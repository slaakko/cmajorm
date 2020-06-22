// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/ClientExecutionContext.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <stdexcept>

namespace cmajor { namespace build {

ClientExecutionContext::ClientExecutionContext(const std::string& serverName) : ExecutionContext()
{
    socketClient.reset(new SocketClient(GetLog(), serverName));
    socketClient->Run();
}

ClientExecutionContext::~ClientExecutionContext()
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
    }
    catch (const std::exception&)
    {
    }
}

BuildClient* ClientExecutionContext::GetClient()
{
    return socketClient->GetClient();
}

Connection* ClientExecutionContext::GetConnection()
{
    return socketClient->GetConnection();
}

} } // namespace cmajor::build
