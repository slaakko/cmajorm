// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_SERVER_INCLUDED
#define CMAJOR_BUILD_SERVER_INCLUDED
#include <cmajor/build/Connection.hpp>
#include <cmajor/build/MessageHandler.hpp>
#include <cmajor/build/MessageBody.hpp>
#include <soulng/util/Process.hpp>

namespace cmajor { namespace build {

class BUILD_API BuildServer : public RequestHandler
{
public:
    BuildServer(Connection* connection_);
    ~BuildServer();
    std::string Name() const override { return "buildserver"; }
    void Run();
    void Exit();
    void Handle(PushProjectRequest& request) override;
    void Handle(PushProjectFileRequest& request) override;
    void Handle(PushProjectFileContentRequest& request) override;
    void Handle(BuildProjectRequest& request) override;
    void Handle(ShowBuildMessageResponse& response) override;
    void Handle(CloseConnectionRequest& request) override;
    void SetConnection(Connection* connection_) { connection = connection_; }
    Connection* GetConnection() const { return connection; }
    void SendBuildMessages(Process* buildProcess);
private:
    std::string repositoryDir;
    Connection* connection;
    bool exiting;
};

void SetServer(BuildServer* server_);

void RunServer();

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SERVER_INCLUDED
