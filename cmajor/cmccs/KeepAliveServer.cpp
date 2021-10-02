// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmccs/KeepAliveServer.hpp>
#include <cmajor/cmmsg/CodeCompletionServerMessage.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <memory>
#include <thread>
#include <chrono>

namespace cmccs {

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

std::string CmajorLogDir()
{
    std::string logDir = Path::Combine(CmajorRootDir(), "log");
    boost::filesystem::create_directories(logDir);
    return logDir;
}

std::string LogFilePath()
{
    return Path::Combine(CmajorLogDir(), "cmccs.log");
}

class KeepAliveServer
{
public:
    static void Init();
    static void Done();
    static KeepAliveServer& Instance() { return *instance; }
    void Start(int keepAliveServerPort_, std::condition_variable* exitVar_, bool* exiting_);
    void Stop();
    void Run();
    bool Started() const { return started; }
    bool Timeout() const;
private:
    static std::unique_ptr<KeepAliveServer> instance;
    KeepAliveServer();
    TcpSocket listenSocket;
    int keepAliveServerPort;
    std::thread thread;
    bool started;
    bool exit;
    std::chrono::time_point<std::chrono::steady_clock> keepAliveReceivedTimePoint;
    std::condition_variable* exitVar;
    bool* exiting;
};

KeepAliveServer::KeepAliveServer() : exit(false), started(false), keepAliveServerPort(defaultKeepAliveServerPort), exitVar(nullptr), exiting(nullptr)
{
}

void KeepAliveServer::Run()
{
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    keepAliveReceivedTimePoint = now;
    listenSocket.Bind(keepAliveServerPort);
    listenSocket.Listen(10);
    started = true;
    while (!exit)
    {
        TcpSocket socket = listenSocket.Accept();
        if (exit) return;
        try
        {
            std::string requestStr = ReadStr(socket);
            std::unique_ptr<sngxml::dom::Document> requestDoc = sngxml::dom::ParseDocument(ToUtf32(requestStr), "socket");
            if (requestDoc->DocumentElement()->Name() != U"keepAliveCCRequest")
            {
                throw std::runtime_error("unknown request kind received");
            }
            KeepAliveCCRequest keepAliveRequest;
            keepAliveRequest.FromXml(requestDoc->DocumentElement());
            keepAliveReceivedTimePoint = std::chrono::steady_clock::now();
            KeepAliveCCReply keepAliveReply;
            sngxml::dom::Document replyDoc;
            std::unique_ptr<sngxml::dom::Element> replyElement = keepAliveReply.ToXml("keepAliveCCReply");
            replyDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(replyElement.release()));
            std::stringstream s;
            CodeFormatter formatter(s);
            formatter.SetIndentSize(1);
            replyDoc.Write(formatter);
            std::string replyStr = s.str();
            Write(socket, replyStr);
        }
        catch (const std::exception& ex)
        {
            LogFileWriter writer(LogFilePath());
            writer.WriteCurrentDateTime();
            writer << "cmccs keep alive server got exception '" << ex.what() << "'" << std::endl;
        }
    }
}

bool KeepAliveServer::Timeout() const
{
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - keepAliveReceivedTimePoint).count() > timeoutSecs)
    {
        if (exitVar && exiting)
        {
            *exiting = true;
            exitVar->notify_one();
        }
        return true;
    }
    else
    {
        return false;
    }
}

void RunServer(KeepAliveServer* server)
{
    try
    {
        server->Run();
    }
    catch (...)
    {
    }
}

void KeepAliveServer::Start(int keepAliveServerPort_, std::condition_variable* exitVar_, bool* exiting_)
{
    keepAliveServerPort = keepAliveServerPort_;
    exitVar = exitVar_;
    exiting = exiting_;
    thread = std::thread{ RunServer, this };
}

void KeepAliveServer::Stop()
{
    try
    {
        exit = true;
        TcpSocket socket("localhost", std::to_string(keepAliveServerPort));
        thread.join();
    }
    catch (...)
    {
    }
}

std::unique_ptr<KeepAliveServer> KeepAliveServer::instance;

void KeepAliveServer::Init()
{
    instance.reset(new KeepAliveServer());
}

void KeepAliveServer::Done()
{
    instance.reset();
}

void StartKeepAliveServer(int keepAliveServerPort, std::condition_variable* exitVar, bool* exiting)
{
    KeepAliveServer::Init();
    KeepAliveServer::Instance().Start(keepAliveServerPort, exitVar, exiting);
    while (!KeepAliveServer::Instance().Started())
    {
        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    }
}

void StopKeepAliveServer()
{
    KeepAliveServer::Instance().Stop();
    KeepAliveServer::Done();
}

bool Timeout()
{
    return KeepAliveServer::Instance().Timeout();
}

} // namespace cmccs
