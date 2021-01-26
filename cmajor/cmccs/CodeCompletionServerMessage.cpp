#include "CodeCompletionServerMessage.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

CodeCompletionRequest::CodeCompletionRequest()
    : created(), received()
{
}

CodeCompletionRequest::CodeCompletionRequest(sngxml::dom::Element* element)
    : created(), received()
{
    sngxml::xmlser::FromXml(element, "created", created);
    sngxml::xmlser::FromXml(element, "received", received);
}

CodeCompletionRequest::~CodeCompletionRequest()
{
}

std::unique_ptr<sngxml::dom::Element> CodeCompletionRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(created, "created").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(received, "received").release()));
    return element;
}

CodeCompletionReply::CodeCompletionReply()
    : requestCreated(), requestReceived(), created()
{
}

CodeCompletionReply::CodeCompletionReply(sngxml::dom::Element* element)
    : requestCreated(), requestReceived(), created()
{
    sngxml::xmlser::FromXml(element, "requestCreated", requestCreated);
    sngxml::xmlser::FromXml(element, "requestReceived", requestReceived);
    sngxml::xmlser::FromXml(element, "created", created);
}

CodeCompletionReply::~CodeCompletionReply()
{
}

std::unique_ptr<sngxml::dom::Element> CodeCompletionReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestCreated, "requestCreated").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestReceived, "requestReceived").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(created, "created").release()));
    return element;
}

LoadEditModuleRequest::LoadEditModuleRequest()
    : CodeCompletionRequest()
{
}

LoadEditModuleRequest::LoadEditModuleRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
{
}

std::unique_ptr<sngxml::dom::Element> LoadEditModuleRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    return element;
}

LoadEditModuleReply::LoadEditModuleReply()
    : CodeCompletionReply()
{
}

LoadEditModuleReply::LoadEditModuleReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
{
}

std::unique_ptr<sngxml::dom::Element> LoadEditModuleReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    return element;
}

StopRequest::StopRequest()
    : CodeCompletionRequest()
{
}

StopRequest::StopRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
{
}

std::unique_ptr<sngxml::dom::Element> StopRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    return element;
}

StopReply::StopReply()
    : CodeCompletionReply()
{
}

StopReply::StopReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
{
}

std::unique_ptr<sngxml::dom::Element> StopReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    return element;
}

KeepAliveRequest::KeepAliveRequest()
{
}

KeepAliveRequest::KeepAliveRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

KeepAliveReply::KeepAliveReply()
{
}

KeepAliveReply::KeepAliveReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

