// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/BuildServer.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/build/Message.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <cmajor/build/FiberConnection.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <sngcm/ast/Project.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>

namespace cmajor { namespace build {

using namespace sngcm::ast;
using namespace soulng::util;
using namespace cmajor::symbols;

std::string CmajorRepositoryDir()
{
    return GetFullPath(Path::Combine(CmajorRootDir(), "repository"));
}

BuildServer::BuildServer(Connection* connection_) : connection(connection_), exiting(false)
{
    repositoryDir = CmajorRepositoryDir();
    boost::filesystem::create_directories(repositoryDir);
    if (connection)
    {
        connection->SetServer(this);
        connection->SetServerAlive(true);
    }
}

BuildServer::~BuildServer()
{
    if (connection)
    {
        connection->SetServer(nullptr);
        connection->SetServerAlive(false);
    }
}

void BuildServer::Run()
{
    try
    {
        while (!exiting)
        {
            std::unique_ptr<MessageBase> message = connection->Receive();
            message->DispatchTo(*this);
        }
    }
    catch (const std::exception& ex)
    {
        LogMessage(-1, "buildserver got exception: " + std::string(ex.what()));
        if (connection)
        {
            ErrorResponse errorResponse;
            errorResponse.body.error = "buildserver got exception: " + std::string(ex.what());
            errorResponse.SendTo(*connection);
            connection->SetServer(nullptr);
            connection->SetServerAlive(false);
        }
        exiting = true;
        throw;
    }
}

void BuildServer::Exit()
{
    exiting = true;
}

void BuildServer::Handle(PushProjectRequest& request)
{
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: received " + std::string(request.Id()));
    }
    PushProjectResponse pushProjectResponse;
    pushProjectResponse.body.upToDate = false;
    try
    {
        pushProjectResponse.body.projectId = request.body.projectId;
        if (request.body.projectName == "System.Windows" || request.body.projectName == "System.Windows.Install")
        {
            throw std::runtime_error("System.Windows and System.Windows.Install projects not supported (at least yet)");
        }
        bool dependenciesFound = true;
        for (const std::string& dependsOnProjectId : request.body.dependsOnProjects)
        {
            std::string projectDir = Path::Combine(repositoryDir, dependsOnProjectId);
            if (!boost::filesystem::exists(projectDir))
            {
                dependenciesFound = false;
                pushProjectResponse.body.error = "dependent project '" + dependsOnProjectId + "' of project '" + pushProjectResponse.body.projectId + "' not found from repository";
                break;
            }
        }
        std::string projectDir = Path::Combine(repositoryDir, pushProjectResponse.body.projectId);
        if (dependenciesFound)
        {
            if (boost::filesystem::exists(projectDir))
            {
                std::string projectInfoFilePath = Path::Combine(projectDir, pushProjectResponse.body.projectId + ".json");
                if (boost::filesystem::exists(projectInfoFilePath))
                {
                    std::string projectInfoFileContent = ReadFile(projectInfoFilePath);
                    std::u32string projectInfoContent = ToUtf32(projectInfoFileContent);
                    JsonLexer lexer(projectInfoContent, projectInfoFilePath, 0);
                    std::unique_ptr<JsonValue> jsonValue = JsonParser::Parse(lexer);
                    if (jsonValue->Type() == JsonValueType::object)
                    {
                        JsonObject* projectFileInfo = static_cast<JsonObject*>(jsonValue.get());
                        ProjectInfo projectInfo = ProjectInfo(projectFileInfo);
                        if (projectInfo.projectId == pushProjectResponse.body.projectId)
                        {
                            if (projectInfo.projectHash == request.body.projectHash)
                            {
                                pushProjectResponse.body.upToDate = true;
                            }
                            else
                            {
                                pushProjectResponse.body.info = "project '" + pushProjectResponse.body.projectId + "' hash does not match (project changed)";
                            }
                        }
                        else
                        {
                            throw std::runtime_error("invalid project info in project '" + projectDir + "' (" + projectInfoFilePath + "): project id '" + projectInfo.projectId +
                                "' does not match requested project id '" + pushProjectResponse.body.projectId + "'");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("invalid project info in project '" + projectDir + "' (" + projectInfoFilePath + "): JSON object expected");
                    }
                }
                else
                {
                    pushProjectResponse.body.info = "project info file '" + projectInfoFilePath + "' not found";
                }
            }
            else
            {
                pushProjectResponse.body.info = "project directory '" + projectDir + "' not found";
            }
        }
        else
        {
            pushProjectResponse.body.info = "all dependencies of project '" + pushProjectResponse.body.projectId + "' not found";
        }
    }
    catch (const std::exception& ex)
    {
        pushProjectResponse.body.error = "buildserver: error processing request '" + std::string(request.Id()) + ": exception '" + std::string(ex.what()) + "'";
    }
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: sending " + std::string(pushProjectResponse.Id()) + "[projectId=" + pushProjectResponse.body.projectId + ", upToDate=" + BoolStr(pushProjectResponse.body.upToDate) + "]");
    }
    pushProjectResponse.SendTo(*connection);
}

void BuildServer::Handle(PushProjectFileRequest& request)
{
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: received " + std::string(request.Id()));
    }
    PushProjectFileResponse pushProjectFileResponse;
    pushProjectFileResponse.body.projectId = request.body.projectId;
    try
    {
        std::string projectDir = Path::Combine(repositoryDir, pushProjectFileResponse.body.projectId);
        if (!boost::filesystem::exists(projectDir))
        {
            boost::filesystem::create_directories(projectDir);
        }
        ProjectInfo prevProjectInfo;
        std::string projectInfoFilePath = Path::Combine(projectDir, pushProjectFileResponse.body.projectId + ".json");
        if (boost::filesystem::exists(projectInfoFilePath))
        {
            prevProjectInfo = ReadPojectInfo(nullptr, projectInfoFilePath);
        }
        std::string tempProjectInfoFilePath = projectInfoFilePath + ".tmp";
        if (boost::filesystem::exists(tempProjectInfoFilePath))
        {
            boost::filesystem::remove(tempProjectInfoFilePath);
        }
        {
            std::ofstream tempProjectInfoFile(tempProjectInfoFilePath);
            CodeFormatter projectInfoFormatter(tempProjectInfoFile);
            std::unique_ptr<soulng::util::JsonValue> projectInfoValue = request.body.ToJson();
            projectInfoValue->Write(projectInfoFormatter);
        }
        std::string projectFilePath = Path::Combine(projectDir, Path::GetFileName(request.body.projectFilePath));
        std::string tempProjectFilePath = projectFilePath.append(".tmp");
        if (boost::filesystem::exists(tempProjectFilePath))
        {
            boost::filesystem::remove(tempProjectFilePath);
        }
        cmajor::symbols::BackEnd symbolsBackend = cmajor::symbols::GetBackEnd();
        sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::cppcm;
        if (symbolsBackend == cmajor::symbols::BackEnd::llvm)
        {
            backend = sngcm::ast::BackEnd::llvm;
        }
        else if (symbolsBackend == cmajor::symbols::BackEnd::cmsx)
        {
            backend = sngcm::ast::BackEnd::cmsx;
        }
        sngcm::ast::Target target(sngcm::ast::Target::program);
        target = ParseTarget(request.body.target);
        std::unique_ptr<Project> project(new Project(ToUtf32(request.body.projectName), tempProjectFilePath, GetConfig(), backend, GetToolChain()));
        project->AddDeclaration(new TargetDeclaration(target));
        for (const std::string& dependentProjectId : request.body.dependsOnProjects)
        {
            std::string dependentProjectInfoFilePath = Path::Combine(Path::Combine(repositoryDir, dependentProjectId), dependentProjectId + ".json");
            if (!boost::filesystem::exists(dependentProjectInfoFilePath))
            {
                throw std::runtime_error("dependent project info file '" + dependentProjectInfoFilePath + "' does not exist");
            }
            ProjectInfo dependentProjectInfo = ReadPojectInfo(nullptr, dependentProjectInfoFilePath);
            std::string relativeReferencedProjectFilePath = Path::Combine(Path::Combine("..", dependentProjectId), Path::GetFileName(dependentProjectInfo.projectFilePath));
            std::unique_ptr<ReferenceDeclaration> referenceDeclaration(new ReferenceDeclaration(relativeReferencedProjectFilePath));
            project->AddDeclaration(referenceDeclaration.release());
        }
        std::set<std::string> sourceFileNames;
        for (const SourceFileInfo& fileInfo : request.body.fileInfos)
        {
            std::string sourceFileName = Path::GetFileName(fileInfo.filePath);
            if (sourceFileNames.find(sourceFileName) != sourceFileNames.cend())
            {
                throw std::runtime_error("source file name '" + sourceFileName + "' not unique");
            }
            sourceFileNames.insert(sourceFileName);
            std::unique_ptr<SourceFileDeclaration> sourceFileDeclaration(new SourceFileDeclaration(sourceFileName));
            project->AddDeclaration(sourceFileDeclaration.release());
        }
        project->Write(tempProjectFilePath);
        for (SourceFileInfo& fileInfo : request.body.fileInfos)
        {
            bool found = false;
            bool hashMatches = false;
            for (const SourceFileInfo& prevFileInfo : prevProjectInfo.fileInfos)
            {
                if (prevFileInfo.fileId == fileInfo.fileId)
                {
                    found = true;
                    if (prevFileInfo.fileHash == fileInfo.fileHash)
                    {
                        hashMatches = true;
                    }
                    break;
                }
            }
            if (!found)
            {
                fileInfo.info = "file not found";
                pushProjectFileResponse.body.fileInfos.push_back(fileInfo);
            }
            else if (!hashMatches)
            {
                fileInfo.info = "file hash does not match (content changed)";
                pushProjectFileResponse.body.fileInfos.push_back(fileInfo);
            }
        }
    }
    catch (const std::exception& ex)
    {
        pushProjectFileResponse.body.error = "buildserver: error processing request '" + std::string(request.Id()) + ": exception '" + std::string(ex.what()) + "'";
    }
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: sending " + std::string(pushProjectFileResponse.Id()) + "[projectId=" + pushProjectFileResponse.body.projectId + "] with " +
            std::to_string(pushProjectFileResponse.body.fileInfos.size()) + " file ids");
    }
    pushProjectFileResponse.SendTo(*connection);
}

