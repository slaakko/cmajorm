// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_SERVER_CONFIG_INCLUDED
#define CMAJOR_BUILD_SERVER_CONFIG_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace cmajor { namespace build {

using soulng::util::CodeFormatter;

BUILD_API std::string GetDefaultToolChainForCurrentPlatform();
BUILD_API int GetDefaultLocalPort();

class BUILD_API ServerInfo
{
public:
    ServerInfo(const std::string& serverName_, int port_, const std::string& defaultToolChain_);
    ServerInfo(const ServerInfo&) = delete;
    ServerInfo& operator=(const ServerInfo&) = delete;
    const std::string& ServerName() const { return serverName; }
    int Port() const { return port; }
    void SetPort(int port_) { port = port_; }
    const std::string& DefaultToolChain() const { return defaultToolChain; }
    void SetDefaultToolChain(const std::string& defaultToolChain_);
    void Print(CodeFormatter& formatter);
private:
    std::string serverName;
    int port;
    std::string defaultToolChain;
};

class BUILD_API ServerConfig
{
public:
    static void Init();
    static void Done();
    static ServerConfig& Instance() { return *instance; }
    void Read();
    void Write();
    ServerConfig(const ServerConfig&) = delete;
    ServerConfig& operator=(const ServerConfig&) = delete;
    void Add(const std::string& serverName, int port, const std::string& defaultToolChain, bool force, bool read);
    void Show();
    ServerInfo* GetServerInfo(const std::string& serverName, bool failIfNotExist, bool read);
private:
    ServerConfig();
    static std::unique_ptr<ServerConfig> instance;
    std::map<std::string, ServerInfo*> serverMap;
    std::vector<std::unique_ptr<ServerInfo>> serverInfos;
    std::map<int, std::string> portMap;
};

BUILD_API void ServerInit();
BUILD_API void ServerDone();

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_SERVER_CONFIG_INCLUDED
