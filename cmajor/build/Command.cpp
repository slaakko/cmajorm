// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/Command.hpp>
#include <cmajor/build/BuildLangLexer.hpp>
#include <cmajor/build/BuildLangClientParser.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <cmajor/build/Log.hpp>
#include <cmajor/build/BuildClient.hpp>
#include <cmajor/build/BuildServer.hpp>
#include <cmajor/build/ServerConfig.hpp>
#include <cmajor/build/FiberExecutionContext.hpp>
#include <cmajor/build/LocalExecutionContext.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Fiber.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Log.hpp>

namespace cmajor { namespace build {

using namespace soulng::unicode;
using namespace cmajor::symbols;

std::string GetFilePath(const std::string& path)
{
    Assert(path.size() >= 2, "invalid path");
    Assert(path[0] == '<' && path[path.size() - 1] == '>', "invalid path");
    std::string p = path.substr(1, path.size() - 2);
    return GetFullPath(p);
}

std::unique_ptr<ExecutionContext> CreateExecutionContext(const std::string& serverName)
{
    if (serverName.empty())
    {
        return std::unique_ptr<ExecutionContext>(new FiberExecutionContext());
    }
    else if (serverName == "local")
    {
        return std::unique_ptr<ExecutionContext>(new LocalExecutionContext());
    }
    else
    {
        throw std::runtime_error("client execution context not implemented yet");
    }
}

Command::Command()
{
}

Command::~Command()
{
}

PushProjectCommand::PushProjectCommand(const std::string& projectFilePath_, const std::string& serverName_) : projectFilePath(GetFilePath(projectFilePath_)), serverName(serverName_)
{
}

void PushProjectCommand::Execute()
{
    std::set<std::string> pushedProjects;
    std::unique_ptr<ExecutionContext> context = CreateExecutionContext(serverName);
    context->GetClient()->PushProject(projectFilePath, pushedProjects);
    Connection* connection = context->GetConnection();
    if (connection)
    {
        if (GetGlobalFlag(GlobalFlags::printDebugMessages))
        {
            LogMessage(-1, "buildclient: closing connection");
        }
        if (connection->ServerAlive())
        {
            connection->SetServerAlive(false);
            CloseConnectionRequest closeConnectionRequest;
            closeConnectionRequest.SendTo(*connection);
        }
        connection->Close();
    }
    if (serverName.empty())
    {
        LogMessage(-1, "project '" + projectFilePath + "' pushed to local build repository");
    }
    else
    {
        LogMessage(-1, "project '" + projectFilePath + "' pushed to server '" + serverName + "'");
    }
}

RemoveProjectCommand::RemoveProjectCommand(const std::string& projectFilePath_, const std::string& serverName_) : projectFilePath(GetFilePath(projectFilePath_)), serverName(serverName_)
{
}

void RemoveProjectCommand::Execute()
{
}

RemoveServerCommand::RemoveServerCommand(const std::string& serverName_) : serverName(serverName_)
{
}

void RemoveServerCommand::Execute()
{
    ServerConfig::Instance().Remove(serverName);
    LogMessage(-1, "server '" + serverName + "' removed");
}

AddServerCommand::AddServerCommand(const std::string& serverName_, const std::string& host_, int port_, const std::string& defaultToolChain_) :
    serverName(serverName_), host(host_), port(port_), defaultToolChain(defaultToolChain_)
{
}

void AddServerCommand::Execute()
{
    bool force = false;
    if (GetBuildOption(BuildOptions::force))
    {
        force = true;
    }
    ServerConfig::Instance().Add(serverName, host, port, defaultToolChain, force, true, true);
    LogMessage(-1, "server '" + serverName + "' added");
}

BuildProjectCommand::BuildProjectCommand(const std::string& projectFilePath_, const std::string& serverName_) : projectFilePath(GetFilePath(projectFilePath_)), serverName(serverName_)
{
}

void BuildProjectCommand::Execute()
{
    std::unique_ptr<ExecutionContext> context = CreateExecutionContext(serverName);
    std::string config = buildConfig;
    std::string toolChain = buildToolChain;
    bool printBuildOutput = GetBuildOption(BuildOptions::messages);
    bool rebuild = GetBuildOption(BuildOptions::rebuild);
    bool only = GetBuildOption(BuildOptions::only);
    context->GetClient()->BuildProject(projectFilePath, config, toolChain, rebuild, only, printBuildOutput);
    Connection* connection = context->GetConnection();
    if (connection)
    {
        if (GetGlobalFlag(GlobalFlags::printDebugMessages))
        {
            LogMessage(-1, "buildclient: closing connection");
        }
        if (connection->ServerAlive())
        {
            connection->SetServerAlive(false);
            CloseConnectionRequest closeConnectionRequest;
            closeConnectionRequest.SendTo(*connection);
        }
        connection->Close();
    }
    if (serverName.empty())
    {
        LogMessage(-1, "project '" + projectFilePath + "' built using local build repository");
    }
    else
    {
        LogMessage(-1, "project '" + projectFilePath + "' built using server '" + serverName + "'");
    }
}

DebugProjectCommand::DebugProjectCommand(const std::string& projectFilePath_, const std::string& serverName_) : projectFilePath(GetFilePath(projectFilePath_)), serverName(serverName_)
{
}

void DebugProjectCommand::Execute()
{
}

InstallProjectCommand::InstallProjectCommand(const std::string& projectFilePath_, const std::string& directory_, const std::string& serverName_) :
    projectFilePath(GetFilePath(projectFilePath_)), directory(GetFilePath(directory_)), serverName(serverName_)
{
}

void InstallProjectCommand::Execute()
{
}

ShowConfigurationCommand::ShowConfigurationCommand() 
{
}

void ShowConfigurationCommand::Execute()
{
    ServerConfig::Instance().Show();
}

std::unique_ptr<Command> ParseCommand(const std::string& command)
{
    BuildLangLexer lexer(ToUtf32(command), "", 0);
    BuildOptionSetter optionSetter;
    return BuildLangClientParser::Parse(lexer, &optionSetter);
}

} } // namespace cmajor::build;