void BuildServer::Handle(PushProjectFileContentRequest& request)
{
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: received " + std::string(request.Id()));
    }
    PushProjectFileContentResponse pushProjectFileContentResponse;
    pushProjectFileContentResponse.body.projectId = request.body.projectId;
    pushProjectFileContentResponse.body.ok = false;
    try
    {
        std::string projectDir = Path::Combine(repositoryDir, request.body.projectId);
        if (!boost::filesystem::exists(projectDir))
        {
            throw std::runtime_error("project directory does not exist");
        }
        std::string projectInfoFilePath = Path::Combine(projectDir, pushProjectFileContentResponse.body.projectId + ".json");
        std::string tempProjectInfoFilePath = projectInfoFilePath + ".tmp";
        if (!boost::filesystem::exists(tempProjectInfoFilePath))
        {
            throw std::runtime_error("temporary project info file does not exist");
        }
        ProjectInfo tempProjectInfo = ReadPojectInfo(nullptr, tempProjectInfoFilePath);
        for (const SourceFileInfo& fileInfo : request.body.fileInfos)
        {
            std::string sourceFileName = Path::GetFileName(fileInfo.filePath);
            std::string tempSourceFilePath = Path::Combine(projectDir, sourceFileName + ".tmp");
            if (boost::filesystem::exists(tempSourceFilePath))
            {
                boost::filesystem::remove(tempSourceFilePath);
            }
            BinaryWriter writer(tempSourceFilePath);
            writer.Write(fileInfo.content, false);
        }
        for (const SourceFileInfo& fileInfo : request.body.fileInfos)
        {
            std::string sourceFileName = Path::GetFileName(fileInfo.filePath);
            std::string sourceFilePath = Path::Combine(projectDir, sourceFileName);
            std::string tempSourceFilePath = Path::Combine(projectDir, sourceFileName + ".tmp");
            if (boost::filesystem::exists(sourceFilePath))
            {
                boost::filesystem::remove(sourceFilePath);
            }
            boost::filesystem::rename(tempSourceFilePath, sourceFilePath);
        }
        std::string projectFileName = Path::GetFileName(tempProjectInfo.projectFilePath);
        std::string projectFilePath = Path::Combine(projectDir, projectFileName);
        std::string tempProjectFilePath = projectFilePath + ".tmp";
        if (boost::filesystem::exists(projectFilePath))
        {
            boost::filesystem::remove(projectFilePath);
        }
        boost::filesystem::rename(tempProjectFilePath, projectFilePath);
        if (boost::filesystem::exists(projectInfoFilePath))
        {
            boost::filesystem::remove(projectInfoFilePath);
        }
        boost::filesystem::rename(tempProjectInfoFilePath, projectInfoFilePath);
        pushProjectFileContentResponse.body.ok = true;
    }
    catch (const std::exception& ex)
    {
        pushProjectFileContentResponse.body.error = "buildserver: error processing request '" + std::string(request.Id()) + ": exception '" + std::string(ex.what()) + "'";
    }
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: sending " + std::string(pushProjectFileContentResponse.Id()) + "[projectId=" + pushProjectFileContentResponse.body.projectId +
            ", ok=" + BoolStr(pushProjectFileContentResponse.body.ok) + "]");
    }
    pushProjectFileContentResponse.SendTo(*connection);
}

