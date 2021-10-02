#include "CodeCompletionServerMessage.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int CodeCompletionRequest::classId = -1;

CodeCompletionRequest::CodeCompletionRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), created(), received(), dataPtr(nullptr)
{
}

void* CodeCompletionRequest::Create()
{
    return new CodeCompletionRequest();
}

void CodeCompletionRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<CodeCompletionRequest>(classId, &CodeCompletionRequest::Create);
}

std::string CodeCompletionRequest::StaticClassName()
{
    return "CodeCompletionRequest";
}

std::string CodeCompletionRequest::ClassName() const
{
    return "CodeCompletionRequest";
}

void CodeCompletionRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "created", created);
    sngxml::xmlser::FromXml(element, "received", received);
}

std::vector<sngxml::xmlser::XmlPtrBase*> CodeCompletionRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void CodeCompletionRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

CodeCompletionRequest::~CodeCompletionRequest()
{
}

std::unique_ptr<sngxml::dom::Element> CodeCompletionRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(created, "created").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(received, "received").release()));
    return element;
}

int CodeCompletionReply::classId = -1;

CodeCompletionReply::CodeCompletionReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), requestCreated(), requestReceived(), created(), dataPtr(nullptr)
{
}

void* CodeCompletionReply::Create()
{
    return new CodeCompletionReply();
}

void CodeCompletionReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<CodeCompletionReply>(classId, &CodeCompletionReply::Create);
}

std::string CodeCompletionReply::StaticClassName()
{
    return "CodeCompletionReply";
}

std::string CodeCompletionReply::ClassName() const
{
    return "CodeCompletionReply";
}

void CodeCompletionReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "requestCreated", requestCreated);
    sngxml::xmlser::FromXml(element, "requestReceived", requestReceived);
    sngxml::xmlser::FromXml(element, "created", created);
}

std::vector<sngxml::xmlser::XmlPtrBase*> CodeCompletionReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void CodeCompletionReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

CodeCompletionReply::~CodeCompletionReply()
{
}

std::unique_ptr<sngxml::dom::Element> CodeCompletionReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestCreated, "requestCreated").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(requestReceived, "requestReceived").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(created, "created").release()));
    return element;
}

int LoadEditModuleRequest::classId = -1;

LoadEditModuleRequest::LoadEditModuleRequest()
     : CodeCompletionRequest(), projectFilePath(), backend(), config()
{
}

void* LoadEditModuleRequest::Create()
{
    return new LoadEditModuleRequest();
}

void LoadEditModuleRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<LoadEditModuleRequest>(classId, &LoadEditModuleRequest::Create);
}

std::string LoadEditModuleRequest::StaticClassName()
{
    return "LoadEditModuleRequest";
}

std::string LoadEditModuleRequest::ClassName() const
{
    return "LoadEditModuleRequest";
}

void LoadEditModuleRequest::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionRequest::FromXml(element);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
}

std::vector<sngxml::xmlser::XmlPtrBase*> LoadEditModuleRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionRequest::GetPtrs();
    return ptrs;
}

void LoadEditModuleRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

LoadEditModuleRequest::~LoadEditModuleRequest()
{
}

std::unique_ptr<sngxml::dom::Element> LoadEditModuleRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionRequest::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    return element;
}

int LoadEditModuleReply::classId = -1;

LoadEditModuleReply::LoadEditModuleReply()
     : CodeCompletionReply(), key(), result(), error(), startLoading(), startParsing(), endParsing(), endLoading()
{
}

void* LoadEditModuleReply::Create()
{
    return new LoadEditModuleReply();
}

void LoadEditModuleReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<LoadEditModuleReply>(classId, &LoadEditModuleReply::Create);
}

std::string LoadEditModuleReply::StaticClassName()
{
    return "LoadEditModuleReply";
}

std::string LoadEditModuleReply::ClassName() const
{
    return "LoadEditModuleReply";
}

void LoadEditModuleReply::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionReply::FromXml(element);
    sngxml::xmlser::FromXml(element, "key", key);
    sngxml::xmlser::FromXml(element, "result", result);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "startLoading", startLoading);
    sngxml::xmlser::FromXml(element, "startParsing", startParsing);
    sngxml::xmlser::FromXml(element, "endParsing", endParsing);
    sngxml::xmlser::FromXml(element, "endLoading", endLoading);
}

std::vector<sngxml::xmlser::XmlPtrBase*> LoadEditModuleReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionReply::GetPtrs();
    return ptrs;
}

void LoadEditModuleReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

LoadEditModuleReply::~LoadEditModuleReply()
{
}

