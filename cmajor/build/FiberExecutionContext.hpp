// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_FIBER_EXECUTION_CONTEXT_INCLUDED
#define CMAJOR_BUILD_FIBER_EXECUTION_CONTEXT_INCLUDED
#include <cmajor/build/ExecutionContext.hpp>

namespace cmajor { namespace build {

class BuildServer;
class FiberConnection;
class Connection;

class BUILD_API FiberExecutionContext : public ExecutionContext
{
public:
    FiberExecutionContext();
    ~FiberExecutionContext();
    BuildClient* GetClient() override { return client.get(); }
    Connection* GetConnection() override;
private:
    std::unique_ptr<FiberConnection> connection;
    std::unique_ptr<BuildClient> client;
    std::unique_ptr<BuildServer> server;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_FIBER_EXECUTION_CONTEXT_INCLUDED
