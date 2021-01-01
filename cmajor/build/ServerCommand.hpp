// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_SERVER_COMMAND_INCLUDED
#define CMAJOR_BUILD_SERVER_COMMAND_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <memory>
#include <string>

namespace cmajor { namespace build {

class BUILD_API ServerCommand
{
public:
    ServerCommand();
    virtual ~ServerCommand();
    virtual void Execute() = 0;
};

class BUILD_API AddServerServerCommand : public ServerCommand
{
public:
    AddServerServerCommand(const std::string& serverName_, int port_);
    void Execute() override;
private:
    std::string serverName;
    int port;
};

class BUILD_API RemoveServerServerCommand : public ServerCommand
{
public:
    RemoveServerServerCommand(const std::string& serverName_);
    void Execute() override;
private:
    std::string serverName;
};

class BUILD_API RunServerServerCommand : public ServerCommand
{
public:
    RunServerServerCommand(const std::string& serverName_);
    void Execute() override;
private:
    std::string serverName;
};

class BUILD_API ShowConfigurationServerCommand : public ServerCommand
{
public:
    ShowConfigurationServerCommand();
    void Execute() override;
};

BUILD_API std::unique_ptr<ServerCommand> ParseServerCommand(const std::string& command);

} } // namespace cmajor::build;

#endif // CMAJOR_BUILD_SERVER_COMMAND_INCLUDED
