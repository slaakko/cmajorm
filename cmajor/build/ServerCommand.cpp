// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/ServerCommand.hpp>
#include <cmajor/build/ServerExecutionContext.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <cmajor/build/ServerConfig.hpp>
#include <cmajor/build/BuildLangLexer.hpp>
#include <cmajor/build/BuildLangServerParser.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>

namespace cmajor { namespace build {

using namespace soulng::util;
using namespace soulng::unicode;

std::unique_ptr<ExecutionContext> CreateServerExecutionContext(const std::string& serverName)
{
    if (serverName == "local")
    {
        throw std::runtime_error("cannot run 'local' server in server execution context");
    }
    return std::unique_ptr<ExecutionContext>(new ServerExecutionContext(serverName));
}

ServerCommand::ServerCommand() 
{
}

ServerCommand::~ServerCommand()
{
}

AddServerServerCommand::AddServerServerCommand(const std::string& serverName_, int port_) : serverName(serverName_), port(port_)
{
}

void AddServerServerCommand::Execute()
{
    bool force = false;
    if (GetBuildOption(BuildOptions::force))
    {
        force = true;
    }
    ServerConfig::Instance().Add(serverName, std::string(), port, std::string(), force, true, true);
    LogMessage(-1, "server '" + serverName + "' added");
}

RemoveServerServerCommand::RemoveServerServerCommand(const std::string& serverName_) : serverName(serverName_)
{
}

void RemoveServerServerCommand::Execute()
{
    ServerConfig::Instance().Remove(serverName);
    LogMessage(-1, "server '" + serverName + "' removed");
}

RunServerServerCommand::RunServerServerCommand(const std::string& serverName_) : serverName(serverName_)
{
}

void RunServerServerCommand::Execute()
{
    std::unique_ptr<ExecutionContext> context = CreateServerExecutionContext(serverName);
    while (true)
    {
        std::string commandStr;
        std::getline(std::cin, commandStr);
        if (commandStr == "exit")
        {
            context->ExitServer();
            std::cout << "exit." << std::endl;
            return;
        }
        else
        {
            std::cerr << "unknown command '" << commandStr << "', type 'exit' <ENTER> to stop and exit server." << std::endl;
        }
    }
}

ShowConfigurationServerCommand::ShowConfigurationServerCommand()
{
}

void ShowConfigurationServerCommand::Execute()
{
    ServerConfig::Instance().Show();
}

std::unique_ptr<ServerCommand> ParseServerCommand(const std::string& command)
{
    BuildLangLexer lexer(ToUtf32(command), "", 0);
    BuildOptionSetter optionSetter;
    return BuildLangServerParser::Parse(lexer, &optionSetter);
}

} } // namespace cmajor::build;
