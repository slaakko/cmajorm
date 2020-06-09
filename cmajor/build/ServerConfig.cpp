// =================================
// Copyright (c) 2020 Seppo Laakko
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
    return "clang";
#endif
}

int GetDefaultLocalPort()
{
    return 52360;
}

ServerInfo::ServerInfo(const std::string& serverName_, int port_, const std::string& defaultToolChain_) : serverName(serverName_), port(port_), defaultToolChain(defaultToolChain_)
{
}

void ServerInfo::SetDefaultToolChain(const std::string& defaultToolChain_)
{
    defaultToolChain = defaultToolChain_;
}

void ServerInfo::Print(CodeFormatter& formatter)
{
    std::string toolChainStr;
    if (!defaultToolChain.empty())
    {
        toolChainStr = ", default tool chain '" + defaultToolChain + "'";
    }
    formatter.WriteLine("server '" + serverName + "', port " + std::to_string(port) + toolChainStr);
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
            portMap.clear();
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
                            std::string serverPort = ToUtf8(element->GetAttribute(U"port"));
                            int port = boost::lexical_cast<int>(serverPort);
                            std::string defaultToolChain = ToUtf8(element->GetAttribute(U"defaultToolChain"));
                            Add(serverName, port, defaultToolChain, true, false);
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        LogMessage(-1, "serverconfig: reading server configuration from '" + serverConfigFilePath + "' failed: " + ex.what());
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
        LogMessage(-1, "serverconfig: writing server configuration to '" + serverConfigFilePath + "' failed: " + ex.what());
    }
}

void ServerConfig::Add(const std::string& serverName, int port, const std::string& defaultToolChain, bool force, bool read)
{
    if (read)
    {
        Read();
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::string toolChainStr;
        if (!defaultToolChain.empty())
        {
            toolChainStr = ", default tool chain '" + defaultToolChain + "'";
        }
        LogMessage(-1, "serverconfig: server '" + serverName + "', port " + std::to_string(port) + toolChainStr);
    }
    ServerInfo* serverInfo = GetServerInfo(serverName, false, false);
    if (serverInfo != nullptr)
    {
        if (!force)
        {
            throw std::runtime_error("serverconfig: server '" + serverName + "' already exists, use --force to add anyway");
        }
        auto it = portMap.find(port);
        if (it != portMap.cend())
        {
            std::string prevServerName = it->second;
            if (prevServerName != serverName)
            {
                LogMessage(-1, "serverconfig: warning: port " + std::to_string(port) + " already used by server '" + prevServerName + "'");
            }
        }
        serverInfo->SetPort(port);
        serverInfo->SetDefaultToolChain(defaultToolChain);
        portMap[port] = serverName;
    }
    else
    {
        auto it = portMap.find(port);
        if (it != portMap.cend())
        {
            std::string prevServerName = it->second;
            if (prevServerName != serverName)
            {
                throw std::runtime_error("serverconfig: error: port " + std::to_string(port) + " already used by server '" + prevServerName + "'");
            }
        }
        std::unique_ptr<ServerInfo> newServerInfo(new ServerInfo(serverName, port, defaultToolChain));
        serverMap[serverName] = newServerInfo.get();
        serverInfos.push_back(std::move(newServerInfo));
        portMap[port] = serverName;
    }
    Write();
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
    ServerInfo* local = GetServerInfo("$local", false, false);
    if (!local)
    {
        Add("$local", GetDefaultLocalPort(), GetDefaultToolChainForCurrentPlatform(), true, false);
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
            throw std::runtime_error("serverconfig: error: server name '" + serverName + "' not found from configuration file '" + CmajorServerConfigFilePath() + "'");
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
