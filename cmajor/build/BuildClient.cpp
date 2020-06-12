// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/BuildClient.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Log.hpp>

namespace cmajor { namespace build {

using namespace cmajor::symbols;

BuildClient::BuildClient(Connection* connection_) : connection(connection_), state(State::initialized)
{
}

void BuildClient::PushProject(const std::string& projectFilePath, std::set<std::string>& pushedProjects)
{
    if (pushedProjects.find(projectFilePath) != pushedProjects.cend()) return;
    pushedProjects.insert(projectFilePath);
    state = State::initialized;
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildclient: pushing project '" + projectFilePath + "' to build server...");
    }
    std::unique_ptr<Project> project = ReadProject(projectFilePath);
    if (project->Name() == U"System.Windows" || project->Name() == U"System.Windows.Install")
    {
        throw std::runtime_error("'System.Windows' and 'System.Windows.Install' projects not supported (at least yet)");
    }
    if (!GetBuildOption(BuildOptions::only))
    {
        for (const std::string& referencedProjectFilePath : project->ReferencedProjectFilePaths())
        {
            PushProject(referencedProjectFilePath, pushedProjects);
        }
    }
    projectInfoFilePath = Path::ChangeExtension(projectFilePath, ".json");
    projectInfo = ReadPojectInfo(project.get(), projectInfoFilePath);
    PushProjectRequest pushProjectRequest;
    pushProjectRequest.body.projectName = projectInfo.projectName;
    pushProjectRequest.body.projectFilePath = projectInfo.projectFilePath;
    pushProjectRequest.body.projectHash = projectInfo.projectHash;
    pushProjectRequest.body.projectId = projectInfo.projectId;
    pushProjectRequest.body.target = projectInfo.target;
    pushProjectRequest.body.dependsOnProjects = projectInfo.dependsOnProjects;
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildclient: sending " + std::string(pushProjectRequest.Id()) + "[projectId=" + pushProjectRequest.body.projectId + "] to build server...");
    }
    pushProjectRequest.SendTo(*connection);
    state = State::pushProjectRequestSent;
    std::unique_ptr<MessageBase> response = connection->Receive();
    response->DispatchTo(*this);
}

void BuildClient::Handle(PushProjectResponse& response)
{
    if (state == State::pushProjectRequestSent)
    {
        state = State::pushProjectResponseReceived;
        if (GetGlobalFlag(GlobalFlags::printDebugMessages))
        {
            LogMessage(-1, "buildclient: received " + std::string(response.Id()) + "[projectId=" + response.body.projectId + ", upToDate=" + BoolStr(response.body.upToDate) + "] from build server");
        }
        if (!response.body.error.empty())
        {
            throw std::runtime_error("buildclient: " + response.body.error);
        }
        if (projectInfo.projectId != response.body.projectId)
        {
            throw std::runtime_error("buildclient: project ID mismatch in " + std::string(response.Id()) + " message from buildserver: " +
                "project info project id=" + projectInfo.projectId + ", response project id=" + response.body.projectId);
        }
        if (response.body.upToDate)
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(-1, "buildclient: project '" + projectInfo.projectFilePath + "' is up-to-date");
            }
        }
        else
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(-1, "buildclient: project '" + projectInfo.projectFilePath + "' changed (info: " + response.body.info + ")");
            }
            PushProjectFileRequest pushProjectFileRequest;
            pushProjectFileRequest.body.projectFilePath = projectInfo.projectFilePath;
            pushProjectFileRequest.body.projectHash = projectInfo.projectHash;
            pushProjectFileRequest.body.projectId = projectInfo.projectId;
            pushProjectFileRequest.body.projectName = projectInfo.projectName;
            pushProjectFileRequest.body.target = projectInfo.target;
            pushProjectFileRequest.body.dependsOnProjects = projectInfo.dependsOnProjects;
            for (const SourceFileInfo& fileInfo : projectInfo.fileInfos)
            {
                pushProjectFileRequest.body.fileInfos.push_back(fileInfo);
            }
            if (GetGlobalFlag(GlobalFlags::printDebugMessages))
            {
                LogMessage(-1, "buildclient: sending " + std::string(pushProjectFileRequest.Id()) + " with " + std::to_string(projectInfo.fileInfos.size()) + " file infos to build server...");
            }
            pushProjectFileRequest.SendTo(*connection);
            state = State::pushProjectFileRequestSent;
            std::unique_ptr<MessageBase> response = connection->Receive();
            response->DispatchTo(*this);
        }
    }
    else
    {
        throw std::runtime_error("buildclient: unexpected " + std::string(response.Id()) + " received: state=" + GetStateStr());
    }
}

