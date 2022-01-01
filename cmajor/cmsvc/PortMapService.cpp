// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/PortMapService.hpp>
#include <cmajor/cmsvc/Config.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmpm/PortMapMessage.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <thread>
#include <sstream>

#undef GetMessage

namespace cmajor { namespace service {

using namespace soulng::unicode;
using namespace soulng::util;

std::string GetMessage(sngxml::dom::Element* element)
{
    return ToUtf8(element->Name());
}

int GetPortMapServicePortNumberFromConfig()
{
    std::string portMapConfigFilePath;
    try
    {
        portMapConfigFilePath = PortMapConfigFilePath();
        std::unique_ptr<sngxml::dom::Document> portMapConfigDoc = sngxml::dom::ReadDocument(portMapConfigFilePath);
        std::unique_ptr<sngxml::xpath::XPathObject> cmpmResult = sngxml::xpath::Evaluate(U"/cmpm", portMapConfigDoc.get());
        if (cmpmResult)
        {
            if (cmpmResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(cmpmResult.get());
                if (nodeSet->Length() == 1)
                {
                    sngxml::dom::Node* node = (*nodeSet)[0];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* cmpmElement = static_cast<sngxml::dom::Element*>(node);
                        std::u32string portMapServicePortAttribute = cmpmElement->GetAttribute(U"portMapServicePort");
                        if (!portMapServicePortAttribute.empty())
                        {
                            return boost::lexical_cast<int>(ToUtf8(portMapServicePortAttribute));
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: could not get port map service port number from '" + portMapConfigFilePath + "': " + std::string(ex.what()));
    }
    return -1;
}

bool StartPortMapServer()
{
    try
    {
        Process* portmapServerProcess = new Process("cmpms", Process::Redirections::processStdIn | Process::Redirections::processStdOut | Process::Redirections::processStdErr);
        std::string portmapServerStatus = Trim(portmapServerProcess->ReadLine(Process::StdHandle::stdOut));
        if (portmapServerStatus == "port-map-server-ready")
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            PutOutputServiceMessage(portmapServerStatus);
            return true;
        }
        else
        {
            std::string errorMessage = Trim(portmapServerProcess->ReadLine(Process::StdHandle::stdOut));
            PutOutputServiceMessage("error: port map server status is: " + portmapServerStatus + ": " + errorMessage);
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: could not start port map server: " + std::string(ex.what()));
    }
    return false;
}

bool StopPortMapServer()
{
    try
    {
        int portMapServerPort = GetPortMapServicePortNumberFromConfig();
        if (portMapServerPort == -1)
        {
            throw std::runtime_error("could not get port map server port number from configuration file '" + PortMapConfigFilePath() + "'");
        }
        TcpSocket portMapServerConnection("localhost", std::to_string(portMapServerPort));
        PutOutputServiceMessage("connection to port map server port " + std::to_string(portMapServerPort) + " established");
        PutOutputServiceMessage("sending stop request...");
        StopPortMapServerRequest request;
        std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("stopPortMapServerRequest");
        sngxml::dom::Document requestDoc;
        requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
        std::stringstream stream;
        CodeFormatter formatter(stream);
        requestDoc.Write(formatter);
        std::string requestStr = stream.str();
        Write(portMapServerConnection, requestStr);
        std::string replyStr = ReadStr(portMapServerConnection);
        std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket");
        std::string message = GetMessage(replyDoc->DocumentElement());
        if (message == "stopPortMapServerReply")
        {
            PutOutputServiceMessage("stop reply received");
            return true;
        }
        else
        {
            throw std::runtime_error("'stopPortMapServerReply' expected, message=" + message);
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: could not stop port map server: " + std::string(ex.what()));
    }
    return false;
}

int GetPortMapServicePortNumber(bool& portMapServerStarted)
{
    portMapServerStarted = false;
    int portmapServicePortNumber = GetPortMapServicePortNumberFromConfig();
    if (portmapServicePortNumber == -1)
    {
        portMapServerStarted = StartPortMapServer();
        portmapServicePortNumber = GetPortMapServicePortNumberFromConfig();
    }
    return portmapServicePortNumber;
}

int GetFreePortNumber(const std::string& processName)
{
    try
    {
        bool portMapServerStarted = false;
        int portmapServicePortNumber = GetPortMapServicePortNumber(portMapServerStarted);
        if (portmapServicePortNumber != -1)
        {
            for (int connectionAttempt = 1; connectionAttempt <= 2; ++connectionAttempt)
            {
                try
                {
                    int pid = GetPid();
                    TcpSocket portMapServerConnection("localhost", std::to_string(portmapServicePortNumber));
                    GetFreePortNumberRequest request;
                    request.processName = processName;
                    request.pid = pid;
                    std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("getFreePortNumberRequest");
                    sngxml::dom::Document requestDoc;
                    requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
                    std::stringstream stream;
                    CodeFormatter formatter(stream);
                    requestDoc.Write(formatter);
                    std::string requestStr = stream.str();
                    Write(portMapServerConnection, requestStr);
                    std::string replyStr = ReadStr(portMapServerConnection);
                    std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket");
                    std::string message = GetMessage(replyDoc->DocumentElement());
                    if (message == "getFreePortNumberReply")
                    {
                        GetFreePortNumberReply reply;
                        reply.FromXml(replyDoc->DocumentElement());
                        int freePortNumber = reply.portNumber;
                        return freePortNumber;
                    }
                    else
                    {
                        throw std::runtime_error("'getFreePortNumberReply' expected, message=" + message);
                    }
                }
                catch (const std::exception& ex)
                {
                    if (connectionAttempt > 1)
                    {
                        throw ex;
                    }
                }
                if (connectionAttempt == 1 && !portMapServerStarted)
                {
                    portMapServerStarted = StartPortMapServer();
                }
                else
                {
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: could not get free port number from port map server: " + std::string(ex.what()));
    }
    return -1;
}

bool IsPortMapServerRunning()
{
    bool logError = true;
    try
    {
        int portMapServerPort = GetPortMapServicePortNumberFromConfig();
        if (portMapServerPort == -1)
        {
            throw std::runtime_error("could not get port map server port number from configuration file '" + PortMapConfigFilePath() + "'");
        }
        logError = false;
        TcpSocket connection("localhost", std::to_string(portMapServerPort));
        logError = true;
        HelloPmsRequest request;
        std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("helloPmsRequest");
        sngxml::dom::Document requestDoc;
        requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
        std::stringstream stream;
        CodeFormatter formatter(stream);
        requestDoc.Write(formatter);
        std::string requestStr = stream.str();
        Write(connection, requestStr);
        std::string replyStr = ReadStr(connection);
        if (replyStr.empty())
        {
            throw std::runtime_error("'helloPmsReply' expected, please stop cmpms process from the Task Manager and use 'Server | Start' command to run updated version");
        }
        std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket");
        std::string message = GetMessage(replyDoc->DocumentElement());
        if (message == "helloPmsReply")
        {
            HelloPmsReply reply;
            reply.FromXml(replyDoc->DocumentElement());
            PutOutputServiceMessage("port map server (cmpms) version " + reply.version + " running");
            return true;
        }
        else
        {
            throw std::runtime_error("'helloPmsReply' expected, message=" + message + ", please stop cmpms process from the Task Manager and use 'Server | Start' command to run updated version");
        }
    }
    catch (const std::exception& ex)
    {
        if (logError)
        {
            PutOutputServiceMessage("error: " + std::string(ex.what()));
        }
    }
    return false;
}


std::vector<PortLease> GetPortLeases()
{
    std::vector<PortLease> portLeases;
    try
    {
        int portMapServerPort = GetPortMapServicePortNumberFromConfig();
        if (portMapServerPort == -1)
        {
            throw std::runtime_error("could not get port map server port number from configuration file '" + PortMapConfigFilePath() + "'");
        }
        TcpSocket connection("localhost", std::to_string(portMapServerPort));
        ViewPortLeaseRequest request;
        std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("viewPortLeaseRequest");
        sngxml::dom::Document requestDoc;
        requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
        std::stringstream stream;
        CodeFormatter formatter(stream);
        requestDoc.Write(formatter);
        std::string requestStr = stream.str();
        Write(connection, requestStr);
        std::string replyStr = ReadStr(connection);
        std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket");
        std::string message = GetMessage(replyDoc->DocumentElement());
        if (message == "viewPortLeaseReply")
        {
            ViewPortLeaseReply reply;
            reply.FromXml(replyDoc->DocumentElement());
            return reply.portLeases;
        }
        else
        {
            throw std::runtime_error("'viewPortLeaseReply' expected, message=" + message);
        }
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("error: " + std::string(ex.what()));
    }
    return portLeases;
}

} } // namespace cmajor::service
