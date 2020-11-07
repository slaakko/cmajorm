// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpm/PortMapServer.hpp>
#include <cmajor/cmpm/PortMapMessage.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Json.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <chrono>

namespace cmajor { namespace cmpm {

using namespace soulng::util;
using namespace soulng::unicode;

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return std::string(e);
}

std::string CmajorConfigDir()
{
    std::string configDir = Path::Combine(CmajorRootDir(), "config");
    boost::filesystem::create_directories(configDir);
    return configDir;
}

std::string ConfigFilePath()
{
    return Path::Combine(CmajorConfigDir(), "cmpm.config.xml");
}

struct PortEntry
{
    PortEntry() : portNumber(-1), programName(), pid(), startTime(), leaseTime()
    {
    }
    PortEntry(int portNumber_, const std::string& programName_, int pid_, const std::string& startTime_) : 
        portNumber(portNumber_), programName(programName_), pid(pid_), startTime(startTime_), leaseTime(std::chrono::steady_clock::now())
    {
    }
    int portNumber;
    std::string programName;
    int pid;
    std::string startTime;
    std::chrono::time_point<std::chrono::steady_clock> leaseTime;
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
    std::string GetMessage(JsonValue* messageValue);
    int GetNextPortNumber(const std::string& programName, int pid, const std::string& startTime);
    PortEntry* GetPortEntry(int portNumber) const;
    GetFreePortNumberReply ProcessGetFreePortNumberRequest(const GetFreePortNumberRequest& request);
    ExtendPortLeaseReply ProcessExtendPortLeaseRequest(const ExtendPortLeaseRequest& request);
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

PortMapServer::PortMapServer() : portMapServicePort(54321), startPortRange(54300), endPortRange(54319), nextPortNumber(-1), exiting(false)
{
    std::string configFilePath = ConfigFilePath();
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

std::string PortMapServer::GetMessage(JsonValue* messageValue)
{
    if (messageValue->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(messageValue);
        JsonValue* message = jsonObject->GetField(U"message");
        if (message && message->Type() == JsonValueType::string)
        {
            JsonString* messageStr = static_cast<JsonString*>(message);
            return ToUtf8(messageStr->Value());
        }
    }
    return std::string();
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

int PortMapServer::GetNextPortNumber(const std::string& programName, int pid, const std::string& startTime)
{
    if (nextPortNumber > endPortRange)
    {
        nextPortNumber = startPortRange;
    }
    PortEntry* entry = GetPortEntry(nextPortNumber);
    if (entry == nullptr)
    {
        int portNumber = nextPortNumber++;
        portEntryMap[portNumber] = PortEntry(portNumber, programName, pid, startTime);
        return portNumber;
    }
    else
    {
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();  
        if (std::chrono::duration_cast<std::chrono::seconds>(now - entry->leaseTime).count() > 120)
        {
            ++nextPortNumber;
            entry->programName = programName;
            entry->pid = pid;
            entry->startTime = startTime;
            entry->leaseTime = now;
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
                portEntryMap[portNumber] = PortEntry(portNumber, programName, pid, startTime);
                return portNumber;
            }
            else
            {
                if (std::chrono::duration_cast<std::chrono::seconds>(now - entry->leaseTime).count() > 120)
                {
                    ++nextPortNumber;
                    entry->programName = programName;
                    entry->pid = pid;
                    entry->startTime = startTime;
                    entry->leaseTime = now;
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
    int portNumber = GetNextPortNumber(request.programName, boost::lexical_cast<int>(request.pid), request.time);
    GetFreePortNumberReply reply;
    reply.message = "GetFreePortNumberReply";
    reply.portNumber = std::to_string(portNumber);
    return reply;
}

ExtendPortLeaseReply PortMapServer::ProcessExtendPortLeaseRequest(const ExtendPortLeaseRequest& request)
{
    for (const std::string& portNumberStr : request.portNumbers)
    {
        int port = boost::lexical_cast<int>(portNumberStr);
        PortEntry* entry = GetPortEntry(port);
        if (entry != nullptr)
        {
            std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
            entry->leaseTime = now;
            entry->programName = request.programName;
            entry->pid = boost::lexical_cast<int>(request.pid);
        }
    }
    ExtendPortLeaseReply reply;
    reply.message = "ExtendPortLeaseReply";
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
            JsonLexer lexer(ToUtf32(requestStr), "", 0);
            std::unique_ptr<JsonValue> requestValue = JsonParser::Parse(lexer);
            std::string message = GetMessage(requestValue.get());
            if (!message.empty())
            {
                if (message == "GetFreePortNumberRequest")
                {
                    GetFreePortNumberRequest request(requestValue.get());
                    GetFreePortNumberReply reply = ProcessGetFreePortNumberRequest(request);
                    std::unique_ptr<JsonValue> replyValue = reply.ToJson();
                    std::string replyStr = replyValue->ToString();
                    Write(socket, replyStr);
                }
                else if (message == "ExtendPortLeaseRequest")
                {
                    ExtendPortLeaseRequest request(requestValue.get());
                    ExtendPortLeaseReply reply = ProcessExtendPortLeaseRequest(request);
                    std::unique_ptr<JsonValue> replyValue = reply.ToJson();
                    std::string replyStr = replyValue->ToString();
                    Write(socket, replyStr);
                }
                else if (message == "StopPortNumberServerRequest")
                {
                    exiting = true;
                }
            }
            else
            {
                throw std::runtime_error("unknown message received: '" + message + "'");
            }
        }
        catch (const std::exception& ex)
        {
            // todo
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
