#include "BuildServerMessage.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int StopBuildRequest::classId = -1;

StopBuildRequest::StopBuildRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr), isOwned(false)
{
}

void* StopBuildRequest::Create()
{
    return new StopBuildRequest();
}

void StopBuildRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopBuildRequest>(classId, &StopBuildRequest::Create);
}

std::string StopBuildRequest::StaticClassName()
{
    return "StopBuildRequest";
}

std::string StopBuildRequest::ClassName() const
{
    return "StopBuildRequest";
}

void StopBuildRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopBuildRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StopBuildRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopBuildRequest::~StopBuildRequest()
{
}

std::unique_ptr<sngxml::dom::Element> StopBuildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int StopBuildReply::classId = -1;

StopBuildReply::StopBuildReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr), isOwned(false)
{
}

void* StopBuildReply::Create()
{
    return new StopBuildReply();
}

void StopBuildReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopBuildReply>(classId, &StopBuildReply::Create);
}

std::string StopBuildReply::StaticClassName()
{
    return "StopBuildReply";
}

std::string StopBuildReply::ClassName() const
{
    return "StopBuildReply";
}

void StopBuildReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopBuildReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StopBuildReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopBuildReply::~StopBuildReply()
{
}

std::unique_ptr<sngxml::dom::Element> StopBuildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int KeepAliveBuildRequest::classId = -1;

KeepAliveBuildRequest::KeepAliveBuildRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr), isOwned(false)
{
}

void* KeepAliveBuildRequest::Create()
{
    return new KeepAliveBuildRequest();
}

void KeepAliveBuildRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<KeepAliveBuildRequest>(classId, &KeepAliveBuildRequest::Create);
}

std::string KeepAliveBuildRequest::StaticClassName()
{
    return "KeepAliveBuildRequest";
}

std::string KeepAliveBuildRequest::ClassName() const
{
    return "KeepAliveBuildRequest";
}

void KeepAliveBuildRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> KeepAliveBuildRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void KeepAliveBuildRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

KeepAliveBuildRequest::~KeepAliveBuildRequest()
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveBuildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int KeepAliveBuildReply::classId = -1;

KeepAliveBuildReply::KeepAliveBuildReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr), isOwned(false)
{
}

void* KeepAliveBuildReply::Create()
{
    return new KeepAliveBuildReply();
}

void KeepAliveBuildReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<KeepAliveBuildReply>(classId, &KeepAliveBuildReply::Create);
}

std::string KeepAliveBuildReply::StaticClassName()
{
    return "KeepAliveBuildReply";
}

std::string KeepAliveBuildReply::ClassName() const
{
    return "KeepAliveBuildReply";
}

void KeepAliveBuildReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> KeepAliveBuildReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void KeepAliveBuildReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

KeepAliveBuildReply::~KeepAliveBuildReply()
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveBuildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int BuildRequest::classId = -1;

BuildRequest::BuildRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), messageKind(), backend(), config(), optimizationLevel(), filePath(), defines(), numBuildThreads(), toolChain(), verbose(), quiet(), clean(), strictNothrow(), emitIR(), emitOptIR(), linkWithDebugRuntime(), linkUsingMsLink(), rebuild(), buildAllDependencies(), forceDebugInfo(), forceNoDebugInfo(), disableModuleCache(), singleThreadedCompile(), disableCodeGen(), justMyCode(), dataPtr(nullptr), isOwned(false)
{
}

void* BuildRequest::Create()
{
    return new BuildRequest();
}

void BuildRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BuildRequest>(classId, &BuildRequest::Create);
}

std::string BuildRequest::StaticClassName()
{
    return "BuildRequest";
}

std::string BuildRequest::ClassName() const
{
    return "BuildRequest";
}

void BuildRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
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

std::vector<sngxml::xmlser::XmlPtrBase*> BuildRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BuildRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BuildRequest::~BuildRequest()
{
}

std::unique_ptr<sngxml::dom::Element> BuildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
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

int CompileError::classId = -1;

CompileError::CompileError()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), message(), project(), file(), line(), scol(), ecol(), dataPtr(nullptr), isOwned(false)
{
}

void* CompileError::Create()
{
    return new CompileError();
}

void CompileError::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<CompileError>(classId, &CompileError::Create);
}

std::string CompileError::StaticClassName()
{
    return "CompileError";
}

std::string CompileError::ClassName() const
{
    return "CompileError";
}

void CompileError::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "message", message);
    sngxml::xmlser::FromXml(element, "project", project);
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "scol", scol);
    sngxml::xmlser::FromXml(element, "ecol", ecol);
}

std::vector<sngxml::xmlser::XmlPtrBase*> CompileError::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void CompileError::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

CompileError::~CompileError()
{
}

