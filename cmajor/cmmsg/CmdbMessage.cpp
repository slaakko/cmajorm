#include "CmdbMessage.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int Location::classId = -1;

Location::Location()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), func(), file(), line(), scol(), ecol(), level(), dataPtr(nullptr)
{
}

void* Location::Create()
{
    return new Location();
}

void Location::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Location>(classId, &Location::Create);
}

std::string Location::StaticClassName()
{
    return "Location";
}

std::string Location::ClassName() const
{
    return "Location";
}

void Location::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "func", func);
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "scol", scol);
    sngxml::xmlser::FromXml(element, "ecol", ecol);
    sngxml::xmlser::FromXml(element, "level", level);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Location::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void Location::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Location::~Location()
{
}

std::unique_ptr<sngxml::dom::Element> Location::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(func, "func").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(scol, "scol").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(ecol, "ecol").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(level, "level").release()));
    return element;
}

int Type::classId = -1;

Type::Type()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), name(), id(), dataPtr(nullptr)
{
}

void* Type::Create()
{
    return new Type();
}

void Type::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Type>(classId, &Type::Create);
}

std::string Type::StaticClassName()
{
    return "Type";
}

std::string Type::ClassName() const
{
    return "Type";
}

void Type::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "name", name);
    sngxml::xmlser::FromXml(element, "id", id);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Type::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void Type::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Type::~Type()
{
}

std::unique_ptr<sngxml::dom::Element> Type::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(name, "name").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(id, "id").release()));
    return element;
}

int Result::classId = -1;

Result::Result()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), staticType(), dynamicType(), initialized(), value(), dataPtr(nullptr)
{
}

void* Result::Create()
{
    return new Result();
}

void Result::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Result>(classId, &Result::Create);
}

std::string Result::StaticClassName()
{
    return "Result";
}

std::string Result::ClassName() const
{
    return "Result";
}

void Result::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "staticType", staticType);
    sngxml::xmlser::FromXml(element, "dynamicType", dynamicType);
    sngxml::xmlser::FromXml(element, "initialized", initialized);
    sngxml::xmlser::FromXml(element, "value", value);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Result::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void Result::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Result::~Result()
{
}

std::unique_ptr<sngxml::dom::Element> Result::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(staticType, "staticType").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(dynamicType, "dynamicType").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(initialized, "initialized").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(value, "value").release()));
    return element;
}

int ChildResult::classId = -1;

ChildResult::ChildResult()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), expr(), name(), type(), dynType(), value(), count(), dataPtr(nullptr)
{
}

void* ChildResult::Create()
{
    return new ChildResult();
}

void ChildResult::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ChildResult>(classId, &ChildResult::Create);
}

std::string ChildResult::StaticClassName()
{
    return "ChildResult";
}

std::string ChildResult::ClassName() const
{
    return "ChildResult";
}

void ChildResult::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "expr", expr);
    sngxml::xmlser::FromXml(element, "name", name);
    sngxml::xmlser::FromXml(element, "type", type);
    sngxml::xmlser::FromXml(element, "dynType", dynType);
    sngxml::xmlser::FromXml(element, "value", value);
    sngxml::xmlser::FromXml(element, "count", count);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ChildResult::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ChildResult::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ChildResult::~ChildResult()
{
}

std::unique_ptr<sngxml::dom::Element> ChildResult::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expr, "expr").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(name, "name").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(type, "type").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(dynType, "dynType").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(value, "value").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(count, "count").release()));
    return element;
}

int TargetState::classId = -1;

TargetState::TargetState()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), stopReason(), exitCode(), breakpointId(), signalName(), signalMeaning(), dataPtr(nullptr)
{
}

void* TargetState::Create()
{
    return new TargetState();
}

void TargetState::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<TargetState>(classId, &TargetState::Create);
}

std::string TargetState::StaticClassName()
{
    return "TargetState";
}

std::string TargetState::ClassName() const
{
    return "TargetState";
}

void TargetState::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "stopReason", stopReason);
    sngxml::xmlser::FromXml(element, "exitCode", exitCode);
    sngxml::xmlser::FromXml(element, "breakpointId", breakpointId);
    sngxml::xmlser::FromXml(element, "signalName", signalName);
    sngxml::xmlser::FromXml(element, "signalMeaning", signalMeaning);
}

