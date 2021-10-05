#include "Configuration.hpp"
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

int RecentSolution::classId = -1;

RecentSolution::RecentSolution()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), name(), filePath(), dataPtr(nullptr), isOwned(false)
{
}

void* RecentSolution::Create()
{
    return new RecentSolution();
}

void RecentSolution::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<RecentSolution>(classId, &RecentSolution::Create);
}

std::string RecentSolution::StaticClassName()
{
    return "RecentSolution";
}

std::string RecentSolution::ClassName() const
{
    return "RecentSolution";
}

void RecentSolution::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "name", name);
    sngxml::xmlser::FromXml(element, "filePath", filePath);
}

std::vector<sngxml::xmlser::XmlPtrBase*> RecentSolution::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void RecentSolution::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

RecentSolution::~RecentSolution()
{
}

std::unique_ptr<sngxml::dom::Element> RecentSolution::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(name, "name").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(filePath, "filePath").release()));
    return element;
}

int BuildSettings::classId = -1;

BuildSettings::BuildSettings()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), numberOfProjectBuildThreads(), singleThreadedCompile(), generateIntermediateCodeFiles(), generateOptimizedIntermediateCodeFiles(), dataPtr(nullptr), isOwned(false)
{
}

void* BuildSettings::Create()
{
    return new BuildSettings();
}

void BuildSettings::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<BuildSettings>(classId, &BuildSettings::Create);
}

std::string BuildSettings::StaticClassName()
{
    return "BuildSettings";
}

std::string BuildSettings::ClassName() const
{
    return "BuildSettings";
}

void BuildSettings::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "numberOfProjectBuildThreads", numberOfProjectBuildThreads);
    sngxml::xmlser::FromXml(element, "singleThreadedCompile", singleThreadedCompile);
    sngxml::xmlser::FromXml(element, "generateIntermediateCodeFiles", generateIntermediateCodeFiles);
    sngxml::xmlser::FromXml(element, "generateOptimizedIntermediateCodeFiles", generateOptimizedIntermediateCodeFiles);
}

std::vector<sngxml::xmlser::XmlPtrBase*> BuildSettings::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void BuildSettings::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

BuildSettings::~BuildSettings()
{
}

std::unique_ptr<sngxml::dom::Element> BuildSettings::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(numberOfProjectBuildThreads, "numberOfProjectBuildThreads").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(singleThreadedCompile, "singleThreadedCompile").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(generateIntermediateCodeFiles, "generateIntermediateCodeFiles").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(generateOptimizedIntermediateCodeFiles, "generateOptimizedIntermediateCodeFiles").release()));
    return element;
}

int Options::classId = -1;

Options::Options()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), defined(), codeCompletionDefined(), showStartupDialog(), numberOfRecentSolutions(), codeCompletion(), parsingFrequency(), dataPtr(nullptr), isOwned(false)
{
}

void* Options::Create()
{
    return new Options();
}

void Options::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Options>(classId, &Options::Create);
}

std::string Options::StaticClassName()
{
    return "Options";
}

std::string Options::ClassName() const
{
    return "Options";
}

void Options::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "defined", defined);
    sngxml::xmlser::FromXml(element, "codeCompletionDefined", codeCompletionDefined);
    sngxml::xmlser::FromXml(element, "showStartupDialog", showStartupDialog);
    sngxml::xmlser::FromXml(element, "numberOfRecentSolutions", numberOfRecentSolutions);
    sngxml::xmlser::FromXml(element, "codeCompletion", codeCompletion);
    sngxml::xmlser::FromXml(element, "parsingFrequency", parsingFrequency);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Options::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void Options::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Options::~Options()
{
}

std::unique_ptr<sngxml::dom::Element> Options::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(defined, "defined").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(codeCompletionDefined, "codeCompletionDefined").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(showStartupDialog, "showStartupDialog").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(numberOfRecentSolutions, "numberOfRecentSolutions").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(codeCompletion, "codeCompletion").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(parsingFrequency, "parsingFrequency").release()));
    return element;
}

int WindowLocation::classId = -1;

WindowLocation::WindowLocation()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), x(), y(), dataPtr(nullptr), isOwned(false)
{
}

void* WindowLocation::Create()
{
    return new WindowLocation();
}

void WindowLocation::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<WindowLocation>(classId, &WindowLocation::Create);
}

std::string WindowLocation::StaticClassName()
{
    return "WindowLocation";
}

std::string WindowLocation::ClassName() const
{
    return "WindowLocation";
}

void WindowLocation::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "x", x);
    sngxml::xmlser::FromXml(element, "y", y);
}

std::vector<sngxml::xmlser::XmlPtrBase*> WindowLocation::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void WindowLocation::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

WindowLocation::~WindowLocation()
{
}

std::unique_ptr<sngxml::dom::Element> WindowLocation::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(x, "x").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(y, "y").release()));
    return element;
}

