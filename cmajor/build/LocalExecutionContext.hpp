// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_LOCAL_EXECUTION_CONTEXT_INCLUDED
#define CMAJOR_BUILD_LOCAL_EXECUTION_CONTEXT_INCLUDED
#include <cmajor/build/ExecutionContext.hpp>
#include <cmajor/build/SocketServer.hpp>
#include <cmajor/build/SocketClient.hpp>
#include <thread>

namespace cmajor { namespace build {

class BUILD_API LocalExecutionContext : public ExecutionContext
{
public:
    LocalExecutionContext();
    ~LocalExecutionContext();
    BuildClient* GetClient() override;
    Connection* GetConnection() override;
private:
    std::thread serverThread;
    std::unique_ptr<SocketServer> socketServer;
    std::unique_ptr<SocketClient> socketClient;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_LOCAL_EXECUTION_CONTEXT_INCLUDED