std::unique_ptr<sngxml::dom::Element> CompileError::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(message, "message").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(project, "project").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(scol, "scol").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ecol, "ecol").release()));
    return element;
}

int BuildReply::classId = -1;

BuildReply::BuildReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), requestValid(), requestErrorMessage(), logException(), time(), success(), errors(), dataPtr(nullptr), isOwned(false)
{
}

void* BuildReply::Create()
{
    return new BuildReply();
}

void BuildReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BuildReply>(classId, &BuildReply::Create);
}

std::string BuildReply::StaticClassName()
{
    return "BuildReply";
}

std::string BuildReply::ClassName() const
{
    return "BuildReply";
}

void BuildReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "requestValid", requestValid);
    sngxml::xmlser::FromXml(element, "requestErrorMessage", requestErrorMessage);
    sngxml::xmlser::FromXml(element, "logException", logException);
    sngxml::xmlser::FromXml(element, "time", time);
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "errors", errors);
}

std::vector<sngxml::xmlser::XmlPtrBase*> BuildReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BuildReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BuildReply::~BuildReply()
{
}

std::unique_ptr<sngxml::dom::Element> BuildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestValid, "requestValid").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestErrorMessage, "requestErrorMessage").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(logException, "logException").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(time, "time").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(errors, "errors").release()));
    return element;
}

int LogBuildMessageRequest::classId = -1;

LogBuildMessageRequest::LogBuildMessageRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), message(), dataPtr(nullptr), isOwned(false)
{
}

void* LogBuildMessageRequest::Create()
{
    return new LogBuildMessageRequest();
}

void LogBuildMessageRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<LogBuildMessageRequest>(classId, &LogBuildMessageRequest::Create);
}

std::string LogBuildMessageRequest::StaticClassName()
{
    return "LogBuildMessageRequest";
}

std::string LogBuildMessageRequest::ClassName() const
{
    return "LogBuildMessageRequest";
}

void LogBuildMessageRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "message", message);
}

std::vector<sngxml::xmlser::XmlPtrBase*> LogBuildMessageRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void LogBuildMessageRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

LogBuildMessageRequest::~LogBuildMessageRequest()
{
}

std::unique_ptr<sngxml::dom::Element> LogBuildMessageRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(message, "message").release()));
    return element;
}

int LogBuildMessageReply::classId = -1;

LogBuildMessageReply::LogBuildMessageReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), ok(), dataPtr(nullptr), isOwned(false)
{
}

void* LogBuildMessageReply::Create()
{
    return new LogBuildMessageReply();
}

void LogBuildMessageReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<LogBuildMessageReply>(classId, &LogBuildMessageReply::Create);
}

std::string LogBuildMessageReply::StaticClassName()
{
    return "LogBuildMessageReply";
}

std::string LogBuildMessageReply::ClassName() const
{
    return "LogBuildMessageReply";
}

void LogBuildMessageReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "ok", ok);
}

std::vector<sngxml::xmlser::XmlPtrBase*> LogBuildMessageReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void LogBuildMessageReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

LogBuildMessageReply::~LogBuildMessageReply()
{
}

std::unique_ptr<sngxml::dom::Element> LogBuildMessageReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    return element;
}

int BuildProgressMessage::classId = -1;

BuildProgressMessage::BuildProgressMessage()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr), isOwned(false)
{
}

void* BuildProgressMessage::Create()
{
    return new BuildProgressMessage();
}

void BuildProgressMessage::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BuildProgressMessage>(classId, &BuildProgressMessage::Create);
}

std::string BuildProgressMessage::StaticClassName()
{
    return "BuildProgressMessage";
}

std::string BuildProgressMessage::ClassName() const
{
    return "BuildProgressMessage";
}

void BuildProgressMessage::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> BuildProgressMessage::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BuildProgressMessage::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BuildProgressMessage::~BuildProgressMessage()
{
}

std::unique_ptr<sngxml::dom::Element> BuildProgressMessage::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int GenericBuildErrorReply::classId = -1;

GenericBuildErrorReply::GenericBuildErrorReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), error(), dataPtr(nullptr), isOwned(false)
{
}

void* GenericBuildErrorReply::Create()
{
    return new GenericBuildErrorReply();
}

void GenericBuildErrorReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GenericBuildErrorReply>(classId, &GenericBuildErrorReply::Create);
}

std::string GenericBuildErrorReply::StaticClassName()
{
    return "GenericBuildErrorReply";
}

std::string GenericBuildErrorReply::ClassName() const
{
    return "GenericBuildErrorReply";
}

void GenericBuildErrorReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GenericBuildErrorReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void GenericBuildErrorReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GenericBuildErrorReply::~GenericBuildErrorReply()
{
}

