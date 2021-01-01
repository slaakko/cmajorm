// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_SERVER_EXECUTION_CONTEXT_INCLUDED
#define CMAJOR_BUILD_SERVER_EXECUTION_CONTEXT_INCLUDED
#include <cmajor/build/ExecutionContext.hpp>
#include <cmajor/build/SocketServer.hpp>
#include <thread>

namespace cmajor { namespace build {

class BUILD_API ServerExecutionContext : public ExecutionContext
{
public:
    ServerExecutionContext(const std::string& serverName);
    ~ServerExecutionContext();
    void ExitServer() override;
private:
    std::thread serverThread;
    std::unique_ptr<SocketServer> socketServer;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SERVER_EXECUTION_CONTEXT_INCLUDED
