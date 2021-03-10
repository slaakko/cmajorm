// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/KeepAliveServer.hpp>
#include <cmajor/cmsvc/BuildServerMessage.hpp>
#ifdef TRACE
#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/Trace.hpp>
#endif
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <memory>
#include <thread>
#include <chrono>

namespace cmbs {

using namespace sngxml::dom;
using namespace soulng::util;
using namespace soulng::unicode;

std::string CmajorRootDir()
{
#ifdef TRACE
    soulng::util::Tracer tracer(CmajorRootDir_f);
#endif // TRACE
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return std::string(e);
}

std::string CmajorLogDir()
{
#ifdef TRACE
    soulng::util::Tracer tracer(CmajorLogDir_f);
#endif // TRACE
    std::string logDir = Path::Combine(CmajorRootDir(), "log");
    boost::filesystem::create_directories(logDir);
    return logDir;
}

std::string LogFilePath()
{
#ifdef TRACE
    soulng::util::Tracer tracer(LogFilePath_f);
#endif // TRACE
    return Path::Combine(CmajorLogDir(), "cmbs.log");
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
    std::string GetMessage(Element* element);
    std::string ElementToString(Element* element);
    KeepAliveBuildReply ProcessKeepAliveRequest(const KeepAliveBuildRequest& request);
    std::condition_variable* exitVar;
    bool* exiting;
};

KeepAliveServer::KeepAliveServer() : exit(false), started(false), keepAliveServerPort(defaultKeepAliveServerPort), exitVar(nullptr), exiting(nullptr)
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_KeepAliveServer);
#endif // TRACE
}

std::string KeepAliveServer::GetMessage(Element* element)
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_GetMessage);
#endif // TRACE
    return ToUtf8(element->Name());
}

std::string KeepAliveServer::ElementToString(Element* element)
{
    Document doc;
    doc.AppendChild(std::unique_ptr<Node>(element));
    std::stringstream strStream;
    CodeFormatter formatter(strStream);
    doc.Write(formatter);
    return strStream.str();
}

KeepAliveBuildReply KeepAliveServer::ProcessKeepAliveRequest(const KeepAliveBuildRequest& request)
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_ProcessKeepAliveRequest);
#endif // TRACE
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    keepAliveReceivedTimePoint = now;
    KeepAliveBuildReply reply;
    return reply;
}

void KeepAliveServer::Run()
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_Run);
#endif // TRACE
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
            std::unique_ptr<Document> requestDoc = ParseDocument(ToUtf32(requestStr), "socket");
            std::string message = GetMessage(requestDoc->DocumentElement());
            if (message == "keepAliveBuildRequest")
            {
                KeepAliveBuildRequest request(requestDoc->DocumentElement());
                KeepAliveBuildReply reply = ProcessKeepAliveRequest(request);
                std::unique_ptr<Element> replyElement = reply.ToXml("keepAliveBuildReply");
                std::string replyStr = ElementToString(replyElement.release());
                Write(socket, replyStr);
            }
            else
            {
                throw std::runtime_error("unknown message kind '" + message + "' received");
            }
        }
        catch (const std::exception& ex)
        {
            LogFileWriter writer(LogFilePath());
            writer.WriteCurrentDateTime();
            writer << "cmbs keep alive server got exception '" << ex.what() << "'" << std::endl;
        }
    }
}

bool KeepAliveServer::Timeout() const
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_Timeout);
#endif // TRACE
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
#ifdef TRACE
    soulng::util::SetThreadId('K');
    soulng::util::Tracer tracer(RunServer_KeepAliveServer_f);
#endif // TRACE
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
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_Start);
#endif // TRACE
    keepAliveServerPort = keepAliveServerPort_;
    exitVar = exitVar_;
    exiting = exiting_;
    thread = std::thread{ RunServer, this };
}

void KeepAliveServer::Stop()
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_Stop);
#endif // TRACE
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
#ifdef TRACE
    soulng::util::Tracer tracer(StartKeepAliveServer_f);
#endif // TRACE
    KeepAliveServer::Init();
    KeepAliveServer::Instance().Start(keepAliveServerPort, exitVar, exiting);
    while (!KeepAliveServer::Instance().Started())
    {
        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    }
}

void StopKeepAliveServer()
{
#ifdef TRACE
    soulng::util::Tracer tracer(StopKeepAliveServer_f);
#endif // TRACE
    KeepAliveServer::Instance().Stop();
    KeepAliveServer::Done();
}

bool Timeout() 
{
#ifdef TRACE
    soulng::util::Tracer tracer(Timeout_f);
#endif // TRACE
    return KeepAliveServer::Instance().Timeout();
}

} // namespace cmbs
