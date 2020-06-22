// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_CLIENT_COMMAND_INCLUDED
#define CMAJOR_BUILD_CLIENT_COMMAND_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <cmajor/build/Connection.hpp>
#include <string>
#include <memory>

namespace cmajor { namespace build {

class ExecutionContext;

BUILD_API std::unique_ptr<ExecutionContext> CreateExecutionContext(const std::string& serverName);

class BUILD_API ClientCommand
{
public:
    ClientCommand();
    virtual ~ClientCommand();
    virtual void Execute() = 0;
};

class BUILD_API PushProjectClientCommand : public ClientCommand
{
public:
    PushProjectClientCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API RemoveProjectClientCommand : public ClientCommand
{
public:
    RemoveProjectClientCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API RemoveServerClientCommand : public ClientCommand
{
public:
    RemoveServerClientCommand(const std::string& serverName_);
    void Execute() override;
private:
    std::string serverName;
};

class BUILD_API AddServerClientCommand : public ClientCommand
{
public:
    AddServerClientCommand(const std::string& serverName_, const std::string& host_, int port_, const std::string& defaultToolChain_);
    void Execute() override;
private:
    std::string serverName;
    std::string host;
    int port;
    std::string defaultToolChain;
};

class BUILD_API BuildProjectClientCommand : public ClientCommand
{
public:
    BuildProjectClientCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API DebugProjectClientCommand : public ClientCommand
{
public:
    DebugProjectClientCommand(const std::string& projectFilePath_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string serverName;
};

class BUILD_API InstallProjectClientCommand : public ClientCommand
{
public:
    InstallProjectClientCommand(const std::string& projectFilePath_, const std::string& directory_, const std::string& serverName_);
    void Execute() override;
private:
    std::string projectFilePath;
    std::string directory;
    std::string serverName;
};

class BUILD_API ShowConfigurationClientCommand : public ClientCommand
{
public:
    ShowConfigurationClientCommand();
    void Execute() override;
};

BUILD_API std::unique_ptr<ClientCommand> ParseClientCommand(const std::string& command);

} } // namespace cmajor::build;

#endif // CMAJOR_BUILD_CLIENT_COMMAND_INCLUDED
