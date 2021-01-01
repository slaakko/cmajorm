// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/KeepAliveServer.hpp>
#include <cmajor/cmbs/BuildServerMessage.hpp>
#ifdef TRACE
#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/Trace.hpp>
#endif
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <memory>
#include <thread>
#include <chrono>

namespace cmbs {

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
    std::string GetMessage(JsonValue* messageValue);
    KeepAliveReply ProcessKeepAliveRequest(const KeepAliveRequest& request);
    std::condition_variable* exitVar;
    bool* exiting;
};

KeepAliveServer::KeepAliveServer() : exit(false), started(false), keepAliveServerPort(defaultKeepAliveServerPort), exitVar(nullptr), exiting(nullptr)
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_KeepAliveServer);
#endif // TRACE
}

std::string KeepAliveServer::GetMessage(JsonValue* messageValue)
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_GetMessage);
#endif // TRACE
    if (messageValue->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(messageValue);
        JsonValue* message = jsonObject->GetField(U"messageKind");
        if (message && message->Type() == JsonValueType::string)
        {
            JsonString* messageStr = static_cast<JsonString*>(message);
            return ToUtf8(messageStr->Value());
        }
    }
    return std::string();
}

KeepAliveReply KeepAliveServer::ProcessKeepAliveRequest(const KeepAliveRequest& request)
{
#ifdef TRACE
    soulng::util::Tracer tracer(KeepAliveServer_ProcessKeepAliveRequest);
#endif // TRACE
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    keepAliveReceivedTimePoint = now;
    KeepAliveReply reply;
    reply.messageKind = "keepAliveReply";
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
            JsonLexer lexer(ToUtf32(requestStr), "", 0);
            std::unique_ptr<JsonValue> requestValue = JsonParser::Parse(lexer);
            std::string message = GetMessage(requestValue.get());
            if (message == "keepAliveRequest")
            {
                KeepAliveRequest request(requestValue.get());
                KeepAliveReply reply = ProcessKeepAliveRequest(request);
                std::unique_ptr<JsonValue> replyValue = reply.ToJson();
                std::string replyStr = replyValue->ToString();
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