std::unique_ptr<sngxml::dom::Element> LoadEditModuleReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionReply::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(key, "key").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(result, "result").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startLoading, "startLoading").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startParsing, "startParsing").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endParsing, "endParsing").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endLoading, "endLoading").release()));
    return element;
}

int ResetEditModuleCacheRequest::classId = -1;

ResetEditModuleCacheRequest::ResetEditModuleCacheRequest()
     : CodeCompletionRequest()
{
}

void* ResetEditModuleCacheRequest::Create()
{
    return new ResetEditModuleCacheRequest();
}

void ResetEditModuleCacheRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ResetEditModuleCacheRequest>(classId, &ResetEditModuleCacheRequest::Create);
}

std::string ResetEditModuleCacheRequest::StaticClassName()
{
    return "ResetEditModuleCacheRequest";
}

std::string ResetEditModuleCacheRequest::ClassName() const
{
    return "ResetEditModuleCacheRequest";
}

void ResetEditModuleCacheRequest::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionRequest::FromXml(element);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ResetEditModuleCacheRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionRequest::GetPtrs();
    return ptrs;
}

void ResetEditModuleCacheRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ResetEditModuleCacheRequest::~ResetEditModuleCacheRequest()
{
}

std::unique_ptr<sngxml::dom::Element> ResetEditModuleCacheRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionRequest::ToXml(fieldName);
    return element;
}

int ResetEditModuleCacheReply::classId = -1;

ResetEditModuleCacheReply::ResetEditModuleCacheReply()
     : CodeCompletionReply(), result(), error()
{
}

void* ResetEditModuleCacheReply::Create()
{
    return new ResetEditModuleCacheReply();
}

void ResetEditModuleCacheReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ResetEditModuleCacheReply>(classId, &ResetEditModuleCacheReply::Create);
}

std::string ResetEditModuleCacheReply::StaticClassName()
{
    return "ResetEditModuleCacheReply";
}

std::string ResetEditModuleCacheReply::ClassName() const
{
    return "ResetEditModuleCacheReply";
}

void ResetEditModuleCacheReply::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionReply::FromXml(element);
    sngxml::xmlser::FromXml(element, "result", result);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ResetEditModuleCacheReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionReply::GetPtrs();
    return ptrs;
}

void ResetEditModuleCacheReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ResetEditModuleCacheReply::~ResetEditModuleCacheReply()
{
}

std::unique_ptr<sngxml::dom::Element> ResetEditModuleCacheReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionReply::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(result, "result").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int ParseSourceRequest::classId = -1;

ParseSourceRequest::ParseSourceRequest()
     : CodeCompletionRequest(), projectFilePath(), backend(), config(), sourceFilePath(), sourceCode()
{
}

void* ParseSourceRequest::Create()
{
    return new ParseSourceRequest();
}

void ParseSourceRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ParseSourceRequest>(classId, &ParseSourceRequest::Create);
}

std::string ParseSourceRequest::StaticClassName()
{
    return "ParseSourceRequest";
}

std::string ParseSourceRequest::ClassName() const
{
    return "ParseSourceRequest";
}

void ParseSourceRequest::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionRequest::FromXml(element);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "sourceFilePath", sourceFilePath);
    sngxml::xmlser::FromXml(element, "sourceCode", sourceCode);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ParseSourceRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionRequest::GetPtrs();
    return ptrs;
}

void ParseSourceRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ParseSourceRequest::~ParseSourceRequest()
{
}

std::unique_ptr<sngxml::dom::Element> ParseSourceRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionRequest::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceFilePath, "sourceFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceCode, "sourceCode").release()));
    return element;
}

int ParseSourceReply::classId = -1;

ParseSourceReply::ParseSourceReply()
     : CodeCompletionReply(), ok(), error(), errors(), synchronized(), cursorContainer(), ruleContext(), startParsing(), endParsing()
{
}

void* ParseSourceReply::Create()
{
    return new ParseSourceReply();
}

void ParseSourceReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ParseSourceReply>(classId, &ParseSourceReply::Create);
}

std::string ParseSourceReply::StaticClassName()
{
    return "ParseSourceReply";
}

std::string ParseSourceReply::ClassName() const
{
    return "ParseSourceReply";
}

void ParseSourceReply::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionReply::FromXml(element);
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "errors", errors);
    sngxml::xmlser::FromXml(element, "synchronized", synchronized);
    sngxml::xmlser::FromXml(element, "cursorContainer", cursorContainer);
    sngxml::xmlser::FromXml(element, "ruleContext", ruleContext);
    sngxml::xmlser::FromXml(element, "startParsing", startParsing);
    sngxml::xmlser::FromXml(element, "endParsing", endParsing);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ParseSourceReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionReply::GetPtrs();
    return ptrs;
}

void ParseSourceReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ParseSourceReply::~ParseSourceReply()
{
}

std::unique_ptr<sngxml::dom::Element> ParseSourceReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionReply::ToXml(fieldName);
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

int GetCCListRequest::classId = -1;

GetCCListRequest::GetCCListRequest()
     : CodeCompletionRequest(), projectFilePath(), backend(), config(), sourceFilePath(), ccText(), ruleContext(), cursorLine()
{
}

void* GetCCListRequest::Create()
{
    return new GetCCListRequest();
}

void GetCCListRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetCCListRequest>(classId, &GetCCListRequest::Create);
}

std::string GetCCListRequest::StaticClassName()
{
    return "GetCCListRequest";
}

std::string GetCCListRequest::ClassName() const
{
    return "GetCCListRequest";
}

void GetCCListRequest::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionRequest::FromXml(element);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "sourceFilePath", sourceFilePath);
    sngxml::xmlser::FromXml(element, "ccText", ccText);
    sngxml::xmlser::FromXml(element, "ruleContext", ruleContext);
    sngxml::xmlser::FromXml(element, "cursorLine", cursorLine);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetCCListRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionRequest::GetPtrs();
    return ptrs;
}

void GetCCListRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetCCListRequest::~GetCCListRequest()
{
}

std::unique_ptr<sngxml::dom::Element> GetCCListRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionRequest::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceFilePath, "sourceFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ccText, "ccText").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ruleContext, "ruleContext").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(cursorLine, "cursorLine").release()));
    return element;
}

int GetCCListReply::classId = -1;

GetCCListReply::GetCCListReply()
     : CodeCompletionReply(), ok(), error(), ccList(), startGetCCList(), endGetCCList()
{
}

void* GetCCListReply::Create()
{
    return new GetCCListReply();
}

void GetCCListReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetCCListReply>(classId, &GetCCListReply::Create);
}

std::string GetCCListReply::StaticClassName()
{
    return "GetCCListReply";
}

std::string GetCCListReply::ClassName() const
{
    return "GetCCListReply";
}

void GetCCListReply::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionReply::FromXml(element);
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "ccList", ccList);
    sngxml::xmlser::FromXml(element, "startGetCCList", startGetCCList);
    sngxml::xmlser::FromXml(element, "endGetCCList", endGetCCList);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetCCListReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionReply::GetPtrs();
    return ptrs;
}

void GetCCListReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetCCListReply::~GetCCListReply()
{
}

std::unique_ptr<sngxml::dom::Element> GetCCListReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionReply::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ccList, "ccList").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startGetCCList, "startGetCCList").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endGetCCList, "endGetCCList").release()));
    return element;
}

int GetParamHelpListRequest::classId = -1;

GetParamHelpListRequest::GetParamHelpListRequest()
     : CodeCompletionRequest(), projectFilePath(), backend(), config(), sourceFilePath(), symbolIndex()
{
}

void* GetParamHelpListRequest::Create()
{
    return new GetParamHelpListRequest();
}

void GetParamHelpListRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetParamHelpListRequest>(classId, &GetParamHelpListRequest::Create);
}

std::string GetParamHelpListRequest::StaticClassName()
{
    return "GetParamHelpListRequest";
}

std::string GetParamHelpListRequest::ClassName() const
{
    return "GetParamHelpListRequest";
}

void GetParamHelpListRequest::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionRequest::FromXml(element);
    sngxml::xmlser::FromXml(element, "projectFilePath", projectFilePath);
    sngxml::xmlser::FromXml(element, "backend", backend);
    sngxml::xmlser::FromXml(element, "config", config);
    sngxml::xmlser::FromXml(element, "sourceFilePath", sourceFilePath);
    sngxml::xmlser::FromXml(element, "symbolIndex", symbolIndex);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetParamHelpListRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionRequest::GetPtrs();
    return ptrs;
}

void GetParamHelpListRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetParamHelpListRequest::~GetParamHelpListRequest()
{
}

std::unique_ptr<sngxml::dom::Element> GetParamHelpListRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionRequest::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(projectFilePath, "projectFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(backend, "backend").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(config, "config").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceFilePath, "sourceFilePath").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(symbolIndex, "symbolIndex").release()));
    return element;
}

int GetParamHelpListReply::classId = -1;

