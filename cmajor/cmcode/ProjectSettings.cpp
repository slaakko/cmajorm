#include "ProjectSettings.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int ProjectBreakpoint::classId = -1;

ProjectBreakpoint::ProjectBreakpoint()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), file(), line(), condition(), disabled(), dataPtr(nullptr), isOwned(false)
{
}

void* ProjectBreakpoint::Create()
{
    return new ProjectBreakpoint();
}

void ProjectBreakpoint::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ProjectBreakpoint>(classId, &ProjectBreakpoint::Create);
}

std::string ProjectBreakpoint::StaticClassName()
{
    return "ProjectBreakpoint";
}

std::string ProjectBreakpoint::ClassName() const
{
    return "ProjectBreakpoint";
}

void ProjectBreakpoint::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "condition", condition);
    sngxml::xmlser::FromXml(element, "disabled", disabled);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ProjectBreakpoint::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ProjectBreakpoint::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ProjectBreakpoint::~ProjectBreakpoint()
{
}

std::unique_ptr<sngxml::dom::Element> ProjectBreakpoint::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> ProjectBreakpoint::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))
    {
        SetObjectXmlAttributes(element.get());
    }
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(condition, "condition", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(disabled, "disabled", ctx).release()));
    return element;
}

int ProjectSettings::classId = -1;

ProjectSettings::ProjectSettings()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), programArguments(), breakpoints(), dataPtr(nullptr), isOwned(false)
{
}

void* ProjectSettings::Create()
{
    return new ProjectSettings();
}

void ProjectSettings::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<ProjectSettings>(classId, &ProjectSettings::Create);
}

std::string ProjectSettings::StaticClassName()
{
    return "ProjectSettings";
}

std::string ProjectSettings::ClassName() const
{
    return "ProjectSettings";
}

void ProjectSettings::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "programArguments", programArguments);
    sngxml::xmlser::FromXml(element, "breakpoints", breakpoints);
}

std::vector<sngxml::xmlser::XmlPtrBase*> ProjectSettings::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void ProjectSettings::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

ProjectSettings::~ProjectSettings()
{
}

std::unique_ptr<sngxml::dom::Element> ProjectSettings::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> ProjectSettings::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))
    {
        SetObjectXmlAttributes(element.get());
    }
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(programArguments, "programArguments", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpoints, "breakpoints", ctx).release()));
    return element;
}