std::unique_ptr<sngxml::dom::Element> GenericBuildErrorReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int CacheModuleRequest::classId = -1;

CacheModuleRequest::CacheModuleRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), projectName(), projectFilePath(), backend(), config(), dataPtr(nullptr), isOwned(false)
{
}

void* CacheModuleRequest::Create()
{
    return new CacheModuleRequest();
}

void CacheModuleRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<CacheModuleRequest>(classId, &CacheModuleRequest::Create);
}

std::string CacheModuleRequest::StaticClassName()
{
    return "CacheModuleRequest";
}

std::string CacheModuleRequest::ClassName() const
{
    return "CacheModuleRequest";
}

void CacheModuleRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "projectName", projectName);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
}

std::vector<sngxml::xmlser::XmlPtrBase*> CacheModuleRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void CacheModuleRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

CacheModuleRequest::~CacheModuleRequest()
{
}

std::unique_ptr<sngxml::dom::Element> CacheModuleRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectName, "projectName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    return element;
}

int CacheModuleReply::classId = -1;

CacheModuleReply::CacheModuleReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), error(), dataPtr(nullptr), isOwned(false)
{
}

void* CacheModuleReply::Create()
{
    return new CacheModuleReply();
}

void CacheModuleReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<CacheModuleReply>(classId, &CacheModuleReply::Create);
}

std::string CacheModuleReply::StaticClassName()
{
    return "CacheModuleReply";
}

std::string CacheModuleReply::ClassName() const
{
    return "CacheModuleReply";
}

void CacheModuleReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> CacheModuleReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void CacheModuleReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

CacheModuleReply::~CacheModuleReply()
{
}

std::unique_ptr<sngxml::dom::Element> CacheModuleReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int DefinitionSourceLocation::classId = -1;

DefinitionSourceLocation::DefinitionSourceLocation()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), file(), line(), scol(), dataPtr(nullptr), isOwned(false)
{
}

void* DefinitionSourceLocation::Create()
{
    return new DefinitionSourceLocation();
}

void DefinitionSourceLocation::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<DefinitionSourceLocation>(classId, &DefinitionSourceLocation::Create);
}

std::string DefinitionSourceLocation::StaticClassName()
{
    return "DefinitionSourceLocation";
}

std::string DefinitionSourceLocation::ClassName() const
{
    return "DefinitionSourceLocation";
}

void DefinitionSourceLocation::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "scol", scol);
}

std::vector<sngxml::xmlser::XmlPtrBase*> DefinitionSourceLocation::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void DefinitionSourceLocation::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

DefinitionSourceLocation::~DefinitionSourceLocation()
{
}

std::unique_ptr<sngxml::dom::Element> DefinitionSourceLocation::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(scol, "scol").release()));
    return element;
}

int GetDefinitionRequest::classId = -1;

GetDefinitionRequest::GetDefinitionRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), projectName(), projectFilePath(), backend(), config(), identifier(), identifierLocation(), dataPtr(nullptr), isOwned(false)
{
}

void* GetDefinitionRequest::Create()
{
    return new GetDefinitionRequest();
}

void GetDefinitionRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetDefinitionRequest>(classId, &GetDefinitionRequest::Create);
}

std::string GetDefinitionRequest::StaticClassName()
{
    return "GetDefinitionRequest";
}

std::string GetDefinitionRequest::ClassName() const
{
    return "GetDefinitionRequest";
}

void GetDefinitionRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "projectName", projectName);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "identifier", identifier);
    sngxml::xmlser::FromXml(element, "identifierLocation", identifierLocation);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetDefinitionRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void GetDefinitionRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetDefinitionRequest::~GetDefinitionRequest()
{
}

std::unique_ptr<sngxml::dom::Element> GetDefinitionRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectName, "projectName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(identifier, "identifier").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(identifierLocation, "identifierLocation").release()));
    return element;
}

int GetDefinitionReply::classId = -1;

GetDefinitionReply::GetDefinitionReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), definitionLocation(), ok(), error(), dataPtr(nullptr), isOwned(false)
{
}

void* GetDefinitionReply::Create()
{
    return new GetDefinitionReply();
}

void GetDefinitionReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetDefinitionReply>(classId, &GetDefinitionReply::Create);
}

std::string GetDefinitionReply::StaticClassName()
{
    return "GetDefinitionReply";
}

std::string GetDefinitionReply::ClassName() const
{
    return "GetDefinitionReply";
}

void GetDefinitionReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "definitionLocation", definitionLocation);
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetDefinitionReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void GetDefinitionReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetDefinitionReply::~GetDefinitionReply()
{
}

std::unique_ptr<sngxml::dom::Element> GetDefinitionReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(definitionLocation, "definitionLocation").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

