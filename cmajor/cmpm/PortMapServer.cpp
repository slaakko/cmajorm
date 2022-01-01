// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpm/PortMapServer.hpp>
#include <cmajor/cmpm/PortMapMessage.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Time.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <chrono>

namespace cmajor { namespace cmpm {

const char* portMapServerVersion = "4.3.0";

using namespace soulng::util;
using namespace soulng::unicode;

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return GetFullPath(std::string(e));
}

std::string CmajorConfigDir()
{
    std::string configDir = GetFullPath(Path::Combine(CmajorRootDir(), "config"));
    boost::filesystem::create_directories(configDir);
    return configDir;
}

std::string PortMapConfigFilePath()
{
    return GetFullPath(Path::Combine(CmajorConfigDir(), "cmpm.config.xml"));
}

std::string CmajorLogDir()
{
    std::string logDir = Path::Combine(CmajorRootDir(), "log");
    boost::filesystem::create_directories(logDir);
    return logDir;
}

std::string PortMapLogFilePath()
{
    return GetFullPath(Path::Combine(CmajorLogDir(), "cmpms.log"));
}

struct PortEntry
{
    PortEntry() : portNumber(-1), processName(), pid(), leaseStartTime(), leaseRenewalTime(), leaseTimePoint()
    {
    }
    PortEntry(int portNumber_, const std::string& processName_, int pid_) :
        portNumber(portNumber_), processName(processName_), pid(pid_), leaseStartTime(soulng::util::GetCurrentDateTime()), leaseRenewalTime(), leaseTimePoint(std::chrono::steady_clock::now())
    {
    }
    int portNumber;
    std::string processName;
    int pid;
    datetime leaseStartTime;
    datetime leaseRenewalTime;
    std::chrono::time_point<std::chrono::steady_clock> leaseTimePoint;
};

class PortMapServer
{
public:
    static void Init();
    static void Done();
    static PortMapServer& Instance() { return *instance; }
    void Run();
private:
    static std::unique_ptr<PortMapServer> instance;
    PortMapServer();
    int portMapServicePort;
    int startPortRange;
    int endPortRange;
    int nextPortNumber;
    std::map<int, PortEntry> portEntryMap;
    TcpSocket listenSocket;
    bool exiting;
    std::string GetMessage(sngxml::dom::Element* element);
    int GetNextPortNumber(const std::string& processName, int pid);
    PortEntry* GetPortEntry(int portNumber) const;
    GetFreePortNumberReply ProcessGetFreePortNumberRequest(const GetFreePortNumberRequest& request);
    ExtendPortLeaseReply ProcessExtendPortLeaseRequest(const ExtendPortLeaseRequest& request);
    ViewPortLeaseReply ProcessViewPortLeaseRequest(const ViewPortLeaseRequest& request);
    StopPortMapServerReply ProcessStopPortMapServerRequest(const StopPortMapServerRequest& request);
};

std::unique_ptr<PortMapServer> PortMapServer::instance;

void PortMapServer::Init()
{
    instance.reset(new PortMapServer());
}

void PortMapServer::Done()
{
    instance.reset();
}

PortMapServer::PortMapServer() : portMapServicePort(54321), startPortRange(54300), endPortRange(54320), nextPortNumber(-1), exiting(false)
{
    std::string configFilePath = PortMapConfigFilePath();
    std::unique_ptr<sngxml::dom::Document> configDoc;
    if (boost::filesystem::exists(configFilePath))
    {
        configDoc = sngxml::dom::ReadDocument(configFilePath);
    }
    else
    {
        configDoc.reset(new sngxml::dom::Document());
        sngxml::dom::Element* rootElement = new sngxml::dom::Element(U"cmpm");
        configDoc->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootElement));
        rootElement->SetAttribute(U"portMapServicePort", ToUtf32(std::to_string(portMapServicePort)));
        rootElement->SetAttribute(U"startPortRange", ToUtf32(std::to_string(startPortRange)));
        rootElement->SetAttribute(U"endPortRange", ToUtf32(std::to_string(endPortRange)));
        std::ofstream configFile(configFilePath);
        CodeFormatter formatter(configFile);
        formatter.SetIndentSize(1);
        configDoc->Write(formatter);
    }
    std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(U"/cmpm", configDoc.get());
    if (result)
    {
        if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
            if (nodeSet->Length() == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string portMapServicePortAttribute = element->GetAttribute(U"portMapServicePort");
                    if (!portMapServicePortAttribute.empty())
                    {
                        portMapServicePort = boost::lexical_cast<int>(ToUtf8(portMapServicePortAttribute));
                    }
                    else
                    {
                        throw std::runtime_error("'portMapServicePort' attribute not found from 'cmpm' element of file '" + configFilePath + "'");
                    }
                    std::u32string startPortRangeAttribute = element->GetAttribute(U"startPortRange");
                    if (!startPortRangeAttribute.empty())
                    {
                        startPortRange  = boost::lexical_cast<int>(ToUtf8(startPortRangeAttribute));
                    }
                    else
                    {
                        throw std::runtime_error("'startPortRange' attribute not found from 'cmpm' element of file '" + configFilePath + "'");
                    }
                    std::u32string endPortRangeAttribute = element->GetAttribute(U"endPortRange");
                    if (!endPortRangeAttribute.empty())
                    {
                        endPortRange = boost::lexical_cast<int>(ToUtf8(endPortRangeAttribute));
                    }
                    else
                    {
                        throw std::runtime_error("'endPortRange' attribute not found from 'cmpm' element of file '" + configFilePath + "'");
                    }
                    nextPortNumber = startPortRange;
                }
                else
                {
                    throw std::runtime_error("single 'cmpm' element expected to be found from '" + configFilePath + "'");
                }
            }
            else
            {
                throw std::runtime_error("single 'cmpm' element expected to be found from '" + configFilePath + "'");
            }
        }
        else
        {
            throw std::runtime_error("'cmpm' element expected to be found from '" + configFilePath + "'");
        }
    }
    else
    {
        throw std::runtime_error("'cmpm' element not found from '" + configFilePath + "'");
    }
}