std::vector<sngxml::xmlser::XmlPtrBase*> TargetState::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void TargetState::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

TargetState::~TargetState()
{
}

std::unique_ptr<sngxml::dom::Element> TargetState::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(stopReason, "stopReason").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(exitCode, "exitCode").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointId, "breakpointId").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(signalName, "signalName").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(signalMeaning, "signalMeaning").release()));
    return element;
}

int SourceLoc::classId = -1;

SourceLoc::SourceLoc()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), path(), line(), dataPtr(nullptr)
{
}

void* SourceLoc::Create()
{
    return new SourceLoc();
}

void SourceLoc::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<SourceLoc>(classId, &SourceLoc::Create);
}

std::string SourceLoc::StaticClassName()
{
    return "SourceLoc";
}

std::string SourceLoc::ClassName() const
{
    return "SourceLoc";
}

void SourceLoc::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "path", path);
    sngxml::xmlser::FromXml(element, "line", line);
}

std::vector<sngxml::xmlser::XmlPtrBase*> SourceLoc::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void SourceLoc::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

SourceLoc::~SourceLoc()
{
}

std::unique_ptr<sngxml::dom::Element> SourceLoc::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(path, "path").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    return element;
}

int BreakpointInfo::classId = -1;

BreakpointInfo::BreakpointInfo()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), breakpointId(), numInsts(), location(), error(), dataPtr(nullptr)
{
}

void* BreakpointInfo::Create()
{
    return new BreakpointInfo();
}

void BreakpointInfo::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BreakpointInfo>(classId, &BreakpointInfo::Create);
}

std::string BreakpointInfo::StaticClassName()
{
    return "BreakpointInfo";
}

std::string BreakpointInfo::ClassName() const
{
    return "BreakpointInfo";
}

void BreakpointInfo::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "breakpointId", breakpointId);
    sngxml::xmlser::FromXml(element, "numInsts", numInsts);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> BreakpointInfo::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BreakpointInfo::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BreakpointInfo::~BreakpointInfo()
{
}

std::unique_ptr<sngxml::dom::Element> BreakpointInfo::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointId, "breakpointId").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(numInsts, "numInsts").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int StartDebugRequest::classId = -1;

StartDebugRequest::StartDebugRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), breakpointLocations(), dataPtr(nullptr)
{
}

void* StartDebugRequest::Create()
{
    return new StartDebugRequest();
}

void StartDebugRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StartDebugRequest>(classId, &StartDebugRequest::Create);
}

std::string StartDebugRequest::StaticClassName()
{
    return "StartDebugRequest";
}

std::string StartDebugRequest::ClassName() const
{
    return "StartDebugRequest";
}

void StartDebugRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "breakpointLocations", breakpointLocations);
}

std::vector<sngxml::xmlser::XmlPtrBase*> StartDebugRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StartDebugRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StartDebugRequest::~StartDebugRequest()
{
}

std::unique_ptr<sngxml::dom::Element> StartDebugRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointLocations, "breakpointLocations").release()));
    return element;
}

int StartDebugReply::classId = -1;

StartDebugReply::StartDebugReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), location(), state(), breakpointInfos(), error(), dataPtr(nullptr)
{
}

void* StartDebugReply::Create()
{
    return new StartDebugReply();
}

void StartDebugReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StartDebugReply>(classId, &StartDebugReply::Create);
}

std::string StartDebugReply::StaticClassName()
{
    return "StartDebugReply";
}

std::string StartDebugReply::ClassName() const
{
    return "StartDebugReply";
}

void StartDebugReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "breakpointInfos", breakpointInfos);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> StartDebugReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StartDebugReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StartDebugReply::~StartDebugReply()
{
}

std::unique_ptr<sngxml::dom::Element> StartDebugReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointInfos, "breakpointInfos").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int StopDebugRequest::classId = -1;

StopDebugRequest::StopDebugRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* StopDebugRequest::Create()
{
    return new StopDebugRequest();
}

void StopDebugRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopDebugRequest>(classId, &StopDebugRequest::Create);
}

std::string StopDebugRequest::StaticClassName()
{
    return "StopDebugRequest";
}

std::string StopDebugRequest::ClassName() const
{
    return "StopDebugRequest";
}

void StopDebugRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopDebugRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StopDebugRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopDebugRequest::~StopDebugRequest()
{
}

std::unique_ptr<sngxml::dom::Element> StopDebugRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int StopDebugReply::classId = -1;

StopDebugReply::StopDebugReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* StopDebugReply::Create()
{
    return new StopDebugReply();
}

void StopDebugReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StopDebugReply>(classId, &StopDebugReply::Create);
}

std::string StopDebugReply::StaticClassName()
{
    return "StopDebugReply";
}

std::string StopDebugReply::ClassName() const
{
    return "StopDebugReply";
}

void StopDebugReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> StopDebugReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StopDebugReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StopDebugReply::~StopDebugReply()
{
}

std::unique_ptr<sngxml::dom::Element> StopDebugReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int GenericDebugErrorReply::classId = -1;

GenericDebugErrorReply::GenericDebugErrorReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), errorMessage(), dataPtr(nullptr)
{
}

void* GenericDebugErrorReply::Create()
{
    return new GenericDebugErrorReply();
}

void GenericDebugErrorReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<GenericDebugErrorReply>(classId, &GenericDebugErrorReply::Create);
}

std::string GenericDebugErrorReply::StaticClassName()
{
    return "GenericDebugErrorReply";
}

std::string GenericDebugErrorReply::ClassName() const
{
    return "GenericDebugErrorReply";
}

void GenericDebugErrorReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "errorMessage", errorMessage);
}

std::vector<sngxml::xmlser::XmlPtrBase*> GenericDebugErrorReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void GenericDebugErrorReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

GenericDebugErrorReply::~GenericDebugErrorReply()
{
}

std::unique_ptr<sngxml::dom::Element> GenericDebugErrorReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(errorMessage, "errorMessage").release()));
    return element;
}

int ContinueRequest::classId = -1;

ContinueRequest::ContinueRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* ContinueRequest::Create()
{
    return new ContinueRequest();
}

void ContinueRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ContinueRequest>(classId, &ContinueRequest::Create);
}

std::string ContinueRequest::StaticClassName()
{
    return "ContinueRequest";
}

std::string ContinueRequest::ClassName() const
{
    return "ContinueRequest";
}

void ContinueRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> ContinueRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ContinueRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ContinueRequest::~ContinueRequest()
{
}

std::unique_ptr<sngxml::dom::Element> ContinueRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int ContinueReply::classId = -1;

ContinueReply::ContinueReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), location(), state(), error(), dataPtr(nullptr)
{
}

void* ContinueReply::Create()
{
    return new ContinueReply();
}

void ContinueReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ContinueReply>(classId, &ContinueReply::Create);
}

std::string ContinueReply::StaticClassName()
{
    return "ContinueReply";
}

std::string ContinueReply::ClassName() const
{
    return "ContinueReply";
}

void ContinueReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ContinueReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ContinueReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ContinueReply::~ContinueReply()
{
}

std::unique_ptr<sngxml::dom::Element> ContinueReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int NextRequest::classId = -1;

NextRequest::NextRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* NextRequest::Create()
{
    return new NextRequest();
}

void NextRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<NextRequest>(classId, &NextRequest::Create);
}

std::string NextRequest::StaticClassName()
{
    return "NextRequest";
}

std::string NextRequest::ClassName() const
{
    return "NextRequest";
}

void NextRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> NextRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void NextRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

NextRequest::~NextRequest()
{
}

std::unique_ptr<sngxml::dom::Element> NextRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int NextReply::classId = -1;

NextReply::NextReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), location(), state(), error(), dataPtr(nullptr)
{
}

void* NextReply::Create()
{
    return new NextReply();
}

void NextReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<NextReply>(classId, &NextReply::Create);
}

std::string NextReply::StaticClassName()
{
    return "NextReply";
}

std::string NextReply::ClassName() const
{
    return "NextReply";
}

void NextReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> NextReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void NextReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

NextReply::~NextReply()
{
}

std::unique_ptr<sngxml::dom::Element> NextReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int StepRequest::classId = -1;

StepRequest::StepRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* StepRequest::Create()
{
    return new StepRequest();
}

void StepRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StepRequest>(classId, &StepRequest::Create);
}

