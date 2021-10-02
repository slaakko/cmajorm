// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpm/PortMapClient.hpp>
#include <cmajor/cmpm/PortMapServer.hpp>
#include <cmajor/cmpm/PortMapMessage.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <sstream>
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
    void Start(int portMapServicePort, const std::vector<int>& portNumbers, const std::string& processName, int pid);
    void Stop();
    void Run();
private:
    static std::unique_ptr<PortMapClient> instance;
    PortMapClient();
    void ExtendPortLease();
    int portMapServicePort;
    std::vector<int> portNumbers;
    std::string processName;
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

PortMapClient::PortMapClient() : portMapServicePort(-1), portNumbers(), processName(), pid(-1), exiting(false), run(false)
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
        request.processName = processName;
        request.pid = pid;
        for (int port : portNumbers)
        {
            request.portNumbers.push_back(port);
        }
        std::unique_ptr<sngxml::dom::Element> requestValue = request.ToXml("extendPortLeaseRequest");
        std::stringstream strStream; 
        CodeFormatter formatter(strStream);
        requestValue->Write(formatter);
        std::string requestStr = strStream.str();
        Write(socket, requestStr);
        std::string replyStr = ReadStr(socket);
        std::unique_ptr<sngxml::dom::Document> replyDoc = sngxml::dom::ParseDocument(ToUtf32(replyStr), "socket");
        sngxml::dom::Element* replyElement = replyDoc->DocumentElement();
        if (replyElement->Name() == U"extendPortLeaseReply")
        {
            ExtendPortLeaseReply reply;
            reply.FromXml(replyElement);
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

void PortMapClient::Start(int portMapServicePort_, const std::vector<int>& portNumbers_, const std::string& processName_, int pid_)
{
    portMapServicePort = portMapServicePort_;
    portNumbers = portNumbers_;
    processName = processName_;
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

void StartPortMapClient(int portMapServicePort, const std::vector<int>& portNumbers, const std::string& processName, int pid)
{
    PortMapClient::Instance().Start(portMapServicePort, portNumbers, processName, pid);
}

void StopPortMapClient()
{
    PortMapClient::Instance().Stop();
}

} } // namespace cmajor::cmpm
