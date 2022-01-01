// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/KillChannel.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <memory>
#include <sstream>
#include <thread>

namespace cmajor { namespace debug {

using namespace soulng::util;
using namespace soulng::unicode;

class KillChannel
{
public:
    static void Init();
    static void Done();
    static KillChannel& Instance() { return *instance; }
    void Start(int port_);
    void Run();
    void Stop();
    void KillGdb();
private:
    KillChannel();
    static std::unique_ptr<KillChannel> instance;
    std::thread thread;
    int port;
    TcpSocket listenSocket;
    bool exiting;
};

std::unique_ptr<KillChannel> KillChannel::instance;

void KillChannel::Init()
{
    instance.reset(new KillChannel());
}

void KillChannel::Done()
{
    instance.reset();
}

KillChannel::KillChannel() : exiting(false), port(0)
{
}

void RunChannel(KillChannel* channel)
{
    channel->Run();
}

void KillChannel::Start(int port_)
{
    port = port_;
    thread = std::thread(RunChannel, this);
}

void KillChannel::Run()
{
    try
    {
        while (!exiting)
        {
            listenSocket = TcpSocket();
            listenSocket.Bind(port);
            listenSocket.Listen(10);
            TcpSocket socket = listenSocket.Accept();
            if (exiting) return;
            std::string requestStr = ReadStr(socket);
            std::unique_ptr<sngxml::dom::Document> requestDoc = sngxml::dom::ParseDocument(ToUtf32(requestStr), "socket");
            if (requestDoc->DocumentElement()->Name() == U"killRequest")
            {
                KillRequest killRequest;
                killRequest.FromXml(requestDoc->DocumentElement());
                KillGdb();
                KillReply killReply;
                std::unique_ptr<sngxml::dom::Element> replyElement = killReply.ToXml("killReply");
                sngxml::dom::Document replyDoc;
                replyDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(replyElement.release()));
                std::stringstream strStream;
                CodeFormatter formatter(strStream);
                replyDoc.Write(formatter);
                std::string replyStr = strStream.str();
                Write(socket, replyStr);
            }
        }
    }
    catch (...)
    {
    }
}

void KillChannel::KillGdb()
{
    try
    {
        TerminateGDB();
    }
    catch (...)
    {
    }
}

void KillChannel::Stop()
{
    try
    {
        exiting = true;
        TcpSocket socket("localhost", std::to_string(port));
        thread.join();
    }
    catch (...)
    {
    }
}

void StartKillChannel(int port)
{
    KillChannel::Instance().Start(port);
}

void StopKillChannel()
{
    KillChannel::Instance().Stop();
}

void InitKillChannel()
{
    KillChannel::Init();
}

void DoneKillChannel()
{
    KillChannel::Done();
}

} } // namespace cmajor::debug