std::string StepRequest::StaticClassName()
{
    return "StepRequest";
}

std::string StepRequest::ClassName() const
{
    return "StepRequest";
}

void StepRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> StepRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StepRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StepRequest::~StepRequest()
{
}

std::unique_ptr<sngxml::dom::Element> StepRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int StepReply::classId = -1;

StepReply::StepReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), location(), state(), error(), dataPtr(nullptr)
{
}

void* StepReply::Create()
{
    return new StepReply();
}

void StepReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<StepReply>(classId, &StepReply::Create);
}

std::string StepReply::StaticClassName()
{
    return "StepReply";
}

std::string StepReply::ClassName() const
{
    return "StepReply";
}

void StepReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> StepReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void StepReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

StepReply::~StepReply()
{
}

std::unique_ptr<sngxml::dom::Element> StepReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int FinishRequest::classId = -1;

FinishRequest::FinishRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* FinishRequest::Create()
{
    return new FinishRequest();
}

void FinishRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<FinishRequest>(classId, &FinishRequest::Create);
}

std::string FinishRequest::StaticClassName()
{
    return "FinishRequest";
}

std::string FinishRequest::ClassName() const
{
    return "FinishRequest";
}

void FinishRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> FinishRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void FinishRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

FinishRequest::~FinishRequest()
{
}

std::unique_ptr<sngxml::dom::Element> FinishRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int FinishReply::classId = -1;

FinishReply::FinishReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), location(), state(), error(), dataPtr(nullptr)
{
}

void* FinishReply::Create()
{
    return new FinishReply();
}

void FinishReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<FinishReply>(classId, &FinishReply::Create);
}

std::string FinishReply::StaticClassName()
{
    return "FinishReply";
}

std::string FinishReply::ClassName() const
{
    return "FinishReply";
}

void FinishReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> FinishReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void FinishReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

FinishReply::~FinishReply()
{
}

std::unique_ptr<sngxml::dom::Element> FinishReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int UntilRequest::classId = -1;

UntilRequest::UntilRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), sourceLoc(), dataPtr(nullptr)
{
}

void* UntilRequest::Create()
{
    return new UntilRequest();
}

void UntilRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<UntilRequest>(classId, &UntilRequest::Create);
}

std::string UntilRequest::StaticClassName()
{
    return "UntilRequest";
}

std::string UntilRequest::ClassName() const
{
    return "UntilRequest";
}

void UntilRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "sourceLoc", sourceLoc);
}

std::vector<sngxml::xmlser::XmlPtrBase*> UntilRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void UntilRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

UntilRequest::~UntilRequest()
{
}

std::unique_ptr<sngxml::dom::Element> UntilRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(sourceLoc, "sourceLoc").release()));
    return element;
}

int UntilReply::classId = -1;

UntilReply::UntilReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), location(), state(), error(), dataPtr(nullptr)
{
}

void* UntilReply::Create()
{
    return new UntilReply();
}

void UntilReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<UntilReply>(classId, &UntilReply::Create);
}

std::string UntilReply::StaticClassName()
{
    return "UntilReply";
}

std::string UntilReply::ClassName() const
{
    return "UntilReply";
}

void UntilReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "state", state);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> UntilReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void UntilReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

UntilReply::~UntilReply()
{
}

std::unique_ptr<sngxml::dom::Element> UntilReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(state, "state").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int BreakRequest::classId = -1;

BreakRequest::BreakRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), breakpointLocation(), dataPtr(nullptr)
{
}

void* BreakRequest::Create()
{
    return new BreakRequest();
}

void BreakRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BreakRequest>(classId, &BreakRequest::Create);
}

std::string BreakRequest::StaticClassName()
{
    return "BreakRequest";
}

std::string BreakRequest::ClassName() const
{
    return "BreakRequest";
}

void BreakRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "breakpointLocation", breakpointLocation);
}

std::vector<sngxml::xmlser::XmlPtrBase*> BreakRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BreakRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BreakRequest::~BreakRequest()
{
}

std::unique_ptr<sngxml::dom::Element> BreakRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointLocation, "breakpointLocation").release()));
    return element;
}

int BreakReply::classId = -1;

