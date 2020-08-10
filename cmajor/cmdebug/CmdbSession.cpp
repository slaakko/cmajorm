// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/CmdbSession.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Sha1.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <thread>
#include <fstream>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <chrono>
#include <time.h>

namespace cmajor { namespace debug {

using namespace soulng::util;
using namespace soulng::unicode;

int port = 54322;

RemoveCmdbSessionFileGuard::RemoveCmdbSessionFileGuard(const std::string& cmdbSessionFilePath_) : cmdbSessionFilePath(cmdbSessionFilePath_)
{
}

RemoveCmdbSessionFileGuard::~RemoveCmdbSessionFileGuard()
{
    try
    {
        boost::filesystem::remove(cmdbSessionFilePath);
    }
    catch (...)
    {
    }
}

CmdbSessionClient::~CmdbSessionClient()
{
}

class CmdbSession
{
public:
    static void Init();
    static void Done();
    static CmdbSession& Instance() { return *instance; }
    void SetKeys(const std::string& skeyStr_, const std::string& rkeyStr_);
    void Start(CmdbSessionClient* client_);
    void Stop();
    void Run();
    void WaitForStarted();
    void Transact(TcpSocket& socket);
    void OpenSession(TcpSocket& socket, sngxml::dom::Element* request);
    void CloseSession();
    void GetInput(TcpSocket& socket);
    void PutOutput(TcpSocket& socket, sngxml::dom::Element* request);
private:
    static std::unique_ptr<CmdbSession> instance;
    std::string skeyStr;
    std::string rkeyStr;
    std::thread thread;
    TcpSocket listenSocket;
    std::mutex mtx;
    std::condition_variable started;
    std::atomic<bool> exiting;
    std::atomic<bool> sessionClosed;
    CmdbSessionClient* client;
    CmdbSession();
};

std::unique_ptr<CmdbSession> CmdbSession::instance;

void CmdbSession::Init()
{
    instance.reset(new CmdbSession());
}

void CmdbSession::Done()
{
    instance.reset();
}

CmdbSession::CmdbSession() : exiting(false), sessionClosed(false), client(nullptr)
{
}

void CmdbSession::SetKeys(const std::string& skeyStr_, const std::string& rkeyStr_)
{
    skeyStr = skeyStr_;
    rkeyStr = rkeyStr_;
}

void CmdbSession::Run()
{
    try
    {
        listenSocket.Bind(port);
        listenSocket.Listen(10);
        started.notify_one();
        while (!exiting)
        {
            TcpSocket socket = listenSocket.Accept();
            if (exiting) return;
            sessionClosed = false;
            while (!sessionClosed)
            {
                Transact(socket);
            }
        }
    }
    catch (const std::exception& ex)
    {
        if (!exiting)
        {
            std::cerr << "error from CMDB session run: " << ex.what() << std::endl;
        }
    }
}

void CmdbSession::WaitForStarted()
{
    std::unique_lock<std::mutex> lock(mtx);
    started.wait(lock);
}

void RunSession()
{
    CmdbSession::Instance().Run();
}

void CmdbSession::Start(CmdbSessionClient* client_)
{
    client = client_;
    thread = std::thread{ RunSession };
}

void CmdbSession::Stop()
{
    try
    {
        exiting = true;
        try
        {
            TcpSocket socket;
            socket.Connect("localhost", std::to_string(port));
        }
        catch (...)
        {

        }
        thread.join();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "error stopping CMDB session: " << ex.what() << std::endl;
    }
}

void CmdbSession::OpenSession(TcpSocket& socket, sngxml::dom::Element* request)
{
    std::u32string skeyValue = request->GetAttribute(U"key");
    if (!skeyValue.empty())
    {
        if (GetSha1MessageDigest(skeyStr) == ToUtf8(skeyValue))
        {
            sngxml::dom::Document openSessionResponse;
            sngxml::dom::Element* responseMessage = new sngxml::dom::Element(U"cmdbMessage");
            responseMessage->SetAttribute(U"kind", U"openSessionResponse");
            responseMessage->SetAttribute(U"key", ToUtf32(GetSha1MessageDigest(rkeyStr)));
            openSessionResponse.AppendChild(std::unique_ptr<sngxml::dom::Node>(responseMessage));
            sngxml::dom::SendDocument(socket, openSessionResponse);
            return;
        }
    }
    throw std::runtime_error("invalid session key");
}

void CmdbSession::CloseSession()
{
    sessionClosed = true;
}

void CmdbSession::Transact(TcpSocket& socket)
{
    std::unique_ptr<sngxml::dom::Document> request = sngxml::dom::ReceiveDocument(socket);
    if (request)
    {
        std::unique_ptr<sngxml::xpath::XPathObject> requestObject(sngxml::xpath::Evaluate(U"/cmdbMessage", request.get()));
        if (requestObject)
        {
            if (requestObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(requestObject.get());
                if (nodeSet->Length() == 1)
                {
                    sngxml::dom::Node* node = (*nodeSet)[0];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                        std::u32string kind = element->GetAttribute(U"kind");
                        if (kind == U"openSessionRequest")
                        {
                            OpenSession(socket, element);
                        }
                        else if (kind == U"closeSessionRequest")
                        {
                            CloseSession();
                        }
                        else if (kind == U"inputRequest")
                        {
                            GetInput(socket);
                        }
                        else if (kind == U"outputRequest")
                        {
                            PutOutput(socket, element);
                        }
                        else
                        {
                            throw std::runtime_error("invalid CMDB session request received: kind='" + ToUtf8(kind) + "'");
                        }
                    }
                }
            }
        }
    }
    else
    {
        CloseSession();
    }
}

void CmdbSession::GetInput(TcpSocket& socket)
{
    std::string bytes = client->GetTargetInputBytes();
    sngxml::dom::Document inputResponse;
    sngxml::dom::Element* inputResponseElement = new sngxml::dom::Element(U"cmdbMessage");
    inputResponseElement->SetAttribute(U"kind", U"inputResponse");
    inputResponseElement->SetAttribute(U"bytes", ToUtf32(bytes));
    inputResponse.AppendChild(std::unique_ptr<sngxml::dom::Node>(inputResponseElement));
    sngxml::dom::SendDocument(socket, inputResponse);
}

void CmdbSession::PutOutput(TcpSocket& socket, sngxml::dom::Element* request)
{
    std::u32string handleValue = request->GetAttribute(U"handle");
    if (!handleValue.empty())
    {
        std::string handleStr = ToUtf8(handleValue);
        int handle = boost::lexical_cast<int>(handleStr);
        if (handle == 1 || handle == 2)
        {
            std::u32string bytesValue = request->GetAttribute(U"bytes");
            if (!bytesValue.empty())
            {
                std::string output;
                std::string bytesStr = ToUtf8(bytesValue);
                int n = bytesStr.length() / 2;
                for (int i = 0; i < n; ++i)
                {
                    std::string hex;
                    hex.append(1, bytesStr[i * 2]);
                    hex.append(1, bytesStr[i * 2 + 1]);
                    uint8_t x = ParseHexByte(hex);
                    output.append(1, static_cast<char>(x));
                }
                client->WriteTargetOuput(handle, output);
                sngxml::dom::Document outputResponse;
                sngxml::dom::Element* outputResponseElement = new sngxml::dom::Element(U"cmdbMessage");
                outputResponseElement->SetAttribute(U"kind", U"outputResponse");
                outputResponse.AppendChild(std::unique_ptr<sngxml::dom::Node>(outputResponseElement));
                sngxml::dom::SendDocument(socket, outputResponse);
            }
        }
    }
}

void SetCmdbSessionPort(int port_)
{
    port = port_;
}

void StartCmdbSession(const std::string& cmdbSessionFilePath, CodeFormatter& formatter, CmdbSessionClient* client, bool verbose)
{
    if (verbose)
    {
        formatter.WriteLine("starting CMDB session...");
    }
    std::ofstream cmdbSessionFile(cmdbSessionFilePath);
    CodeFormatter sessionFileFormatter(cmdbSessionFile);
    sessionFileFormatter.SetIndentSize(1);
    sngxml::dom::Document cmdbSessionDoc;
    sngxml::dom::Element* sessionElement = new sngxml::dom::Element(U"cmdbSession");
    sngxml::dom::Element* timestampElement = new sngxml::dom::Element(U"timestamp");
    time_t t;
    time(&t);
    timestampElement->SetAttribute(U"value", ToUtf32(std::to_string(t)));
    sessionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(timestampElement));
    sngxml::dom::Element* skeyElement = new sngxml::dom::Element(U"skey");
    boost::uuids::uuid skey = boost::uuids::random_generator()();
    std::string skeyStr = boost::uuids::to_string(skey);
    skeyElement->SetAttribute(U"value", ToUtf32(skeyStr));
    sessionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(skeyElement));
    sngxml::dom::Element* rkeyElement = new sngxml::dom::Element(U"rkey");
    boost::uuids::uuid rkey = boost::uuids::random_generator()();
    std::string rkeyStr = boost::uuids::to_string(rkey);
    rkeyElement->SetAttribute(U"value", ToUtf32(rkeyStr));
    sessionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rkeyElement));
    sngxml::dom::Element* portElement = new sngxml::dom::Element(U"port");
    portElement->SetAttribute(U"value", ToUtf32(std::to_string(port)));
    sessionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(portElement));
    cmdbSessionDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(sessionElement));
    cmdbSessionDoc.Write(sessionFileFormatter);
    CmdbSession::Instance().SetKeys(skeyStr, rkeyStr);
    CmdbSession::Instance().Start(client);
    CmdbSession::Instance().WaitForStarted();
}

void StopCmdbSession()
{
    CmdbSession::Instance().Stop();
}

void CmdbSessionInit()
{
    CmdbSession::Init();
}

void CmdbSessionDone()
{
    CmdbSession::Done();
}

} } // namespace cmajor::debug

