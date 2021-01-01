// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/ServerConfig.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngcm/ast/Project.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace cmajor { namespace build {

using namespace sngcm::ast;
using namespace sngxml::dom;
using namespace sngxml::xpath;
using namespace soulng::unicode;
using namespace cmajor::symbols;

std::string GetDefaultToolChainForCurrentPlatform()
{
#ifdef _WIN32
    return "gcc";
#else
    return "gcc";
#endif
}

int GetDefaultLocalPort()
{
    return 52360;
}

ServerInfo::ServerInfo(const std::string& serverName_, const std::string& host_, int port_, const std::string& defaultToolChain_) :
    serverName(serverName_), host(host_), port(port_), defaultToolChain(defaultToolChain_)
{
}

void ServerInfo::SetDefaultToolChain(const std::string& defaultToolChain_)
{
    defaultToolChain = defaultToolChain_;
}

void ServerInfo::Print(CodeFormatter& formatter)
{
    std::string toolChainStr;
    std::string hostStr;
    if (!defaultToolChain.empty())
    {
        toolChainStr = ", default tool chain '" + defaultToolChain + "'";
    }
    if (!host.empty())
    {
        hostStr = ", host " + host;
    }
    formatter.WriteLine("server '" + serverName + "'" + hostStr + ", port " + std::to_string(port) + toolChainStr);
}

void ServerConfig::Init()
{
    instance.reset(new ServerConfig());
}

void ServerConfig::Done()
{
    instance.reset();
}

std::string CmajorConfigDir()
{
    return Path::Combine(CmajorRootDir(), "config");
}

std::string CmajorServerConfigFilePath()
{
    return Path::Combine(CmajorConfigDir(), "server-config.xml");
}

std::unique_ptr<ServerConfig> ServerConfig::instance;

