#include "BuildServerMessage.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

StopBuildRequest::StopBuildRequest()
{
}

StopBuildRequest::StopBuildRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> StopBuildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

StopBuildReply::StopBuildReply()
{
}

StopBuildReply::StopBuildReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> StopBuildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

KeepAliveBuildRequest::KeepAliveBuildRequest()
{
}

KeepAliveBuildRequest::KeepAliveBuildRequest(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveBuildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

KeepAliveBuildReply::KeepAliveBuildReply()
{
}

KeepAliveBuildReply::KeepAliveBuildReply(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveBuildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

BuildRequest::BuildRequest()
    : messageKind(), backend(), config(), optimizationLevel(), filePath(), defines(), numBuildThreads(), toolChain(), verbose(), quiet(), clean(), strictNothrow(), emitIR(), emitOptIR(), linkWithDebugRuntime(), linkUsingMsLink(), rebuild(), buildAllDependencies(), forceDebugInfo(), forceNoDebugInfo(), disableModuleCache(), singleThreadedCompile(), disableCodeGen(), justMyCode()
{
}

BuildRequest::BuildRequest(sngxml::dom::Element* element)
    : messageKind(), backend(), config(), optimizationLevel(), filePath(), defines(), numBuildThreads(), toolChain(), verbose(), quiet(), clean(), strictNothrow(), emitIR(), emitOptIR(), linkWithDebugRuntime(), linkUsingMsLink(), rebuild(), buildAllDependencies(), forceDebugInfo(), forceNoDebugInfo(), disableModuleCache(), singleThreadedCompile(), disableCodeGen(), justMyCode()
{
    sngxml::xmlser::FromXml(element, "messageKind", messageKind);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "optimizationLevel", optimizationLevel);
    sngxml::xmlser::FromXml(element, "filePath", filePath);
    sngxml::xmlser::FromXml(element, "defines", defines);
    sngxml::xmlser::FromXml(element, "numBuildThreads", numBuildThreads);
    sngxml::xmlser::FromXml(element, "toolChain", toolChain);
    sngxml::xmlser::FromXml(element, "verbose", verbose);
    sngxml::xmlser::FromXml(element, "quiet", quiet);
    sngxml::xmlser::FromXml(element, "clean", clean);
    sngxml::xmlser::FromXml(element, "strictNothrow", strictNothrow);
    sngxml::xmlser::FromXml(element, "emitIR", emitIR);
    sngxml::xmlser::FromXml(element, "emitOptIR", emitOptIR);
    sngxml::xmlser::FromXml(element, "linkWithDebugRuntime", linkWithDebugRuntime);
    sngxml::xmlser::FromXml(element, "linkUsingMsLink", linkUsingMsLink);
    sngxml::xmlser::FromXml(element, "rebuild", rebuild);
    sngxml::xmlser::FromXml(element, "buildAllDependencies", buildAllDependencies);
    sngxml::xmlser::FromXml(element, "forceDebugInfo", forceDebugInfo);
    sngxml::xmlser::FromXml(element, "forceNoDebugInfo", forceNoDebugInfo);
    sngxml::xmlser::FromXml(element, "disableModuleCache", disableModuleCache);
    sngxml::xmlser::FromXml(element, "singleThreadedCompile", singleThreadedCompile);
    sngxml::xmlser::FromXml(element, "disableCodeGen", disableCodeGen);
    sngxml::xmlser::FromXml(element, "justMyCode", justMyCode);
}

std::unique_ptr<sngxml::dom::Element> BuildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(messageKind, "messageKind").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(optimizationLevel, "optimizationLevel").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(filePath, "filePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(defines, "defines").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(numBuildThreads, "numBuildThreads").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(toolChain, "toolChain").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(verbose, "verbose").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(quiet, "quiet").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(clean, "clean").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(strictNothrow, "strictNothrow").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(emitIR, "emitIR").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(emitOptIR, "emitOptIR").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(linkWithDebugRuntime, "linkWithDebugRuntime").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(linkUsingMsLink, "linkUsingMsLink").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(rebuild, "rebuild").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(buildAllDependencies, "buildAllDependencies").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(forceDebugInfo, "forceDebugInfo").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(forceNoDebugInfo, "forceNoDebugInfo").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(disableModuleCache, "disableModuleCache").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(singleThreadedCompile, "singleThreadedCompile").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(disableCodeGen, "disableCodeGen").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(justMyCode, "justMyCode").release()));
    return element;
}

CompileError::CompileError()
    : message(), project(), file(), line(), scol(), ecol()
{
}

CompileError::CompileError(sngxml::dom::Element* element)
    : message(), project(), file(), line(), scol(), ecol()
{
    sngxml::xmlser::FromXml(element, "message", message);
    sngxml::xmlser::FromXml(element, "project", project);
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "scol", scol);
    sngxml::xmlser::FromXml(element, "ecol", ecol);
}

