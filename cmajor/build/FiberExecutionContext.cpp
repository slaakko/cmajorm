// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/FiberExecutionContext.hpp>
#include <cmajor/build/FiberConnection.hpp>
#include <cmajor/build/BuildClient.hpp>
#include <cmajor/build/BuildServer.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <soulng/util/Fiber.hpp>
#include <stdexcept>

namespace cmajor { namespace build {

void (*runServerFunction)();

FiberExecutionContext::FiberExecutionContext()
{
    connection = CreateFiberConnection(GetLog());
    client.reset(new BuildClient(connection.get()));
    server.reset(new BuildServer(connection.get()));
    SetServer(server.get());
    void* clientFiber = ConvertThreadToFiber(nullptr);
    runServerFunction = RunServer;
    void* startAddress = reinterpret_cast<void*>(runServerFunction);
    void* serverFiber = CreateFiber(1024 * 1024, startAddress, nullptr);
    connection->SetFibers(serverFiber, clientFiber);
    SwitchToFiber(serverFiber);
}

FiberExecutionContext::~FiberExecutionContext()
{
    try
    {
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

Connection* FiberExecutionContext::GetConnection()
{
    return connection.get();
}

} } // namespace cmajor::build
