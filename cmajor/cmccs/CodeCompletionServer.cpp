// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmccs/CodeCompletionServer.hpp>
#include <cmajor/cmccs/KeepAliveServer.hpp>
#include <cmajor/cmccs/CodeCompletionServerMessage.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <atomic>

namespace cmccs {

using namespace soulng::unicode;
using namespace soulng::util;

enum class CodeCompletionRequestKind
{
    loadEditModuleRequest, stopRequest
};

class CodeCompletionServer
{
public:
    static void Init();
    static void Done();
    static CodeCompletionServer& Instance() { return *instance; }
    void Start(int port_, const std::string& version_, std::condition_variable* exitVar_, bool* exiting_);
    void Stop();
    void Run();
    std::unique_ptr<CodeCompletionReply> HandleRequest(sngxml::dom::Element* requestElement, std::string& rootElementName);
    std::unique_ptr<CodeCompletionReply> HandleRequest(const LoadEditModuleRequest& request, std::string& rootElementName);
    std::unique_ptr<CodeCompletionReply> HandleRequest(const StopRequest& request, std::string& rootElementName);
private:
    static std::unique_ptr<CodeCompletionServer> instance;
    CodeCompletionServer();
    int port;
    std::string version;
    std::atomic_bool exit;
    std::condition_variable* exitVar;
    bool* exiting;
    bool running;
    bool stopRequestReceived;
    std::thread thread;
    TcpSocket listenSocket;
    TcpSocket socket;
    std::map<std::string, CodeCompletionRequestKind> requestKindMap;
};

std::unique_ptr<CodeCompletionServer> CodeCompletionServer::instance;

void CodeCompletionServer::Init()
{
    instance.reset(new CodeCompletionServer());
}

void CodeCompletionServer::Done()
{
    instance.reset();
}

CodeCompletionServer::CodeCompletionServer() : port(-1), version(), exit(false), exitVar(), exiting(), running(false), stopRequestReceived(false), listenSocket()
{
    requestKindMap["loadEditModuleRequest"] = CodeCompletionRequestKind::loadEditModuleRequest;
    requestKindMap["stopRequest"] = CodeCompletionRequestKind::stopRequest;
}

void RunServer(CodeCompletionServer* server)
{
    try
    {
        server->Run();
    }
    catch (const std::exception& ex)
    {
        LogFileWriter writer(LogFilePath());
        writer.WriteCurrentDateTime();
        writer << "cmccs code completion server got exception '" << ex.what() << "'" << std::endl;
    }
}

void CodeCompletionServer::Start(int port_, const std::string& version_, std::condition_variable* exitVar_, bool* exiting_)
{
    port = port_;
    version = version_;
    exitVar = exitVar_;
    exiting = exiting_;
    thread = std::thread{ RunServer, this };
    while (!running)
    {
        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    }
}

void CodeCompletionServer::Stop()
{
    exit = true;
    if (running)
    {
        if (!stopRequestReceived)
        {
            StopRequest request;
            std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("stopRequest");
            sngxml::dom::Document requestDoc;
            requestDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(requestElement.release()));
            std::stringstream s;
            CodeFormatter formatter(s);
            requestDoc.Write(formatter);
            TcpSocket socket("localhost", std::to_string(port));
            Write(socket, s.str());
        }
        else
        {
            TcpSocket socket("localhost", std::to_string(port));
        }
        thread.join();
    }
}

void CodeCompletionServer::Run()
{
    try
    {
        running = true;
        listenSocket.Bind(port);
        listenSocket.Listen(10);
        while (!exit)
        {
            std::cout << "code-completion-server-ready" << std::endl;
            socket = listenSocket.Accept();
            if (exit) return;
            stopRequestReceived = false;
            while (!stopRequestReceived)
            {
                std::string requestStr = ReadStr(socket);
                std::u32string content = ToUtf32(requestStr);
                std::unique_ptr<sngxml::dom::Document> requestDoc = sngxml::dom::ParseDocument(content, "socket");
                std::string rootElementName;
                std::unique_ptr<CodeCompletionReply> reply = HandleRequest(requestDoc->DocumentElement(), rootElementName);
                sngxml::dom::Document replyDoc;
                std::unique_ptr<sngxml::dom::Element> replyElement = reply->ToXml(rootElementName);
                replyDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(replyElement.release()));
                std::stringstream s;
                CodeFormatter formatter(s);
                formatter.SetIndentSize(1);
                replyDoc.Write(formatter);
                std::string replyStr = s.str();
                Write(socket, replyStr);
            }
        }
    }
    catch (const std::exception& ex)
    {
        LogFileWriter writer(LogFilePath());
        writer.WriteCurrentDateTime();
        writer << "cmccs code completion server got exception '" << ex.what() << "'" << std::endl;
    }
}

std::unique_ptr<CodeCompletionReply> CodeCompletionServer::HandleRequest(sngxml::dom::Element* requestElement, std::string& rootElementName)
{
    auto it = requestKindMap.find(ToUtf8(requestElement->Name()));
    if (it != requestKindMap.cend())
    {
        CodeCompletionRequestKind kind = it->second;
        switch (kind)
        {
            case CodeCompletionRequestKind::loadEditModuleRequest:
            {
                LoadEditModuleRequest request(requestElement);
                return HandleRequest(request, rootElementName);
            }
            case CodeCompletionRequestKind::stopRequest:
            {
                StopRequest request(requestElement);
                return HandleRequest(request, rootElementName);
            }
            default:
            {
                throw std::runtime_error("unknown request kind '" + ToUtf8(requestElement->Name()) + "' received");
            }
        }
    }
    else
    {
        throw std::runtime_error("unknown request kind '" + ToUtf8(requestElement->Name()) + "' received");
    }
}

std::unique_ptr<CodeCompletionReply> CodeCompletionServer::HandleRequest(const LoadEditModuleRequest& request, std::string& rootElementName)
{
    rootElementName = "loadEditModuleReply";
    std::unique_ptr<CodeCompletionReply> reply(new LoadEditModuleReply());
    reply->created = std::chrono::steady_clock::now();
    reply->requestCreated = request.created;
    reply->requestReceived = request.received;
    return reply;
}

std::unique_ptr<CodeCompletionReply> CodeCompletionServer::HandleRequest(const StopRequest& request, std::string& rootElementName)
{
    rootElementName = "stopReply";
    std::unique_ptr<CodeCompletionReply> reply(new StopReply());
    stopRequestReceived = true;
    reply->created = std::chrono::steady_clock::now();
    reply->requestCreated = request.created;
    reply->requestReceived = request.received;
    return reply;
}

void StartCodeCompletionServer(int port, const std::string& version, std::condition_variable* exitVar, bool* exiting)
{
    CodeCompletionServer::Init();
    CodeCompletionServer::Instance().Start(port, version, exitVar, exiting);
}

void StopCodeCompletionServer()
{
    CodeCompletionServer::Instance().Stop();
    CodeCompletionServer::Done();
}

} // namespace cmccs
