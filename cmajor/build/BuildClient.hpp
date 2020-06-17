// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_CLIENT_INCLUDED
#define CMAJOR_BUILD_CLIENT_INCLUDED
#include <cmajor/build/Connection.hpp>
#include <cmajor/build/MessageHandler.hpp>
#include <cmajor/build/MessageBody.hpp>
#include <set>

namespace cmajor { namespace build {

class BUILD_API BuildClient : public Requester
{
public:
    enum class State
    {
        initialized,
        pushProjectRequestSent,
        pushProjectResponseReceived,
        pushProjectFileRequestSent,
        pushProjectFileResponseReceived,
        pushProjectFileContentRequestSent,
        pushProjectFileContentResponseReceived,
        buildProjectRequestSent,
        buildProjectResponseReceived,
        error
    };
    BuildClient(Connection* connection_);
    void PushProject(const std::string& projectFilePath, std::set<std::string>& pushedProjects);
    void Handle(PushProjectResponse& response) override;
    void Handle(PushProjectFileResponse& response) override;
    void Handle(PushProjectFileContentResponse& response) override;
    void BuildProject(const std::string& projectFilePath, const std::string& config, const std::string& toolChain, bool rebuild, bool only, bool printBuildOutput);
    void Handle(BuildProjectResponse& response) override;
    void Handle(ShowBuildMessageRequest& request) override;
    void Handle(ErrorResponse& response) override;
    void Handle(CloseConnectionRequest& request) override;
    std::string GetStateStr() const override;
    std::string Name() const override { return "buildclient"; }
    SourceFileInfo GetSourceFileInfo(const std::string& fileId) const;
    void SetDefaultToolChain(const std::string& defaultToolChain_);
private:
    Connection* connection;
    State state;
    ProjectInfo projectInfo;
    std::string projectInfoFilePath;
    std::string defaultToolChain;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_CLIENT_INCLUDED