BreakReply::BreakReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), breakpointInfo(), dataPtr(nullptr)
{
}

void* BreakReply::Create()
{
    return new BreakReply();
}

void BreakReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BreakReply>(classId, &BreakReply::Create);
}

std::string BreakReply::StaticClassName()
{
    return "BreakReply";
}

std::string BreakReply::ClassName() const
{
    return "BreakReply";
}

void BreakReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "breakpointInfo", breakpointInfo);
}

std::vector<sngxml::xmlser::XmlPtrBase*> BreakReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BreakReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BreakReply::~BreakReply()
{
}

std::unique_ptr<sngxml::dom::Element> BreakReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointInfo, "breakpointInfo").release()));
    return element;
}

int DeleteRequest::classId = -1;

DeleteRequest::DeleteRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), breakpointId(), dataPtr(nullptr)
{
}

void* DeleteRequest::Create()
{
    return new DeleteRequest();
}

void DeleteRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<DeleteRequest>(classId, &DeleteRequest::Create);
}

std::string DeleteRequest::StaticClassName()
{
    return "DeleteRequest";
}

std::string DeleteRequest::ClassName() const
{
    return "DeleteRequest";
}

void DeleteRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "breakpointId", breakpointId);
}

std::vector<sngxml::xmlser::XmlPtrBase*> DeleteRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void DeleteRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

DeleteRequest::~DeleteRequest()
{
}

std::unique_ptr<sngxml::dom::Element> DeleteRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpointId, "breakpointId").release()));
    return element;
}

int DeleteReply::classId = -1;

DeleteReply::DeleteReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), error(), dataPtr(nullptr)
{
}

void* DeleteReply::Create()
{
    return new DeleteReply();
}

void DeleteReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<DeleteReply>(classId, &DeleteReply::Create);
}

std::string DeleteReply::StaticClassName()
{
    return "DeleteReply";
}

std::string DeleteReply::ClassName() const
{
    return "DeleteReply";
}

void DeleteReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> DeleteReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void DeleteReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

DeleteReply::~DeleteReply()
{
}

std::unique_ptr<sngxml::dom::Element> DeleteReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int DepthRequest::classId = -1;

DepthRequest::DepthRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* DepthRequest::Create()
{
    return new DepthRequest();
}

void DepthRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<DepthRequest>(classId, &DepthRequest::Create);
}

std::string DepthRequest::StaticClassName()
{
    return "DepthRequest";
}

std::string DepthRequest::ClassName() const
{
    return "DepthRequest";
}

void DepthRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> DepthRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void DepthRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

DepthRequest::~DepthRequest()
{
}

std::unique_ptr<sngxml::dom::Element> DepthRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int DepthReply::classId = -1;

DepthReply::DepthReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), depth(), error(), dataPtr(nullptr)
{
}

void* DepthReply::Create()
{
    return new DepthReply();
}

void DepthReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<DepthReply>(classId, &DepthReply::Create);
}

std::string DepthReply::StaticClassName()
{
    return "DepthReply";
}

std::string DepthReply::ClassName() const
{
    return "DepthReply";
}

void DepthReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "depth", depth);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> DepthReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void DepthReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

DepthReply::~DepthReply()
{
}

std::unique_ptr<sngxml::dom::Element> DepthReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(depth, "depth").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int FramesRequest::classId = -1;

FramesRequest::FramesRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), lowFrame(), highFrame(), dataPtr(nullptr)
{
}

void* FramesRequest::Create()
{
    return new FramesRequest();
}

void FramesRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<FramesRequest>(classId, &FramesRequest::Create);
}

std::string FramesRequest::StaticClassName()
{
    return "FramesRequest";
}

std::string FramesRequest::ClassName() const
{
    return "FramesRequest";
}

void FramesRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "lowFrame", lowFrame);
    sngxml::xmlser::FromXml(element, "highFrame", highFrame);
}

std::vector<sngxml::xmlser::XmlPtrBase*> FramesRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void FramesRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

FramesRequest::~FramesRequest()
{
}

std::unique_ptr<sngxml::dom::Element> FramesRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(lowFrame, "lowFrame").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(highFrame, "highFrame").release()));
    return element;
}

int FramesReply::classId = -1;