std::unique_ptr<sngxml::dom::Element> CompileError::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(message, "message").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(project, "project").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(scol, "scol").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ecol, "ecol").release()));
    return element;
}

BuildReply::BuildReply()
    : requestValid(), requestErrorMessage(), logException(), time(), success(), errors()
{
}

BuildReply::BuildReply(sngxml::dom::Element* element)
    : requestValid(), requestErrorMessage(), logException(), time(), success(), errors()
{
    sngxml::xmlser::FromXml(element, "requestValid", requestValid);
    sngxml::xmlser::FromXml(element, "requestErrorMessage", requestErrorMessage);
    sngxml::xmlser::FromXml(element, "logException", logException);
    sngxml::xmlser::FromXml(element, "time", time);
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "errors", errors);
}

std::unique_ptr<sngxml::dom::Element> BuildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestValid, "requestValid").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestErrorMessage, "requestErrorMessage").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(logException, "logException").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(time, "time").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(errors, "errors").release()));
    return element;
}

LogBuildMessageRequest::LogBuildMessageRequest()
    : message()
{
}

LogBuildMessageRequest::LogBuildMessageRequest(sngxml::dom::Element* element)
    : message()
{
    sngxml::xmlser::FromXml(element, "message", message);
}

std::unique_ptr<sngxml::dom::Element> LogBuildMessageRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(message, "message").release()));
    return element;
}

LogBuildMessageReply::LogBuildMessageReply()
    : ok()
{
}

LogBuildMessageReply::LogBuildMessageReply(sngxml::dom::Element* element)
    : ok()
{
    sngxml::xmlser::FromXml(element, "ok", ok);
}

std::unique_ptr<sngxml::dom::Element> LogBuildMessageReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    return element;
}

BuildProgressMessage::BuildProgressMessage()
{
}

BuildProgressMessage::BuildProgressMessage(sngxml::dom::Element* element)
{
}

std::unique_ptr<sngxml::dom::Element> BuildProgressMessage::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    return element;
}

GenericBuildErrorReply::GenericBuildErrorReply()
    : error()
{
}

GenericBuildErrorReply::GenericBuildErrorReply(sngxml::dom::Element* element)
    : error()
{
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> GenericBuildErrorReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

CacheModuleRequest::CacheModuleRequest()
    : projectName(), projectFilePath(), backend(), config()
{
}

CacheModuleRequest::CacheModuleRequest(sngxml::dom::Element* element)
    : projectName(), projectFilePath(), backend(), config()
{
    sngxml::xmlser::FromXml(element, "projectName", projectName);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
}

std::unique_ptr<sngxml::dom::Element> CacheModuleRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectName, "projectName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    return element;
}

CacheModuleReply::CacheModuleReply()
    : error()
{
}

CacheModuleReply::CacheModuleReply(sngxml::dom::Element* element)
    : error()
{
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> CacheModuleReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

DefinitionSourceLocation::DefinitionSourceLocation()
    : file(), line(), scol(), ecol()
{
}

DefinitionSourceLocation::DefinitionSourceLocation(sngxml::dom::Element* element)
    : file(), line(), scol(), ecol()
{
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "scol", scol);
    sngxml::xmlser::FromXml(element, "ecol", ecol);
}

std::unique_ptr<sngxml::dom::Element> DefinitionSourceLocation::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(scol, "scol").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ecol, "ecol").release()));
    return element;
}

GetDefinitionRequest::GetDefinitionRequest()
    : projectName(), projectFilePath(), backend(), config(), identifier(), identifierLocation()
{
}

GetDefinitionRequest::GetDefinitionRequest(sngxml::dom::Element* element)
    : projectName(), projectFilePath(), backend(), config(), identifier(), identifierLocation()
{
    sngxml::xmlser::FromXml(element, "projectName", projectName);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "identifier", identifier);
    sngxml::xmlser::FromXml(element, "identifierLocation", identifierLocation);
}

std::unique_ptr<sngxml::dom::Element> GetDefinitionRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectName, "projectName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(identifier, "identifier").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(identifierLocation, "identifierLocation").release()));
    return element;
}

GetDefinitionReply::GetDefinitionReply()
    : definitionLocation(), ok(), error()
{
}

GetDefinitionReply::GetDefinitionReply(sngxml::dom::Element* element)
    : definitionLocation(), ok(), error()
{
    sngxml::xmlser::FromXml(element, "definitionLocation", definitionLocation);
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::unique_ptr<sngxml::dom::Element> GetDefinitionReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(definitionLocation, "definitionLocation").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