GetParamHelpListReply::GetParamHelpListReply()
     : CodeCompletionReply(), ok(), error(), list(), startGetParamHelpList(), endGetParamHelpList()
{
}

void* GetParamHelpListReply::Create()
{
    return new GetParamHelpListReply();
}

void GetParamHelpListReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GetParamHelpListReply>(classId, &GetParamHelpListReply::Create);
}

std::string GetParamHelpListReply::StaticClassName()
{
    return "GetParamHelpListReply";
}

std::string GetParamHelpListReply::ClassName() const
{
    return "GetParamHelpListReply";
}

void GetParamHelpListReply::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionReply::FromXml(element);
    sngxml::xmlser::FromXml(element, "ok", ok);
    sngxml::xmlser::FromXml(element, "error", error);
    sngxml::xmlser::FromXml(element, "list", list);
    sngxml::xmlser::FromXml(element, "startGetParamHelpList", startGetParamHelpList);
    sngxml::xmlser::FromXml(element, "endGetParamHelpList", endGetParamHelpList);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GetParamHelpListReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionReply::GetPtrs();
    return ptrs;
}

void GetParamHelpListReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GetParamHelpListReply::~GetParamHelpListReply()
{
}

std::unique_ptr<sngxml::dom::Element> GetParamHelpListReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionReply::ToXml(fieldName);
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ok, "ok").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(list, "list").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(startGetParamHelpList, "startGetParamHelpList").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(endGetParamHelpList, "endGetParamHelpList").release()));
    return element;
}

int StopCCRequest::classId = -1;

StopCCRequest::StopCCRequest()
     : CodeCompletionRequest()
{
}

void* StopCCRequest::Create()
{
    return new StopCCRequest();
}

void StopCCRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopCCRequest>(classId, &StopCCRequest::Create);
}

std::string StopCCRequest::StaticClassName()
{
    return "StopCCRequest";
}

std::string StopCCRequest::ClassName() const
{
    return "StopCCRequest";
}

void StopCCRequest::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionRequest::FromXml(element);
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopCCRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionRequest::GetPtrs();
    return ptrs;
}

void StopCCRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopCCRequest::~StopCCRequest()
{
}

std::unique_ptr<sngxml::dom::Element> StopCCRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionRequest::ToXml(fieldName);
    return element;
}

int StopCCReply::classId = -1;

StopCCReply::StopCCReply()
     : CodeCompletionReply()
{
}

void* StopCCReply::Create()
{
    return new StopCCReply();
}

void StopCCReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopCCReply>(classId, &StopCCReply::Create);
}

std::string StopCCReply::StaticClassName()
{
    return "StopCCReply";
}

std::string StopCCReply::ClassName() const
{
    return "StopCCReply";
}

void StopCCReply::FromXml(sngxml::dom::Element* element)
{
    CodeCompletionReply::FromXml(element);
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopCCReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = CodeCompletionReply::GetPtrs();
    return ptrs;
}

void StopCCReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopCCReply::~StopCCReply()
{
}

std::unique_ptr<sngxml::dom::Element> StopCCReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element = CodeCompletionReply::ToXml(fieldName);
    return element;
}

int KeepAliveCCRequest::classId = -1;

KeepAliveCCRequest::KeepAliveCCRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* KeepAliveCCRequest::Create()
{
    return new KeepAliveCCRequest();
}

void KeepAliveCCRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<KeepAliveCCRequest>(classId, &KeepAliveCCRequest::Create);
}

std::string KeepAliveCCRequest::StaticClassName()
{
    return "KeepAliveCCRequest";
}

std::string KeepAliveCCRequest::ClassName() const
{
    return "KeepAliveCCRequest";
}

void KeepAliveCCRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> KeepAliveCCRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void KeepAliveCCRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

KeepAliveCCRequest::~KeepAliveCCRequest()
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveCCRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int KeepAliveCCReply::classId = -1;

KeepAliveCCReply::KeepAliveCCReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* KeepAliveCCReply::Create()
{
    return new KeepAliveCCReply();
}

void KeepAliveCCReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<KeepAliveCCReply>(classId, &KeepAliveCCReply::Create);
}

std::string KeepAliveCCReply::StaticClassName()
{
    return "KeepAliveCCReply";
}

std::string KeepAliveCCReply::ClassName() const
{
    return "KeepAliveCCReply";
}

void KeepAliveCCReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> KeepAliveCCReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void KeepAliveCCReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

KeepAliveCCReply::~KeepAliveCCReply()
{
}

std::unique_ptr<sngxml::dom::Element> KeepAliveCCReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

