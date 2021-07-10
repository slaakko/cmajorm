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
    , projectFilePath(), backend(), config()
{
}

LoadEditModuleRequest::LoadEditModuleRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
    , projectFilePath(), backend(), config()
{
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
}

std::unique_ptr<sngxml::dom::Element> LoadEditModuleRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    return element;
}

LoadEditModuleReply::LoadEditModuleReply()
    : CodeCompletionReply()
    , key(), result(), error(), startLoading(), startParsing(), endParsing(), endLoading()
{
}

LoadEditModuleReply::LoadEditModuleReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
    , key(), result(), error(), startLoading(), startParsing(), endParsing(), endLoading()
{
    sngxml::xmlser::FromXml(element, "key", key);
    sngxml::xmlser::FromXml(element, "result", result);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "startLoading", startLoading);
    sngxml::xmlser::FromXml(element, "startParsing", startParsing);
    sngxml::xmlser::FromXml(element, "endParsing", endParsing);
    sngxml::xmlser::FromXml(element, "endLoading", endLoading);
}

std::unique_ptr<sngxml::dom::Element> LoadEditModuleReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(key, "key").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(result, "result").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startLoading, "startLoading").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startParsing, "startParsing").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endParsing, "endParsing").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endLoading, "endLoading").release()));
    return element;
}

ResetEditModuleCacheRequest::ResetEditModuleCacheRequest()
    : CodeCompletionRequest()
{
}

ResetEditModuleCacheRequest::ResetEditModuleCacheRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
{
}

std::unique_ptr<sngxml::dom::Element> ResetEditModuleCacheRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    return element;
}

ResetEditModuleCacheReply::ResetEditModuleCacheReply()
    : CodeCompletionReply()
    , result(), error()
{
}

ResetEditModuleCacheReply::ResetEditModuleCacheReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
    , result(), error()
{
    sngxml::xmlser::FromXml(element, "result", result);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> ResetEditModuleCacheReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(result, "result").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

ParseSourceRequest::ParseSourceRequest()
    : CodeCompletionRequest()
    , projectFilePath(), backend(), config(), sourceFilePath(), sourceCode()
{
}

ParseSourceRequest::ParseSourceRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
    , projectFilePath(), backend(), config(), sourceFilePath(), sourceCode()
{
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "sourceFilePath", sourceFilePath);
    sngxml::xmlser::FromXml(element, "sourceCode", sourceCode);
}

std::unique_ptr<sngxml::dom::Element> ParseSourceRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceFilePath, "sourceFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceCode, "sourceCode").release()));
    return element;
}

ParseSourceReply::ParseSourceReply()
    : CodeCompletionReply()
    , ok(), error(), errors(), synchronized(), cursorContainer(), ruleContext(), startParsing(), endParsing()
{
}

ParseSourceReply::ParseSourceReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
    , ok(), error(), errors(), synchronized(), cursorContainer(), ruleContext(), startParsing(), endParsing()
{
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "errors", errors);
    sngxml::xmlser::FromXml(element, "synchronized", synchronized);
    sngxml::xmlser::FromXml(element, "cursorContainer", cursorContainer);
    sngxml::xmlser::FromXml(element, "ruleContext", ruleContext);
    sngxml::xmlser::FromXml(element, "startParsing", startParsing);
    sngxml::xmlser::FromXml(element, "endParsing", endParsing);
}

std::unique_ptr<sngxml::dom::Element> ParseSourceReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(errors, "errors").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(synchronized, "synchronized").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(cursorContainer, "cursorContainer").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ruleContext, "ruleContext").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startParsing, "startParsing").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endParsing, "endParsing").release()));
    return element;
}

GetCCListRequest::GetCCListRequest()
    : CodeCompletionRequest()
    , projectFilePath(), backend(), config(), sourceFilePath(), ccText(), ruleContext(), cursorLine()
{
}

GetCCListRequest::GetCCListRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
    , projectFilePath(), backend(), config(), sourceFilePath(), ccText(), ruleContext(), cursorLine()
{
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "sourceFilePath", sourceFilePath);
    sngxml::xmlser::FromXml(element, "ccText", ccText);
    sngxml::xmlser::FromXml(element, "ruleContext", ruleContext);
    sngxml::xmlser::FromXml(element, "cursorLine", cursorLine);
}

std::unique_ptr<sngxml::dom::Element> GetCCListRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceFilePath, "sourceFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ccText, "ccText").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ruleContext, "ruleContext").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(cursorLine, "cursorLine").release()));
    return element;
}

GetCCListReply::GetCCListReply()
    : CodeCompletionReply()
    , ok(), error(), ccList(), startGetCCList(), endGetCCList()
{
}

GetCCListReply::GetCCListReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
    , ok(), error(), ccList(), startGetCCList(), endGetCCList()
{
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "ccList", ccList);
    sngxml::xmlser::FromXml(element, "startGetCCList", startGetCCList);
    sngxml::xmlser::FromXml(element, "endGetCCList", endGetCCList);
}

std::unique_ptr<sngxml::dom::Element> GetCCListReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ccList, "ccList").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startGetCCList, "startGetCCList").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endGetCCList, "endGetCCList").release()));
    return element;
}

GetParamHelpListRequest::GetParamHelpListRequest()
    : CodeCompletionRequest()
    , projectFilePath(), backend(), config(), sourceFilePath(), symbolIndex()
{
}

GetParamHelpListRequest::GetParamHelpListRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
    , projectFilePath(), backend(), config(), sourceFilePath(), symbolIndex()
{
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "sourceFilePath", sourceFilePath);
    sngxml::xmlser::FromXml(element, "symbolIndex", symbolIndex);
}

std::unique_ptr<sngxml::dom::Element> GetParamHelpListRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceFilePath, "sourceFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(symbolIndex, "symbolIndex").release()));
    return element;
}

GetParamHelpListReply::GetParamHelpListReply()
    : CodeCompletionReply()
    , ok(), error(), list(), startGetParamHelpList(), endGetParamHelpList()
{
}

GetParamHelpListReply::GetParamHelpListReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
    , ok(), error(), list(), startGetParamHelpList(), endGetParamHelpList()
{
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "list", list);
    sngxml::xmlser::FromXml(element, "startGetParamHelpList", startGetParamHelpList);
    sngxml::xmlser::FromXml(element, "endGetParamHelpList", endGetParamHelpList);
}

std::unique_ptr<sngxml::dom::Element> GetParamHelpListReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(list, "list").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startGetParamHelpList, "startGetParamHelpList").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endGetParamHelpList, "endGetParamHelpList").release()));
    return element;
}

StopCCRequest::StopCCRequest()
    : CodeCompletionRequest()
{
}

StopCCRequest::StopCCRequest(sngxml::dom::Element* element)
    : CodeCompletionRequest(sngxml::xmlser::GetXmlFieldElement("base", element))
{
}

std::unique_ptr<sngxml::dom::Element> StopCCRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionRequest::ToXml("base").release()));
    return element;
}

StopCCReply::StopCCReply()
    : CodeCompletionReply()
{
}

StopCCReply::StopCCReply(sngxml::dom::Element* element)
    : CodeCompletionReply(sngxml::xmlser::GetXmlFieldElement("base", element))
{
}

std::unique_ptr<sngxml::dom::Element> StopCCReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(CodeCompletionReply::ToXml("base").release()));
    return element;
}

KeepAliveCCRequest::KeepAliveCCRequest()
{
}

KeepAliveCCRequest::KeepAliveCCRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveCCRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

KeepAliveCCReply::KeepAliveCCReply()
{
}

KeepAliveCCReply::KeepAliveCCReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveCCReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

