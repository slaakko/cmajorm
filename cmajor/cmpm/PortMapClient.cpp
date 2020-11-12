// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpm/PortMapClient.hpp>
#include <cmajor/cmpm/PortMapServer.hpp>
#include <cmajor/cmpm/PortMapMessage.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace cmajor { namespace cmpm {

using namespace soulng::util;
using namespace soulng::unicode;



class PortMapClient
{
public:
    static void Init();
    static void Done();
    static PortMapClient& Instance() { return *instance; }
    void Start(int portMapServicePort, const std::vector<int>& portNumbers, const std::string& programName, int pid);
    void Stop();
    void Run();
private:
    static std::unique_ptr<PortMapClient> instance;
    PortMapClient();
    void ExtendPortLease();
    int portMapServicePort;
    std::vector<int> portNumbers;
    std::string programName;
    int pid;
    std::thread clientThread;
    std::condition_variable exitVar;
    bool exiting;
    std::mutex mtx;
    bool run;
};

std::unique_ptr<PortMapClient> PortMapClient::instance;

void PortMapClient::Init()
{
    instance.reset(new PortMapClient());
}

void PortMapClient::Done()
{
    instance.reset();
}

PortMapClient::PortMapClient() : portMapServicePort(-1), portNumbers(), programName(), pid(-1), exiting(false), run(false)
{
}

void PortMapClient::Run()
{
    run = true;
    while (!exiting)
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (exitVar.wait_for(lock, std::chrono::seconds{ leaseRenewalTimeSecs }, [this] { return exiting; }))
        {
            return;
        }
        ExtendPortLease();
    }
}

void PortMapClient::ExtendPortLease()
{
    try
    {
        TcpSocket socket("localhost", std::to_string(portMapServicePort));
        ExtendPortLeaseRequest request;
        request.message = "ExtendPortLeaseRequest";
        request.programName = programName;
        request.pid = std::to_string(pid);
        for (int port : portNumbers)
        {
            request.portNumbers.push_back(std::to_string(port));
        }
        std::unique_ptr<JsonValue> requestValue = request.ToJson();
        std::string requestStr = requestValue->ToString();
        Write(socket, requestStr);
        std::string replyStr = ReadStr(socket);
        JsonLexer lexer(ToUtf32(replyStr), "", 0);
        std::unique_ptr<JsonValue> replyValue = JsonParser::Parse(lexer);
        if (replyValue->Type() == JsonValueType::object)
        {
            JsonObject* replyObject = static_cast<JsonObject*>(replyValue.get());
            JsonValue* messageValue = replyObject->GetField(U"message");
            if (messageValue && messageValue->Type() == JsonValueType::string)
            {
                JsonString* messageStr = static_cast<JsonString*>(messageValue);
                if (messageStr->Value() == U"ExtendPortLeaseReply")
                {
                    ExtendPortLeaseReply reply(replyValue.get());
                }
            }
        }
    }
    catch (...)
    {
    }
}

void RunPortMapClient(PortMapClient* client)
{
    try
    {
        client->Run();
    }
    catch (...)
    {
    }
}

void PortMapClient::Start(int portMapServicePort_, const std::vector<int>& portNumbers_, const std::string& programName_, int pid_)
{
    portMapServicePort = portMapServicePort_;
    portNumbers = portNumbers_;
    programName = programName_;
    pid = pid_;
    clientThread = std::thread{ RunPortMapClient, this };
}

void PortMapClient::Stop()
{
    if (run)
    {
        exiting = true;
        exitVar.notify_one();
        clientThread.join();
    }
}

void InitPortMapClient()
{
    PortMapClient::Init();
}

void DonePortMapClient()
{
    PortMapClient::Done();
}

void StartPortMapClient(int portMapServicePort, const std::vector<int>& portNumbers, const std::string& programName, int pid)
{
    PortMapClient::Instance().Start(portMapServicePort, portNumbers, programName, pid);
}

void StopPortMapClient()
{
    PortMapClient::Instance().Stop();
}

} } // namespace cmajor::cmpm
