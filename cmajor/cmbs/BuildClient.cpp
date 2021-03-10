// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/BuildClient.hpp>
#include <cmajor/cmsvc/BuildServerMessage.hpp>
#ifdef TRACE
#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/Trace.hpp>
#endif
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <sstream>

namespace cmbs {

using namespace soulng::util;
using namespace soulng::unicode;
using namespace sngxml::dom;

std::string GetMessageKind(Element* element) 
{
#ifdef TRACE
    soulng::util::Tracer tracer(GetMessageKind_f);
#endif // TRACE
    return ToUtf8(element->Name());
}

std::unique_ptr<Document> ReadMessage(TcpSocket& socket)
{
#ifdef TRACE
    soulng::util::Tracer tracer(ReadMessage_f);
#endif // TRACE
    std::string message = ReadStr(socket);
    std::u32string content = ToUtf32(message);
    return ParseDocument(content, "socket");
}

std::string ElementToString(Element* element)
{
    Document doc;
    doc.AppendChild(std::unique_ptr<Node>(element));
    std::stringstream strStream;
    CodeFormatter formatter(strStream);
    doc.Write(formatter);
    return strStream.str();
}

void ProcessBuildReply(const BuildReply& buildReply)
{
#ifdef TRACE
    soulng::util::Tracer tracer(ProcessBuildReply_f);
#endif // TRACE
    if (!buildReply.requestValid)
    {
        std::cout << "buildReply.requestValid=false:\n" << buildReply.requestErrorMessage << std::endl;;
    }
    if (!buildReply.success)
    {
        std::cout << "buildReply.success=false:\n" << std::endl;
    }
    if (!buildReply.logException.empty())
    {
        std::cout << "buildReply.logException:\n" << buildReply.logException << std::endl;
    }
    if (buildReply.success)
    {
        std::cout << "buildReply.success=true" << std::endl;
    }
}

void RunRequest(const std::string& requestFilePath, int port)
{
#ifdef TRACE
    soulng::util::Tracer tracer(RunRequest_f);
#endif // TRACE
    std::string request = ReadFile(requestFilePath);
    TcpSocket socket("localhost", std::to_string(port));
    Write(socket, request);
    std::unique_ptr<Document> message = ReadMessage(socket);
    std::string messageKind = GetMessageKind(message->DocumentElement());
    while (messageKind == "logBuildMessageRequest" || messageKind == "buildProgressMessage")
    {
        if (messageKind == "logBuildMessageRequest")
        {
            LogBuildMessageRequest request(message->DocumentElement());
            std::cout << request.message << std::endl;
            LogBuildMessageReply replyMessage;
            replyMessage.ok = true;
            std::unique_ptr<Element> replyMessageValue(replyMessage.ToXml("logBuildMessageReply"));
            std::string reply = ElementToString(replyMessageValue.release());
            Write(socket, reply);
        }
        message = ReadMessage(socket);
        messageKind = GetMessageKind(message->DocumentElement());
    }
    if (messageKind == "buildReply")
    {
        BuildReply reply(message->DocumentElement());
        ProcessBuildReply(reply);
    }
    else if (messageKind == "genericBuildErrorReply")
    {
        GenericBuildErrorReply reply(message->DocumentElement());
        throw std::runtime_error("generic error received: " + reply.error);
    }
    else
    {
        throw std::runtime_error("unknown reply received: 'messageKind' field value is '" + messageKind + "'");
    }
}

} // namespace cmbs