void ServerConfig::Read()
{
    std::string serverConfigFilePath = CmajorServerConfigFilePath();
    try
    {
        if (boost::filesystem::exists(serverConfigFilePath))
        {
            serverInfos.clear();
            serverMap.clear();
            hostPortMap.clear();
            std::unique_ptr<sngxml::dom::Document> configDoc = ReadDocument(serverConfigFilePath);
            std::unique_ptr<sngxml::xpath::XPathObject> servers = Evaluate(U"/servers/server", configDoc.get());
            if (servers)
            {
                if (servers->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                {
                    sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(servers.get());
                    int n = nodeSet->Length();
                    for (int i = 0; i < n; ++i)
                    {
                        sngxml::dom::Node* node = (*nodeSet)[i];
                        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                        {
                            sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                            std::string serverName = ToUtf8(element->GetAttribute(U"name"));
                            std::string serverHost = ToUtf8(element->GetAttribute(U"host"));
                            std::string serverPort = ToUtf8(element->GetAttribute(U"port"));
                            int port = boost::lexical_cast<int>(serverPort);
                            std::string defaultToolChain = ToUtf8(element->GetAttribute(U"defaultToolChain"));
                            Add(serverName, serverHost, port, defaultToolChain, true, false, false);
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        LogMessage(-1, "server config: reading server configuration from '" + serverConfigFilePath + "' failed: " + ex.what());
    }
}

void ServerConfig::Write()
{
    std::string serverConfigFilePath = CmajorServerConfigFilePath();
    try
    {
        sngxml::dom::Document serverConfigDoc;
        sngxml::dom::Element* serversElement = new sngxml::dom::Element(U"servers");
        serverConfigDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(serversElement));
        for (const std::unique_ptr<ServerInfo>& serverInfo : serverInfos)
        {
            sngxml::dom::Element* serverElement = new sngxml::dom::Element(U"server");
            serverElement->SetAttribute(U"name", ToUtf32(serverInfo->ServerName()));
            if (!serverInfo->Host().empty())
            {
                serverElement->SetAttribute(U"host", ToUtf32(serverInfo->Host()));
            }
            serverElement->SetAttribute(U"port", ToUtf32(std::to_string(serverInfo->Port())));
            serverElement->SetAttribute(U"defaultToolChain", ToUtf32(serverInfo->DefaultToolChain()));
            serversElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(serverElement));
        }
        std::ofstream serverConfigFile(serverConfigFilePath);
        CodeFormatter formatter(serverConfigFile);
        serverConfigDoc.Write(formatter);
    }
    catch (const std::exception& ex)
    {
        LogMessage(-1, "server config: writing server configuration to '" + serverConfigFilePath + "' failed: " + ex.what());
    }
}

void ServerConfig::Add(const std::string& serverName, const std::string& hostName, int port, const std::string& defaultToolChain, bool force, bool read, bool write)
{
    if (read)
    {
        Read();
    }
    std::string host = hostName;
    if (host == "localhost" || host == "127.0.0.1")
    {
        host.clear();
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::string toolChainStr;
        if (!defaultToolChain.empty())
        {
            toolChainStr = ", default tool chain '" + defaultToolChain + "'";
        }
        std::string hostStr;
        if (!host.empty())
        {
            hostStr = ", host " + host;
        }
    }
    ServerInfo* serverInfo = GetServerInfo(serverName, false, false);
    if (serverInfo != nullptr)
    {
        if (!force)
        {
            throw std::runtime_error("server config: server '" + serverName + "' already exists, use --force to add anyway");
        }
        auto it = hostPortMap.find(std::make_pair(host, port));
        if (it != hostPortMap.cend())
        {
            std::string prevServerName = it->second;
            if (prevServerName != serverName)
            {
                std::string hostPortStr;
                if (!host.empty())
                {
                    hostPortStr = "host " + host + ", port " + std::to_string(port);
                }
                else
                {
                    hostPortStr = "port " + std::to_string(port);
                }
                LogMessage(-1, "server config: warning: " + hostPortStr + " already in use for server '" + prevServerName + "'");
            }
        }
        serverInfo->SetHost(host);
        serverInfo->SetPort(port);
        serverInfo->SetDefaultToolChain(defaultToolChain);
        hostPortMap[std::make_pair(host, port)] = serverName;
    }
    else
    {
        auto it = hostPortMap.find(std::make_pair(host, port));
        if (it != hostPortMap.cend())
        {
            std::string prevServerName = it->second;
            if (prevServerName != serverName)
            {
                std::string hostPortStr;
                if (!host.empty())
                {
                    hostPortStr.append("host ").append(host).append(", port ").append(std::to_string(port));
                }
                else
                {
                    hostPortStr.append("port ").append(std::to_string(port));
                }
                throw std::runtime_error("server config: error: " + hostPortStr + " already in use for server '" + prevServerName + "'");
            }
        }
        std::unique_ptr<ServerInfo> newServerInfo(new ServerInfo(serverName, host, port, defaultToolChain));
        serverMap[serverName] = newServerInfo.get();
        serverInfos.push_back(std::move(newServerInfo));
        hostPortMap[std::make_pair(host, port)] = serverName;
    }
    if (write)
    {
        Write();
    }
}

void ServerConfig::Remove(const std::string& serverName)
{
    bool found = false;
    int n = serverInfos.size();
    for (int i = 0; i < n; ++i)
    {
        if (serverInfos[i]->ServerName() == serverName)
        {
            serverInfos.erase(serverInfos.begin() + i);
            found = true;
            break;
        }
    }
    if (found)
    {
        Write();
    }
    else
    {
        throw std::runtime_error("server '" + serverName + "' not found");
    }
}

void ServerConfig::Show()
{
    Read();
    CodeFormatter formatter(std::cout);
    for (const auto& p : serverMap)
    {
        ServerInfo* serverInfo = p.second;
        serverInfo->Print(formatter);
    }
}

ServerConfig::ServerConfig()
{
    Read();
    ServerInfo* local = GetServerInfo("local", false, false);
    if (!local)
    {
        Add("local", std::string(), GetDefaultLocalPort(), GetDefaultToolChainForCurrentPlatform(), true, false, true);
    }
}

ServerInfo* ServerConfig::GetServerInfo(const std::string& serverName, bool failIfNotExist, bool read)
{
    if (read)
    {
        Read();
    }
    auto it = serverMap.find(serverName);
    if (it != serverMap.cend())
    {
        return it->second;
    }
    else
    {
        if (failIfNotExist)
        {
            throw std::runtime_error("server config: error: server name '" + serverName + "' not found from configuration file '" + CmajorServerConfigFilePath() + "'");
        }
        return nullptr;
    }
}

void ServerInit()
{
    ServerConfig::Init();
}

void ServerDone()
{
    ServerConfig::Done();
}

} } // namespace cmajor::build
