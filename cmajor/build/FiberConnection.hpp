// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_FIBER_CONNECTION_INCLUDED
#define CMAJOR_BUILD_FIBER_CONNECTION_INCLUDED
#include <cmajor/build/Connection.hpp>

namespace cmajor { namespace build {

class BuildServer;

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
private:
    std::string messageStr;
    void* currentFiber;
    void* serverFiber;
    void* clientFiber;
    std::string client;
    std::string server;
};

BUILD_API std::unique_ptr<FiberConnection> CreateFiberConnection(Log* log);

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SINGLE_SERVER_CONNECTION_INCLUDED