FramesReply::FramesReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), frames(), error(), dataPtr(nullptr)
{
}

void* FramesReply::Create()
{
    return new FramesReply();
}

void FramesReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<FramesReply>(classId, &FramesReply::Create);
}

std::string FramesReply::StaticClassName()
{
    return "FramesReply";
}

std::string FramesReply::ClassName() const
{
    return "FramesReply";
}

void FramesReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "frames", frames);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> FramesReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void FramesReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

FramesReply::~FramesReply()
{
}

std::unique_ptr<sngxml::dom::Element> FramesReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(frames, "frames").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int EvaluateRequest::classId = -1;

EvaluateRequest::EvaluateRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), expression(), dataPtr(nullptr)
{
}

void* EvaluateRequest::Create()
{
    return new EvaluateRequest();
}

void EvaluateRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<EvaluateRequest>(classId, &EvaluateRequest::Create);
}

std::string EvaluateRequest::StaticClassName()
{
    return "EvaluateRequest";
}

std::string EvaluateRequest::ClassName() const
{
    return "EvaluateRequest";
}

void EvaluateRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "expression", expression);
}

std::vector<sngxml::xmlser::XmlPtrBase*> EvaluateRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void EvaluateRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

EvaluateRequest::~EvaluateRequest()
{
}

std::unique_ptr<sngxml::dom::Element> EvaluateRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expression, "expression").release()));
    return element;
}

int EvaluateReply::classId = -1;

EvaluateReply::EvaluateReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), result(), error(), dataPtr(nullptr)
{
}

void* EvaluateReply::Create()
{
    return new EvaluateReply();
}

void EvaluateReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<EvaluateReply>(classId, &EvaluateReply::Create);
}

std::string EvaluateReply::StaticClassName()
{
    return "EvaluateReply";
}

std::string EvaluateReply::ClassName() const
{
    return "EvaluateReply";
}

void EvaluateReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "result", result);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> EvaluateReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void EvaluateReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

EvaluateReply::~EvaluateReply()
{
}

std::unique_ptr<sngxml::dom::Element> EvaluateReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(result, "result").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int CountRequest::classId = -1;

CountRequest::CountRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), expression(), dataPtr(nullptr)
{
}

void* CountRequest::Create()
{
    return new CountRequest();
}

void CountRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<CountRequest>(classId, &CountRequest::Create);
}

std::string CountRequest::StaticClassName()
{
    return "CountRequest";
}

std::string CountRequest::ClassName() const
{
    return "CountRequest";
}

void CountRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "expression", expression);
}

std::vector<sngxml::xmlser::XmlPtrBase*> CountRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void CountRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

CountRequest::~CountRequest()
{
}

std::unique_ptr<sngxml::dom::Element> CountRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expression, "expression").release()));
    return element;
}

int CountReply::classId = -1;

CountReply::CountReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), count(), error(), dataPtr(nullptr)
{
}

void* CountReply::Create()
{
    return new CountReply();
}

void CountReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<CountReply>(classId, &CountReply::Create);
}

std::string CountReply::StaticClassName()
{
    return "CountReply";
}

std::string CountReply::ClassName() const
{
    return "CountReply";
}

void CountReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "count", count);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> CountReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void CountReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

CountReply::~CountReply()
{
}

std::unique_ptr<sngxml::dom::Element> CountReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(count, "count").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int EvaluateChildRequest::classId = -1;

EvaluateChildRequest::EvaluateChildRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), expression(), start(), count(), dataPtr(nullptr)
{
}

void* EvaluateChildRequest::Create()
{
    return new EvaluateChildRequest();
}

void EvaluateChildRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<EvaluateChildRequest>(classId, &EvaluateChildRequest::Create);
}

std::string EvaluateChildRequest::StaticClassName()
{
    return "EvaluateChildRequest";
}

std::string EvaluateChildRequest::ClassName() const
{
    return "EvaluateChildRequest";
}

void EvaluateChildRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "expression", expression);
    sngxml::xmlser::FromXml(element, "start", start);
    sngxml::xmlser::FromXml(element, "count", count);
}

std::vector<sngxml::xmlser::XmlPtrBase*> EvaluateChildRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void EvaluateChildRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

EvaluateChildRequest::~EvaluateChildRequest()
{
}

