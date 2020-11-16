// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Debug.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <thread>
#include <time.h>

namespace cmajor { namespace rt {

using namespace soulng::util;
using namespace soulng::unicode;

const std::int64_t sessionTimeoutSecs = 5 * 60;

class CmdbSessionServer
{
public:
    static void Init();
    static void Done();
    static CmdbSessionServer& Instance() { return *instance; }
    void Start(const std::string& skey_, const std::string& rkey_, int port_);
    void OpenSession(TcpSocket& socket);
    void CloseSession(TcpSocket& socket);
    bool IsSessionOpen() const { return sessionOpen; }
    void SendOutput(int fileHandle, const std::string& bytes);
    int64_t ReceiveInput(uint8_t* buffer, int64_t bufferSize);
private:
    static std::unique_ptr<CmdbSessionServer> instance;
    CmdbSessionServer();
    std::string skey;
    std::string rkey;
    int port;
    bool sessionOpen;
    std::string inputHexByteBuffer;
};

std::unique_ptr<CmdbSessionServer> CmdbSessionServer::instance;

CmdbSessionServer::CmdbSessionServer() : skey(), rkey(), port(), sessionOpen(false)
{
}

void CmdbSessionServer::Init()
{
    instance.reset(new CmdbSessionServer());
}

void CmdbSessionServer::Done()
{
    instance.reset();
}

void CmdbSessionServer::Start(const std::string& skey_, const std::string& rkey_, int port_)
{
    skey = skey_;
    rkey = rkey_;
    port = port_;
    sessionOpen = true;
}

void CmdbSessionServer::OpenSession(TcpSocket& socket)
{
    sngxml::dom::Document openSessionRequest;
    sngxml::dom::Element* cmdbOpenMessage = new sngxml::dom::Element(U"cmdbMessage");
    openSessionRequest.AppendChild(std::unique_ptr<sngxml::dom::Node>(cmdbOpenMessage));
    cmdbOpenMessage->SetAttribute(U"kind", U"openSessionRequest");
    cmdbOpenMessage->SetAttribute(U"key", ToUtf32(GetSha1MessageDigest(this->skey)));
    sngxml::dom::SendDocument(socket, openSessionRequest);
    bool validOpenResponseReceived = false;
    std::unique_ptr<sngxml::dom::Document> openSessionResponse = sngxml::dom::ReceiveDocument(socket);
    if (openSessionResponse)
    {
        std::unique_ptr<sngxml::xpath::XPathObject> responseObject(sngxml::xpath::Evaluate(U"/cmdbMessage", openSessionResponse.get()));
        if (responseObject)
        {
            if (responseObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(responseObject.get());
                if (nodeSet->Length() == 1)
                {
                    sngxml::dom::Node* node = (*nodeSet)[0];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                        std::u32string kind = element->GetAttribute(U"kind");
                        if (kind == U"openSessionResponse")
                        {
                            std::string key = ToUtf8(element->GetAttribute(U"key"));
                            if (key == GetSha1MessageDigest(this->rkey))
                            {
                                validOpenResponseReceived = true;
                            }
                            else
                            {
                                throw std::runtime_error("invalid CMDB session key");
                            }
                        }
                    }
                }
            }
        }
    }
    if (!validOpenResponseReceived)
    {
        throw std::runtime_error("no valid open session response received");
    }
}

void CmdbSessionServer::CloseSession(TcpSocket& socket)
{
    sngxml::dom::Document closeSessionRequest;
    sngxml::dom::Element* cmdbCloseMessage = new sngxml::dom::Element(U"cmdbMessage");
    closeSessionRequest.AppendChild(std::unique_ptr<sngxml::dom::Node>(cmdbCloseMessage));
    cmdbCloseMessage->SetAttribute(U"kind", U"closeSessionRequest");
    sngxml::dom::SendDocument(socket, closeSessionRequest);
}

void CmdbSessionServer::SendOutput(int fileHandle, const std::string& bytes)
{
    try
    {
        if (!sessionOpen)
        {
            throw std::runtime_error("no CMDB session open");
        }
        TcpSocket socket("localhost", std::to_string(port));
        OpenSession(socket);
        sngxml::dom::Document outputRequest;
        sngxml::dom::Element* cmdbOutputMessage = new sngxml::dom::Element(U"cmdbMessage");
        outputRequest.AppendChild(std::unique_ptr<sngxml::dom::Node>(cmdbOutputMessage));
        cmdbOutputMessage->SetAttribute(U"kind", U"outputRequest");
        cmdbOutputMessage->SetAttribute(U"handle", ToUtf32(std::to_string(fileHandle)));
        cmdbOutputMessage->SetAttribute(U"bytes", ToUtf32(bytes));
        sngxml::dom::SendDocument(socket, outputRequest);
        bool outputResponseReceived = false;
        std::unique_ptr<sngxml::dom::Document> outputResponse = sngxml::dom::ReceiveDocument(socket);
        if (outputResponse)
        {
            std::unique_ptr<sngxml::xpath::XPathObject> responseObject(sngxml::xpath::Evaluate(U"/cmdbMessage", outputResponse.get()));
            if (responseObject)
            {
                if (responseObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                {
                    sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(responseObject.get());
                    if (nodeSet->Length() == 1)
                    {
                        sngxml::dom::Node* node = (*nodeSet)[0];
                        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                        {
                            sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                            std::u32string kind = element->GetAttribute(U"kind");
                            if (kind == U"outputResponse")
                            {
                                outputResponseReceived = true;
                            }
                        }
                    }
                }
            }
            CloseSession(socket);
            socket.Shutdown(ShutdownMode::both);
            socket.Close();
        }
        if (!outputResponseReceived)
        {
            throw std::runtime_error("no valid output response received");
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "CMDB session output request failed: " << ex.what() << std::endl;
    }
}

int64_t CmdbSessionServer::ReceiveInput(uint8_t* buffer, int64_t bufferSize)
{
    try
    {
        if (inputHexByteBuffer.empty())
        {
            if (!sessionOpen)
            {
                throw std::runtime_error("no CMDB session open");
            }
            TcpSocket socket("localhost", std::to_string(port));
            OpenSession(socket);
            sngxml::dom::Document inputRequest;
            sngxml::dom::Element* cmdbInputMessage = new sngxml::dom::Element(U"cmdbMessage");
            inputRequest.AppendChild(std::unique_ptr<sngxml::dom::Node>(cmdbInputMessage));
            cmdbInputMessage->SetAttribute(U"kind", U"inputRequest");
            sngxml::dom::SendDocument(socket, inputRequest);
            bool inputResponseReceived = false;
            std::unique_ptr<sngxml::dom::Document> inputResponse = sngxml::dom::ReceiveDocument(socket);
            if (inputResponse)
            {
                std::unique_ptr<sngxml::xpath::XPathObject> responseObject(sngxml::xpath::Evaluate(U"/cmdbMessage", inputResponse.get()));
                if (responseObject)
                {
                    if (responseObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                    {
                        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(responseObject.get());
                        if (nodeSet->Length() == 1)
                        {
                            sngxml::dom::Node* node = (*nodeSet)[0];
                            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                            {
                                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                                std::u32string kind = element->GetAttribute(U"kind");
                                if (kind == U"inputResponse")
                                {
                                    std::u32string value = element->GetAttribute(U"bytes");
                                    inputHexByteBuffer = ToUtf8(value);
                                    inputResponseReceived = true;
                                }
                            }
                        }
                    }
                }
            }
            CloseSession(socket);
            socket.Shutdown(ShutdownMode::both);
            socket.Close();
            if (!inputResponseReceived)
            {
                throw std::runtime_error("no valid input response received");
            }
        }
        int64_t bytesReceived = 0;
        uint8_t* p = buffer;
        int64_t bytesToReceive = bufferSize;
        while (inputHexByteBuffer.size() >= 2 && bytesToReceive > 0)
        {
            std::string hex;
            hex.append(1, inputHexByteBuffer[0]);
            hex.append(1, inputHexByteBuffer[1]);
            inputHexByteBuffer.erase(inputHexByteBuffer.begin());
            inputHexByteBuffer.erase(inputHexByteBuffer.begin());
            uint8_t byte = ParseHexByte(hex);
            *p++ = byte;
            --bytesToReceive;
            ++bytesReceived;
        }
        return bytesReceived;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "CMDB session input request failed: " << ex.what() << std::endl;
    }
    return 0;
}

void StartCmdbSession()
{
    try
    {
        std::string cmdbSessionFilePath;
        std::string exePath = GetFullPath(GetPathToExecutable());
        if (EndsWith(exePath, ".exe"))
        {
            cmdbSessionFilePath = Path::ChangeExtension(exePath, ".cmdbs");
        }
        else
        {
            cmdbSessionFilePath = exePath + ".cmdbs";
        }
        if (FileExists(cmdbSessionFilePath))
        {
            std::unique_ptr<sngxml::dom::Document> sessionDoc = sngxml::dom::ReadDocument(cmdbSessionFilePath);
            std::unique_ptr<sngxml::xpath::XPathObject> timestampObject = sngxml::xpath::Evaluate(U"/cmdbSession/timestamp", sessionDoc.get());
            if (timestampObject)
            {
                if (timestampObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                {
                    sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(timestampObject.get());
                    if (nodeSet->Length() == 1)
                    {
                        sngxml::dom::Node* timestampNode = (*nodeSet)[0];
                        if (timestampNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                        {
                            sngxml::dom::Element* timestampElement = static_cast<sngxml::dom::Element*>(timestampNode);
                            std::string timestampStr = ToUtf8(timestampElement->GetAttribute(U"value"));
                            if (!timestampStr.empty())
                            {
                                time_t timestamp = boost::lexical_cast<time_t>(timestampStr);
                                time_t now;
                                time(&now);
                                if (now - timestamp >= 0 && now - timestamp < sessionTimeoutSecs)
                                {
                                    std::unique_ptr<sngxml::xpath::XPathObject> skeyObject = sngxml::xpath::Evaluate(U"/cmdbSession/skey", sessionDoc.get());
                                    if (skeyObject)
                                    {
                                        if (skeyObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                                        {
                                            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(skeyObject.get());
                                            if (nodeSet->Length() == 1)
                                            {
                                                sngxml::dom::Node* keyNode = (*nodeSet)[0];
                                                if (keyNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                                                {
                                                    sngxml::dom::Element* keyElement = static_cast<sngxml::dom::Element*>(keyNode);
                                                    std::string skeyStr = ToUtf8(keyElement->GetAttribute(U"value"));
                                                    if (!skeyStr.empty())
                                                    {
                                                        std::unique_ptr<sngxml::xpath::XPathObject> rkeyObject = sngxml::xpath::Evaluate(U"/cmdbSession/rkey", sessionDoc.get());
                                                        if (rkeyObject)
                                                        {
                                                            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(rkeyObject.get());
                                                            if (nodeSet->Length() == 1)
                                                            {
                                                                sngxml::dom::Node* keyNode = (*nodeSet)[0];
                                                                if (keyNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                                                                {
                                                                    sngxml::dom::Element* keyElement = static_cast<sngxml::dom::Element*>(keyNode);
                                                                    std::string rkeyStr = ToUtf8(keyElement->GetAttribute(U"value"));
                                                                    if (!rkeyStr.empty())
                                                                    {
                                                                        std::unique_ptr<sngxml::xpath::XPathObject> portObject = sngxml::xpath::Evaluate(U"/cmdbSession/port", sessionDoc.get());
                                                                        if (portObject)
                                                                        {
                                                                            if (portObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                                                                            {
                                                                                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(portObject.get());
                                                                                if (nodeSet->Length() == 1)
                                                                                {
                                                                                    sngxml::dom::Node* portNode = (*nodeSet)[0];
                                                                                    if (portNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                                                                                    {
                                                                                        sngxml::dom::Element* portElement = static_cast<sngxml::dom::Element*>(portNode);
                                                                                        std::string portStr = ToUtf8(portElement->GetAttribute(U"value"));
                                                                                        if (!portStr.empty())
                                                                                        {
                                                                                            int port = boost::lexical_cast<int>(portStr);
                                                                                            CmdbSessionServer::Instance().Start(skeyStr, rkeyStr, port);
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            throw std::runtime_error("port is empty");
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        throw std::runtime_error("key is empty");
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        throw std::runtime_error("key is empty");
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                throw std::runtime_error("timestamp is empty");
                            }
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "unable to start CMDB session: " << ex.what() << std::endl;
    }
}

bool IsCmdbSessionOpen()
{
    return CmdbSessionServer::Instance().IsSessionOpen();
}

void WriteBytesToCmdbSession(int fileHandle, const uint8_t* buffer, int64_t count)
{
    std::string output;
    for (int64_t i = 0; i < count; ++i)
    {
        output.append(ToHexString(buffer[i]));
    }
    CmdbSessionServer::Instance().SendOutput(fileHandle, output);
}

int64_t ReadBytesFromCmdbSession(uint8_t* buffer, int64_t bufferSize)
{
    return CmdbSessionServer::Instance().ReceiveInput(buffer, bufferSize);
}

void InitCmdbSession()
{
    CmdbSessionServer::Init();
}

void DoneCmdbSession()
{
    CmdbSessionServer::Done();
}

} } // namespace cmajor::rt
