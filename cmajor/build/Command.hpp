// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_COMMAND_INCLUDED
#define CMAJOR_BUILD_COMMAND_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <cmajor/build/Connection.hpp>
#include <string>
#include <memory>

namespace cmajor { namespace build {

class ExecutionContext;

std::unique_ptr<ExecutionContext> CreateExecutionContext(const std::string& serverName);

class BUILD_API Command
{
public:
    Command();
    virtual ~Command();
    virtual void Execute() = 0;
private:
};

class BUILD_API PushProjectCommand : public Command
{
public:
    PushProjectCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API RemoveProjectCommand : public Command
{
public:
    RemoveProjectCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API BuildProjectCommand : public Command
{
public:
    BuildProjectCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API DebugProjectCommand : public Command
{
public:
    DebugProjectCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API InstallProjectCommand : public Command
{
public:
    InstallProjectCommand(const std::string& projectFilePath_, const std::string& directory_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string directory;
    std::string serverName;
};

BUILD_API std::unique_ptr<Command> ParseCommand(const std::string& command);

} } // namespace cmajor::build;

#endif // CMAJOR_BUILD_COMMAND_INCLUDED