void BuildServer::Handle(BuildProjectRequest& request)
{
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: received " + std::string(request.Id()));
    }
    BuildProjectResponse buildProjectResponse;
    buildProjectResponse.body.projectId = request.body.projectId;
    try
    {
        std::string projectDir = Path::Combine(repositoryDir, request.body.projectId);
        if (!boost::filesystem::exists(projectDir))
        {
            throw std::runtime_error("project directory does not exist");
        }
        std::string projectInfoFilePath = Path::Combine(projectDir, buildProjectResponse.body.projectId + ".json");
        if (!boost::filesystem::exists(projectInfoFilePath))
        {
            throw std::runtime_error("project info file not found");
        }
        ProjectInfo projectInfo = ReadPojectInfo(nullptr, projectInfoFilePath);
        std::string projectFileName = Path::GetFileName(projectInfo.projectFilePath);
        std::string projectFilePath = Path::Combine(projectDir, projectFileName);
        std::string buildCommand;
        buildCommand.append("cppcmc");
        if (request.body.sendBuildOutput)
        {
            buildCommand.append(" --verbose");
        }
        if (!request.body.config.empty())
        {
            buildCommand.append(" --config=").append(request.body.config);
        }
        if (!request.body.toolChain.empty())
        {
            buildCommand.append(" --tool-chain=").append(request.body.toolChain);
        }
        buildCommand.append(" --all");
        buildCommand.append(" ").append(QuotedPath(projectFilePath));
        buildProcess.reset(new Process(buildCommand));
        std::string errors;
        errors = buildProcess->ReadToEnd(soulng::util::Process::StdHandle::std_err);
        buildProcess->WaitForExit();
        if (buildProcess->ExitCode() != 0)
        {
            throw std::runtime_error("execution of build command '" + buildCommand + "' failed with exit code " + std::to_string(buildProcess->ExitCode()) + ": " + errors);
        }
        else
        {
            buildProjectResponse.body.info.append("execution of build command '" + buildCommand + "' succeeded");
        }
    }
    catch (const std::exception& ex)
    {
        buildProjectResponse.body.error = "buildserver: error processing request '" + std::string(request.Id()) + ": exception '" + std::string(ex.what()) + "'";
    }
    buildProjectResponse.SendTo(*connection);
}

void BuildServer::Handle(CloseConnectionRequest& request)
{
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: received " + std::string(request.Id()));
    }
    connection->Close();
    if (GetGlobalFlag(GlobalFlags::printDebugMessages))
    {
        LogMessage(-1, "buildserver: connection closed");
    }
}

BuildServer* server = nullptr;
Connection* connection = nullptr;

void SetServer(BuildServer* server_)
{
    server = server_;
}

void RunServer()
{
    try
    {
        if (server)
        {
            connection = server->GetConnection();
            server->Run();
            if (connection)
            {
                delete connection;
            }
            connection = nullptr;
            server = nullptr;
        }
        else
        {
            throw std::runtime_error("runserver: server ptr is null");
        }
    }
    catch (const std::exception&)
    {
        if (connection)
        {
            delete connection;
        }
        connection = nullptr;
        server = nullptr;
    }
}

} } // namespace cmajor::build
