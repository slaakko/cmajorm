// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_CLIENT_EXECUTION_CONTEXT_INCLUDED
#define CMAJOR_BUILD_CLIENT_EXECUTION_CONTEXT_INCLUDED
#include <cmajor/build/ExecutionContext.hpp>
#include <cmajor/build/SocketClient.hpp>
#include <thread>

namespace cmajor { namespace build {

class BUILD_API ClientExecutionContext : public ExecutionContext
{
public:
    ClientExecutionContext(const std::string& serverName);
    ~ClientExecutionContext();
    BuildClient* GetClient() override;
    Connection* GetConnection() override;
private:
    std::unique_ptr<SocketClient> socketClient;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_CLIENT_EXECUTION_CONTEXT_INCLUDED