int WindowSize::classId = -1;

WindowSize::WindowSize()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), width(), height(), dataPtr(nullptr), isOwned(false)
{
}

void* WindowSize::Create()
{
    return new WindowSize();
}

void WindowSize::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<WindowSize>(classId, &WindowSize::Create);
}

std::string WindowSize::StaticClassName()
{
    return "WindowSize";
}

std::string WindowSize::ClassName() const
{
    return "WindowSize";
}

void WindowSize::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "width", width);
    sngxml::xmlser::FromXml(element, "height", height);
}

std::vector<sngxml::xmlser::XmlPtrBase*> WindowSize::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void WindowSize::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

WindowSize::~WindowSize()
{
}

std::unique_ptr<sngxml::dom::Element> WindowSize::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(width, "width").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(height, "height").release()));
    return element;
}

int WindowSettings::classId = -1;

WindowSettings::WindowSettings()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), defined(), windowState(), location(), size(), maximizedHorizontalSplitterDistance(), maximizedVerticalSplitterDistance(), normalHorizontalSplitterDistance(), normalVerticalSplitterDistance(), dataPtr(nullptr), isOwned(false)
{
}

void* WindowSettings::Create()
{
    return new WindowSettings();
}

void WindowSettings::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<WindowSettings>(classId, &WindowSettings::Create);
}

std::string WindowSettings::StaticClassName()
{
    return "WindowSettings";
}

std::string WindowSettings::ClassName() const
{
    return "WindowSettings";
}

void WindowSettings::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "defined", defined);
    sngxml::xmlser::FromXml(element, "windowState", windowState);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "size", size);
    sngxml::xmlser::FromXml(element, "maximizedHorizontalSplitterDistance", maximizedHorizontalSplitterDistance);
    sngxml::xmlser::FromXml(element, "maximizedVerticalSplitterDistance", maximizedVerticalSplitterDistance);
    sngxml::xmlser::FromXml(element, "normalHorizontalSplitterDistance", normalHorizontalSplitterDistance);
    sngxml::xmlser::FromXml(element, "normalVerticalSplitterDistance", normalVerticalSplitterDistance);
}

std::vector<sngxml::xmlser::XmlPtrBase*> WindowSettings::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void WindowSettings::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

WindowSettings::~WindowSettings()
{
}

std::unique_ptr<sngxml::dom::Element> WindowSettings::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(defined, "defined").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(windowState, "windowState").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(location, "location").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(size, "size").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(maximizedHorizontalSplitterDistance, "maximizedHorizontalSplitterDistance").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(maximizedVerticalSplitterDistance, "maximizedVerticalSplitterDistance").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(normalHorizontalSplitterDistance, "normalHorizontalSplitterDistance").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(normalVerticalSplitterDistance, "normalVerticalSplitterDistance").release()));
    return element;
}

int Configuration::classId = -1;

Configuration::Configuration()
     : objectId(boost::uuids::nil_uuid()), container(nullptr), recentSolutions(), options(), buildSettings(), windowSettings(), dataPtr(nullptr), isOwned(false)
{
}

void* Configuration::Create()
{
    return new Configuration();
}

void Configuration::Register(int classId_)
{
    classId = classId_;
    sngxml::xmlser::XmlRegister<Configuration>(classId, &Configuration::Create);
}

std::string Configuration::StaticClassName()
{
    return "Configuration";
}

std::string Configuration::ClassName() const
{
    return "Configuration";
}

void Configuration::FromXml(sngxml::dom::Element* element)
{
    std::u32string objectIdAttr = element->GetAttribute(U"objectId");
    if (!objectIdAttr.empty())
    {
        objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));
    }
    sngxml::xmlser::FromXml(element, "recentSolutions", recentSolutions);
    sngxml::xmlser::FromXml(element, "options", options);
    sngxml::xmlser::FromXml(element, "buildSettings", buildSettings);
    sngxml::xmlser::FromXml(element, "windowSettings", windowSettings);
}

std::vector<sngxml::xmlser::XmlPtrBase*> Configuration::GetPtrs() const
{
    std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;
    return ptrs;
}

void Configuration::SetObjectXmlAttributes(sngxml::dom::Element * element) const
{
    element->SetAttribute(U"classId", ToUtf32(std::to_string(classId)));
    element->SetAttribute(U"objectId", ToUtf32(boost::uuids::to_string(ObjectId())));
    element->SetAttribute(U"className", ToUtf32(ClassName()));
}

Configuration::~Configuration()
{
}

std::unique_ptr<sngxml::dom::Element> Configuration::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    SetObjectXmlAttributes(element.get());
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(recentSolutions, "recentSolutions").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(options, "options").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(buildSettings, "buildSettings").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(windowSettings, "windowSettings").release()));
    return element;
}