void BuildClient::Handle(PushProjectFileResponse& response)
{
    if (state == State::pushProjectFileRequestSent)
    {
        state = State::pushProjectFileResponseReceived;
        if (GetGlobalFlag(GlobalFlags::printDebugMessages))
        {
            LogMessage(-1, "buildclient: received " + std::string(response.Id()) + "[projectId=" + response.body.projectId + "] with " + std::to_string(response.body.fileInfos.size()) + " file infos from build server");
        }
        if (!response.body.error.empty())
        {
            throw std::runtime_error("buildclient: " + response.body.error);
        }
        if (projectInfo.projectId != response.body.projectId)
        {
            throw std::runtime_error("buildclient: project ID mismatch in " + std::string(response.Id()) + " message from buildserver: " +
                "project info project id=" + projectInfo.projectId + ", response project id=" + response.body.projectId);
        }
        PushProjectFileContentRequest pushProjectFileContentRequest;
        pushProjectFileContentRequest.body.projectFilePath = projectInfo.projectFilePath;
        pushProjectFileContentRequest.body.projectHash = projectInfo.projectHash;
        pushProjectFileContentRequest.body.projectId = projectInfo.projectId;
        pushProjectFileContentRequest.body.projectName = projectInfo.projectName;
        pushProjectFileContentRequest.body.target = projectInfo.target;
        pushProjectFileContentRequest.body.dependsOnProjects = projectInfo.dependsOnProjects;
        for (const SourceFileInfo& responseSourceFileInfo : response.body.fileInfos)
        {
            if (!responseSourceFileInfo.info.empty())
            {
                if (GetGlobalFlag(GlobalFlags::printDebugMessages))
                {
                    LogMessage(-1, "buildclient: server requested file '" + responseSourceFileInfo.fileId + "' because " + responseSourceFileInfo.info);
                }
            }
            else
            {
                if (GetGlobalFlag(GlobalFlags::printDebugMessages))
                {
                    LogMessage(-1, "buildclient: server requested file '" + responseSourceFileInfo.fileId + "'");
                }
            }
            const std::string& fileId = responseSourceFileInfo.fileId;
            SourceFileInfo sourceFileInfoWithFileContent = GetSourceFileInfo(fileId);
            pushProjectFileContentRequest.body.fileInfos.push_back(sourceFileInfoWithFileContent);
        }
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(-1, "buildclient: " + std::to_string(response.body.fileInfos.size()) + " files changed or not found from server");
        }
        if (GetGlobalFlag(GlobalFlags::printDebugMessages))
        {
            LogMessage(-1, "buildclient: sending " + std::string(pushProjectFileContentRequest.Id()) + " with " + std::to_string(pushProjectFileContentRequest.body.fileInfos.size()) + " file contents to build server...");
        }
        pushProjectFileContentRequest.SendTo(*connection);
        state = State::pushProjectFileContentRequestSent;
        std::unique_ptr<MessageBase> response = connection->Receive();
        response->DispatchTo(*this);
    }
    else
    {
        throw std::runtime_error("buildclient: unexpected " + std::string(response.Id()) + " received: state=" + GetStateStr());
    }
}

void BuildClient::Handle(PushProjectFileContentResponse& response)
{
    if (state == State::pushProjectFileContentRequestSent)
    {
        state = State::pushProjectFileContentResponseReceived;
        if (GetGlobalFlag(GlobalFlags::printDebugMessages))
        {
            std::string error;
            if (!response.body.ok)
            {
                error = " with error '" + response.body.error;
            }
            LogMessage(-1, "buildclient: received " + std::string(response.Id()) + "[ok=" + BoolStr(response.body.ok) + "] " + error + " from build server");
        }
        if (projectInfo.projectId != response.body.projectId)
        {
            throw std::runtime_error("buildclient: project ID mismatch in " + std::string(response.Id()) + " message from buildserver: " +
                "project info project id=" + projectInfo.projectId + ", response project id=" + response.body.projectId);
        }
        if (response.body.ok)
        {
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                LogMessage(-1, "buildclient: project '" + projectInfo.projectFilePath + "' push successful");
            }
        }
        else
        {
            throw std::runtime_error("buildclient: project '" + projectInfo.projectFilePath + "' push to build server failed with error '" + response.body.error + "'");
        }
    }
    else
    {
        throw std::runtime_error("buildclient: unexpected " + std::string(response.Id()) + " received: state=" + GetStateStr());
    }
}

void BuildClient::Handle(ErrorResponse& response)
{
    state = State::error;
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(-1, "buildclient got error response from buildserver: \"" + response.body.error + "\"");
    }
    throw std::runtime_error("buildclient: got error response from buildserver: \"" + response.body.error + "\"");
}

void BuildClient::Handle(CloseConnectionRequest& request)
{
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildclient: received " + std::string(request.Id()));
    }
    connection->Close();
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildclient: connection closed");
    }
}

SourceFileInfo BuildClient::GetSourceFileInfo(const std::string& fileId) const
{
    for (const SourceFileInfo& sourceFileInfo : projectInfo.fileInfos)
    {
        if (sourceFileInfo.fileId == fileId)
        {
            SourceFileInfo sourceFileInfoWithContent;
            sourceFileInfoWithContent.fileId = sourceFileInfo.fileId;
            sourceFileInfoWithContent.filePath = sourceFileInfo.filePath;
            sourceFileInfoWithContent.fileHash = sourceFileInfo.fileHash;
            sourceFileInfoWithContent.content = ReadFile(sourceFileInfoWithContent.filePath);
            return sourceFileInfoWithContent;
        }
    }
    throw std::runtime_error("buildclient: source file info for build server response file id '" + fileId + "' not found from '" + projectInfoFilePath + "'");
}

std::string BuildClient::GetStateStr() const
{
    switch (state)
    {
        case State::initialized:
        {
            return "initialized";
        }
        case State::pushProjectRequestSent:
        {
            return "push-project-request-sent";
        }
        case State::pushProjectResponseReceived:
        {
            return "push-project-response-received";
        }
        case State::pushProjectFileRequestSent:
        {
            return "push-project-file-request-sent";
        }
        case State::pushProjectFileResponseReceived:
        {
            return "push-project-file-response-received";
        }
        case State::pushProjectFileContentRequestSent:
        {
            return "push-project-file-content-request-sent";
        }
        case State::pushProjectFileContentResponseReceived:
        {
            return "push-project-file-content-response-received";
        }
        case State::error:
        {
            return "error-received";
        }
    }
    return "unknown state";
}

} } // namespace cmajor::build
