// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmccs/CodeCompletionServer.hpp>
#include <cmajor/cmccs/KeepAliveServer.hpp>
#include <cmajor/cmmsg/CodeCompletionServerMessage.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/binder/Access.hpp>
#include <cmajor/binder/TypeBinding.hpp>
#include <cmajor/symbols/EditModuleCache.hpp>
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
using namespace cmajor::symbols;

enum class CodeCompletionRequestKind
{
    loadEditModuleRequest, resetEditModuleCacheRequest, parseSourceRequest, getCCListRequest, stopRequest
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
    std::unique_ptr<CodeCompletionReply> HandleRequest(const ResetEditModuleCacheRequest& request, std::string& rootElementName);
    std::unique_ptr<CodeCompletionReply> HandleRequest(const ParseSourceRequest& request, std::string& rootElementName);
    std::unique_ptr<CodeCompletionReply> HandleRequest(const GetCCListRequest& request, std::string& rootElementName);
    std::unique_ptr<CodeCompletionReply> HandleRequest(const StopCCRequest& request, std::string& rootElementName);
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
    requestKindMap["resetEditModuleCacheRequest"] = CodeCompletionRequestKind::resetEditModuleCacheRequest;
    requestKindMap["parseSourceRequest"] = CodeCompletionRequestKind::parseSourceRequest;
    requestKindMap["getCCListRequest"] = CodeCompletionRequestKind::getCCListRequest;
    requestKindMap["stopCCRequest"] = CodeCompletionRequestKind::stopRequest;
    SetReadProjectFunction(cmajor::build::ReadProject);
    SetTypeBindingFunction(cmajor::binder::BindTypes);
    SetAccessCheckFunction(cmajor::binder::HasAccess);
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
            StopCCRequest request;
            std::unique_ptr<sngxml::dom::Element> requestElement = request.ToXml("stopCCRequest");
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
            case CodeCompletionRequestKind::resetEditModuleCacheRequest:
            {
                ResetEditModuleCacheRequest request(requestElement);
                return HandleRequest(request, rootElementName);
            }
            case CodeCompletionRequestKind::parseSourceRequest:
            {
                ParseSourceRequest request(requestElement);
                return HandleRequest(request, rootElementName);
            }
            case CodeCompletionRequestKind::getCCListRequest:
            {
                GetCCListRequest request(requestElement);
                return HandleRequest(request, rootElementName);
            }
            case CodeCompletionRequestKind::stopRequest:
            {
                StopCCRequest request(requestElement);
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
    std::unique_ptr<LoadEditModuleReply> reply(new LoadEditModuleReply());
    reply->created = std::chrono::steady_clock::now();
    reply->requestCreated = request.created;
    reply->requestReceived = request.received;
    try
    {
        LoadEditModuleResult result = EditModuleCache::Instance().LoadEditModule(request.projectFilePath, request.backend, request.config);
        reply->key = result.key;
        std::string synchronized;
        if (result.synchronized)
        {
            synchronized = ", synchronized";
        }
        switch (result.resultKind)
        {
            case LoadEditModuleResultKind::moduleUpToDate: reply->result = "up-to-date"; break;
            case LoadEditModuleResultKind::moduleLoaded: reply->result = "loaded, number of errors: " + std::to_string(result.numberOfErrors) + synchronized; break;
            case LoadEditModuleResultKind::moduleReloaded: reply->result = "reloaded, number of errors: " + std::to_string(result.numberOfErrors) + synchronized; break;
        }
        reply->startLoading = result.startLoading;
        reply->startParsing = result.startParsing;
        reply->endParsing = result.endParsing;
        reply->endLoading = result.endLoading;
    }
    catch (const std::exception& ex)
    {
        reply->result = "error";
        reply->error = ex.what();
    }
    return std::unique_ptr<CodeCompletionReply>(reply.release());
}

std::unique_ptr<CodeCompletionReply> CodeCompletionServer::HandleRequest(const ResetEditModuleCacheRequest& request, std::string& rootElementName)
{
    rootElementName = "resetEditModuleCacheReply";
    std::unique_ptr<ResetEditModuleCacheReply> reply(new ResetEditModuleCacheReply());
    reply->created = std::chrono::steady_clock::now();
    reply->requestCreated = request.created;
    reply->requestReceived = request.received;
    try
    {
        EditModuleCache::Init();
        reply->result = "ok";
    }
    catch (const std::exception& ex)
    {
        reply->result = "error";
        reply->error = ex.what();
    }
    return std::unique_ptr<CodeCompletionReply>(reply.release());
}

std::unique_ptr<CodeCompletionReply> CodeCompletionServer::HandleRequest(const ParseSourceRequest& request, std::string& rootElementName)
{
    rootElementName = "parseSourceReply";
    std::unique_ptr<ParseSourceReply> reply(new ParseSourceReply());
    reply->ok = true;
    reply->created = std::chrono::steady_clock::now();
    reply->requestCreated = request.created;
    reply->requestReceived = request.received;
    try
    {
        Module* module = EditModuleCache::Instance().GetEditModule(request.projectFilePath, request.backend, request.config);
        if (module)
        {
            ParseResult result = module->ParseSource(request.sourceFilePath, request.sourceCode);
            if (result.ok)
            {
                reply->startParsing = result.start;
                reply->endParsing = result.end;
                reply->errors = result.errors;
                reply->synchronized = result.synchronized;
                reply->cursorContainer = result.cursorContainer;
            }
            else
            {
                reply->ok = false;
                reply->error = result.error;
            }
        }
        else
        {
            throw std::runtime_error("edit module '" + MakeEditModuleKey(request.projectFilePath, request.backend, request.config) + " not found");
        }
    }
    catch (const std::exception& ex)
    {
        reply->ok = false;
        reply->error = ex.what();
    }
    return std::unique_ptr<CodeCompletionReply>(reply.release());
}

std::unique_ptr<CodeCompletionReply> CodeCompletionServer::HandleRequest(const GetCCListRequest& request, std::string& rootElementName)
{
    rootElementName = "getCCListReply";
    std::unique_ptr<GetCCListReply> reply(new GetCCListReply());
    reply->ok = true;
    reply->created = std::chrono::steady_clock::now();
    reply->requestCreated = request.created;
    reply->requestReceived = request.received;
    try
    {
        Module* module = EditModuleCache::Instance().GetEditModule(request.projectFilePath, request.backend, request.config);
        if (module)
        {
            reply->startGetCCList = std::chrono::steady_clock::now();
            reply->ccList = module->GetCCList(request.sourceFilePath, request.ccText);
            reply->endGetCCList = std::chrono::steady_clock::now();
        }
        else
        {
            throw std::runtime_error("edit module '" + MakeEditModuleKey(request.projectFilePath, request.backend, request.config) + " not found");
        }
    }
    catch (const std::exception& ex)
    {
        reply->ok = false;
        reply->error = ex.what();
    }
    return std::unique_ptr<CodeCompletionReply>(reply.release());
}

std::unique_ptr<CodeCompletionReply> CodeCompletionServer::HandleRequest(const StopCCRequest& request, std::string& rootElementName)
{
    rootElementName = "stopCCReply";
    std::unique_ptr<CodeCompletionReply> reply(new StopCCReply());
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
