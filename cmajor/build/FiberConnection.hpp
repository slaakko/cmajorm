// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_FIBER_CONNECTION_INCLUDED
#define CMAJOR_BUILD_FIBER_CONNECTION_INCLUDED
#include <cmajor/build/Connection.hpp>
#include <cmajor/build/Host.hpp>

namespace cmajor { namespace build {

class BuildServer;

class BUILD_API FiberClient : public Host
{
public:
    FiberClient();
    const std::string& Name() const override;
    void Exit() override;
private:
    std::string name;
};

class BUILD_API FiberServer : public Host
{
public:
    FiberServer();
    const std::string& Name() const override;
    void Exit() override;
private:
    std::string name;
};

class BUILD_API FiberConnection : public Connection
{
public:
    FiberConnection(Log* log);
    ~FiberConnection();
    void SetFibers(void* serverFiber, void* clientFiber);
    void DoSend(JsonObject* messageObject) override;
    std::unique_ptr<JsonObject> DoReceive() override;
    void DoClose() override;
    void Switch();
    const std::string& GetActor() const override;
    Host* GetHost() const override;
private:
    std::string messageStr;
    void* currentFiber;
    void* serverFiber;
    void* clientFiber;
    FiberClient client;
    FiberServer server;
};

BUILD_API std::unique_ptr<FiberConnection> CreateFiberConnection(Log* log);

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SINGLE_SERVER_CONNECTION_INCLUDED