std::unique_ptr<sngxml::dom::Element> EvaluateChildRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expression, "expression").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(start, "start").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(count, "count").release()));
    return element;
}

int EvaluateChildReply::classId = -1;

EvaluateChildReply::EvaluateChildReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), success(), results(), error(), dataPtr(nullptr)
{
}

void* EvaluateChildReply::Create()
{
    return new EvaluateChildReply();
}

void EvaluateChildReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<EvaluateChildReply>(classId, &EvaluateChildReply::Create);
}

std::string EvaluateChildReply::StaticClassName()
{
    return "EvaluateChildReply";
}

std::string EvaluateChildReply::ClassName() const
{
    return "EvaluateChildReply";
}

void EvaluateChildReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "success", success);
    sngxml::xmlser::FromXml(element, "results", results);
    sngxml::xmlser::FromXml(element, "error", error);
}

std::vector<sngxml::xmlser::XmlPtrBase*> EvaluateChildReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void EvaluateChildReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

EvaluateChildReply::~EvaluateChildReply()
{
}

std::unique_ptr<sngxml::dom::Element> EvaluateChildReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(success, "success").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(results, "results").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(error, "error").release()));
    return element;
}

int TargetRunningRequest::classId = -1;

TargetRunningRequest::TargetRunningRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* TargetRunningRequest::Create()
{
    return new TargetRunningRequest();
}

void TargetRunningRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<TargetRunningRequest>(classId, &TargetRunningRequest::Create);
}

std::string TargetRunningRequest::StaticClassName()
{
    return "TargetRunningRequest";
}

std::string TargetRunningRequest::ClassName() const
{
    return "TargetRunningRequest";
}

void TargetRunningRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> TargetRunningRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void TargetRunningRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

TargetRunningRequest::~TargetRunningRequest()
{
}

std::unique_ptr<sngxml::dom::Element> TargetRunningRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int TargetRunningReply::classId = -1;

TargetRunningReply::TargetRunningReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* TargetRunningReply::Create()
{
    return new TargetRunningReply();
}

void TargetRunningReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<TargetRunningReply>(classId, &TargetRunningReply::Create);
}

std::string TargetRunningReply::StaticClassName()
{
    return "TargetRunningReply";
}

std::string TargetRunningReply::ClassName() const
{
    return "TargetRunningReply";
}

void TargetRunningReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> TargetRunningReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void TargetRunningReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

TargetRunningReply::~TargetRunningReply()
{
}

std::unique_ptr<sngxml::dom::Element> TargetRunningReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int TargetInputRequest::classId = -1;

TargetInputRequest::TargetInputRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* TargetInputRequest::Create()
{
    return new TargetInputRequest();
}

void TargetInputRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<TargetInputRequest>(classId, &TargetInputRequest::Create);
}

std::string TargetInputRequest::StaticClassName()
{
    return "TargetInputRequest";
}

std::string TargetInputRequest::ClassName() const
{
    return "TargetInputRequest";
}

void TargetInputRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> TargetInputRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void TargetInputRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

TargetInputRequest::~TargetInputRequest()
{
}

std::unique_ptr<sngxml::dom::Element> TargetInputRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int TargetInputReply::classId = -1;

TargetInputReply::TargetInputReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), line(), eof(), dataPtr(nullptr)
{
}

void* TargetInputReply::Create()
{
    return new TargetInputReply();
}

void TargetInputReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<TargetInputReply>(classId, &TargetInputReply::Create);
}

std::string TargetInputReply::StaticClassName()
{
    return "TargetInputReply";
}

std::string TargetInputReply::ClassName() const
{
    return "TargetInputReply";
}

void TargetInputReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "eof", eof);
}

std::vector<sngxml::xmlser::XmlPtrBase*> TargetInputReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void TargetInputReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

TargetInputReply::~TargetInputReply()
{
}

std::unique_ptr<sngxml::dom::Element> TargetInputReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(eof, "eof").release()));
    return element;
}

int TargetOutputRequest::classId = -1;

TargetOutputRequest::TargetOutputRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), handle(), output(), dataPtr(nullptr)
{
}

void* TargetOutputRequest::Create()
{
    return new TargetOutputRequest();
}

void TargetOutputRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<TargetOutputRequest>(classId, &TargetOutputRequest::Create);
}

std::string TargetOutputRequest::StaticClassName()
{
    return "TargetOutputRequest";
}

std::string TargetOutputRequest::ClassName() const
{
    return "TargetOutputRequest";
}

void TargetOutputRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "handle", handle);
    sngxml::xmlser::FromXml(element, "output", output);
}

std::vector<sngxml::xmlser::XmlPtrBase*> TargetOutputRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void TargetOutputRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

TargetOutputRequest::~TargetOutputRequest()
{
}

std::unique_ptr<sngxml::dom::Element> TargetOutputRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(handle, "handle").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(output, "output").release()));
    return element;
}

int TargetOutputReply::classId = -1;

TargetOutputReply::TargetOutputReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* TargetOutputReply::Create()
{
    return new TargetOutputReply();
}

void TargetOutputReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<TargetOutputReply>(classId, &TargetOutputReply::Create);
}

std::string TargetOutputReply::StaticClassName()
{
    return "TargetOutputReply";
}

std::string TargetOutputReply::ClassName() const
{
    return "TargetOutputReply";
}

void TargetOutputReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> TargetOutputReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void TargetOutputReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

TargetOutputReply::~TargetOutputReply()
{
}

std::unique_ptr<sngxml::dom::Element> TargetOutputReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int LogDebugMessageRequest::classId = -1;

LogDebugMessageRequest::LogDebugMessageRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), logMessage(), dataPtr(nullptr)
{
}

void* LogDebugMessageRequest::Create()
{
    return new LogDebugMessageRequest();
}

void LogDebugMessageRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<LogDebugMessageRequest>(classId, &LogDebugMessageRequest::Create);
}

std::string LogDebugMessageRequest::StaticClassName()
{
    return "LogDebugMessageRequest";
}

std::string LogDebugMessageRequest::ClassName() const
{
    return "LogDebugMessageRequest";
}

void LogDebugMessageRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "logMessage", logMessage);
}

std::vector<sngxml::xmlser::XmlPtrBase*> LogDebugMessageRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void LogDebugMessageRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

LogDebugMessageRequest::~LogDebugMessageRequest()
{
}

std::unique_ptr<sngxml::dom::Element> LogDebugMessageRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(logMessage, "logMessage").release()));
    return element;
}

int LogDebugMessageReply::classId = -1;

LogDebugMessageReply::LogDebugMessageReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* LogDebugMessageReply::Create()
{
    return new LogDebugMessageReply();
}

void LogDebugMessageReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<LogDebugMessageReply>(classId, &LogDebugMessageReply::Create);
}

std::string LogDebugMessageReply::StaticClassName()
{
    return "LogDebugMessageReply";
}

std::string LogDebugMessageReply::ClassName() const
{
    return "LogDebugMessageReply";
}

void LogDebugMessageReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> LogDebugMessageReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void LogDebugMessageReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

LogDebugMessageReply::~LogDebugMessageReply()
{
}

std::unique_ptr<sngxml::dom::Element> LogDebugMessageReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int KillRequest::classId = -1;

KillRequest::KillRequest()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* KillRequest::Create()
{
    return new KillRequest();
}

void KillRequest::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<KillRequest>(classId, &KillRequest::Create);
}

std::string KillRequest::StaticClassName()
{
    return "KillRequest";
}

std::string KillRequest::ClassName() const
{
    return "KillRequest";
}

void KillRequest::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> KillRequest::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void KillRequest::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

KillRequest::~KillRequest()
{
}

std::unique_ptr<sngxml::dom::Element> KillRequest::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

int KillReply::classId = -1;

KillReply::KillReply()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), dataPtr(nullptr)
{
}

void* KillReply::Create()
{
    return new KillReply();
}

void KillReply::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<KillReply>(classId, &KillReply::Create);
}

std::string KillReply::StaticClassName()
{
    return "KillReply";
}

std::string KillReply::ClassName() const
{
    return "KillReply";
}

void KillReply::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
}

std::vector<sngxml::xmlser::XmlPtrBase*> KillReply::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void KillReply::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

KillReply::~KillReply()
{
}

std::unique_ptr<sngxml::dom::Element> KillReply::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    return element;
}

