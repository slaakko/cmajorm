#include "SolutionSettings.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int SolutionBreakpoint::classId = -1;

SolutionBreakpoint::SolutionBreakpoint()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), file(), line(), condition(), disabled(), dataPtr(nullptr), isOwned(false)
{
}

void* SolutionBreakpoint::Create()
{
    return new SolutionBreakpoint();
}

void SolutionBreakpoint::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<SolutionBreakpoint>(classId, &SolutionBreakpoint::Create);
}

std::string SolutionBreakpoint::StaticClassName()
{
    return "SolutionBreakpoint";
}

std::string SolutionBreakpoint::ClassName() const
{
    return "SolutionBreakpoint";
}

void SolutionBreakpoint::FromXml(sngxml::dom::Element* element)
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

std::vector<sngxml::xmlser::XmlPtrBase*> SolutionBreakpoint::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void SolutionBreakpoint::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

SolutionBreakpoint::~SolutionBreakpoint()
{
}

std::unique_ptr<sngxml::dom::Element> SolutionBreakpoint::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> SolutionBreakpoint::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
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

int SolutionSettings::classId = -1;

SolutionSettings::SolutionSettings()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), callStackOpen(), localsViewOpen(), openFiles(), currentOpenFile(), currentCursorLine(), breakpoints(), expandedProjects(), dataPtr(nullptr), isOwned(false)
{
}

void* SolutionSettings::Create()
{
    return new SolutionSettings();
}

void SolutionSettings::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<SolutionSettings>(classId, &SolutionSettings::Create);
}

std::string SolutionSettings::StaticClassName()
{
    return "SolutionSettings";
}

std::string SolutionSettings::ClassName() const
{
    return "SolutionSettings";
}

void SolutionSettings::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "callStackOpen", callStackOpen);
    sngxml::xmlser::FromXml(element, "localsViewOpen", localsViewOpen);
    sngxml::xmlser::FromXml(element, "openFiles", openFiles);
    sngxml::xmlser::FromXml(element, "currentOpenFile", currentOpenFile);
    sngxml::xmlser::FromXml(element, "currentCursorLine", currentCursorLine);
    sngxml::xmlser::FromXml(element, "breakpoints", breakpoints);
    sngxml::xmlser::FromXml(element, "expandedProjects", expandedProjects);
}

std::vector<sngxml::xmlser::XmlPtrBase*> SolutionSettings::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void SolutionSettings::SetObjectXmlAttributes(sngxml::dom::Element* element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

SolutionSettings::~SolutionSettings()
{
}

std::unique_ptr<sngxml::dom::Element> SolutionSettings::ToXml(const std::string& fieldName) const
{
    sngxml::xmlser::XmlSerializationContext ctx;
    return ToXml(fieldName, ctx);
}

std::unique_ptr<sngxml::dom::Element> SolutionSettings::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))
    {
        SetObjectXmlAttributes(element.get());
    }
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(callStackOpen, "callStackOpen", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(localsViewOpen, "localsViewOpen", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(openFiles, "openFiles", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(currentOpenFile, "currentOpenFile", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(currentCursorLine, "currentCursorLine", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpoints, "breakpoints", ctx).release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expandedProjects, "expandedProjects", ctx).release()));
    return element;
}