std::string PortMapServer::GetMessage(sngxml::dom::Element* element)
{
    return ToUtf8(element->Name());
}

PortEntry* PortMapServer::GetPortEntry(int portNumber) const
{
    auto it = portEntryMap.find(portNumber);
    if (it != portEntryMap.cend())
    {
        const PortEntry& portEntry = it->second;
        return const_cast<PortEntry*>(&portEntry);
    }
    return nullptr;
}

int PortMapServer::GetNextPortNumber(const std::string& processName, int pid)
{
    if (nextPortNumber > endPortRange)
    {
        nextPortNumber = startPortRange;
    }
    PortEntry* entry = GetPortEntry(nextPortNumber);
    if (entry == nullptr)
    {
        int portNumber = nextPortNumber++;
        portEntryMap[portNumber] = PortEntry(portNumber, processName, pid);
        return portNumber;
    }
    else
    {
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();  
        if (std::chrono::duration_cast<std::chrono::seconds>(now - entry->leaseTimePoint).count() > leaseExpirationTimeSecs)
        {
            ++nextPortNumber;
            entry->processName = processName;
            entry->pid = pid;
            entry->leaseStartTime = GetCurrentDateTime();
            entry->leaseRenewalTime = datetime();
            entry->leaseTimePoint = now;
            return entry->portNumber;
        }
        int start = nextPortNumber++;
        while (nextPortNumber != start)
        {
            if (nextPortNumber > endPortRange)
            {
                nextPortNumber = startPortRange;
            }
            entry = GetPortEntry(nextPortNumber);
            if (entry == nullptr)
            {
                int portNumber = nextPortNumber++;
                portEntryMap[portNumber] = PortEntry(portNumber, processName, pid);
                return portNumber;
            }
            else
            {
                if (std::chrono::duration_cast<std::chrono::seconds>(now - entry->leaseTimePoint).count() > leaseExpirationTimeSecs)
                {
                    ++nextPortNumber;
                    entry->processName = processName;
                    entry->pid = pid;
                    entry->leaseStartTime = GetCurrentDateTime();
                    entry->leaseRenewalTime = datetime();
                    entry->leaseTimePoint = now;
                    return entry->portNumber;
                }
            }
            ++nextPortNumber;
        }
        return -1;
    }
}

GetFreePortNumberReply PortMapServer::ProcessGetFreePortNumberRequest(const GetFreePortNumberRequest& request)
{
    int portNumber = GetNextPortNumber(request.processName, request.pid);
    GetFreePortNumberReply reply;
    reply.portNumber = portNumber;
    return reply;
}

ExtendPortLeaseReply PortMapServer::ProcessExtendPortLeaseRequest(const ExtendPortLeaseRequest& request)
{
    for (int port : request.portNumbers)
    {
        PortEntry* entry = GetPortEntry(port);
        if (entry != nullptr)
        {
            std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
            entry->leaseRenewalTime = GetCurrentDateTime();
            entry->leaseTimePoint = now;
            entry->processName = request.processName;
            entry->pid = request.pid;
        }
    }
    ExtendPortLeaseReply reply;
    return reply;
}

ViewPortLeaseReply PortMapServer::ProcessViewPortLeaseRequest(const ViewPortLeaseRequest& request)
{
    ViewPortLeaseReply reply;
    for (const std::pair<int, PortEntry>& p : portEntryMap)
    {
        const PortEntry& portEntry = p.second;
        PortLease portLease;
        portLease.portNumber = portEntry.portNumber;
        portLease.processName = portEntry.processName;
        portLease.pid = portEntry.pid;
        portLease.leaseStartTime = portEntry.leaseStartTime;
        portLease.leaseRenewalTime = portEntry.leaseRenewalTime;
        std::string leaseStateStr = "valid";
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - portEntry.leaseTimePoint).count() > leaseExpirationTimeSecs)
        {
            leaseStateStr = "expired";
        }
        portLease.leaseState = leaseStateStr;
        reply.portLeases.push_back(portLease);
    }
    return reply;
}

StopPortMapServerReply PortMapServer::ProcessStopPortMapServerRequest(const StopPortMapServerRequest& request)
{
    StopPortMapServerReply reply;
    exiting = true;
    return reply;
}

void PortMapServer::Run()
{
    listenSocket.Bind(portMapServicePort);
    listenSocket.Listen(10);
    std::cout << "port-map-server-ready" << std::endl;
    while (!exiting)
    {
        TcpSocket socket = listenSocket.Accept();
        if (exiting) return;
        try
        {
            std::string requestStr = ReadStr(socket);
            std::unique_ptr<sngxml::dom::Document> requestDoc = sngxml::dom::ParseDocument(ToUtf32(requestStr), "socket");
            std::string message = GetMessage(requestDoc->DocumentElement());
            if (!message.empty())
            {
                if (message == "helloPmsRequest")
                {
                    HelloPmsRequest request;
                    request.FromXml(requestDoc->DocumentElement());
                    HelloPmsReply reply;
                    reply.version = portMapServerVersion;
                    std::unique_ptr<sngxml::dom::Element> replyValue = reply.ToXml("helloPmsReply");
                    std::stringstream strStream;
                    CodeFormatter formatter(strStream);
                    replyValue->Write(formatter);
                    std::string replyStr = strStream.str();
                    Write(socket, replyStr);
                }
                else if (message == "getFreePortNumberRequest")
                {
                    GetFreePortNumberRequest request;
                    request.FromXml(requestDoc->DocumentElement());
                    GetFreePortNumberReply reply = ProcessGetFreePortNumberRequest(request);
                    std::unique_ptr<sngxml::dom::Element> replyValue = reply.ToXml("getFreePortNumberReply");
                    std::stringstream strStream;
                    CodeFormatter formatter(strStream);
                    replyValue->Write(formatter);
                    std::string replyStr = strStream.str();
                    Write(socket, replyStr);
                }
                else if (message == "extendPortLeaseRequest")
                {
                    ExtendPortLeaseRequest request;
                    request.FromXml(requestDoc->DocumentElement());
                    ExtendPortLeaseReply reply = ProcessExtendPortLeaseRequest(request);
                    std::unique_ptr<sngxml::dom::Element> replyValue = reply.ToXml("extendPortLeaseReply");
                    std::stringstream strStream;
                    CodeFormatter formatter(strStream);
                    replyValue->Write(formatter);
                    std::string replyStr = strStream.str();
                    Write(socket, replyStr);
                }
                else if (message == "stopPortMapServerRequest")
                {
                    StopPortMapServerRequest request;
                    request.FromXml(requestDoc->DocumentElement());
                    StopPortMapServerReply reply = ProcessStopPortMapServerRequest(request);
                    std::unique_ptr<sngxml::dom::Element> replyValue = reply.ToXml("stopPortMapServerReply");
                    std::stringstream strStream;
                    CodeFormatter formatter(strStream);
                    replyValue->Write(formatter);
                    std::string replyStr = strStream.str();
                    Write(socket, replyStr);
                }
                else if (message == "viewPortLeaseRequest")
                {
                    ViewPortLeaseRequest request;
                    request.FromXml(requestDoc->DocumentElement());
                    ViewPortLeaseReply reply = ProcessViewPortLeaseRequest(request);
                    std::unique_ptr<sngxml::dom::Element> replyValue = reply.ToXml("viewPortLeaseReply");
                    std::stringstream strStream;
                    CodeFormatter formatter(strStream);
                    replyValue->Write(formatter);
                    std::string replyStr = strStream.str();
                    Write(socket, replyStr);
                }
            }
            else
            {
                throw std::runtime_error("unknown message received: '" + message + "'");
            }
        }
        catch (const std::exception& ex)
        {
            LogFileWriter writer(PortMapLogFilePath());
            writer.WriteCurrentDateTime();
            writer << "cmpms got exception '" << ex.what() << "'" << std::endl;
        }
    }
}

void InitPortMapServer()
{
    PortMapServer::Init();
}

void DonePortMapServer()
{
    PortMapServer::Done();
}

void RunPortMapServer()
{
    PortMapServer::Instance().Run();
}

} } // namespace cmajor::cmpm
