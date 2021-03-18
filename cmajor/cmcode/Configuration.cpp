#include "Configuration.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

BuildSettings::BuildSettings()
    : numberOfProjectBuildThreads(), singleThreadedCompile(), generateIntermediateCodeFiles(), generateOptimizedIntermediateCodeFiles()
{
}

BuildSettings::BuildSettings(sngxml::dom::Element* element)
    : numberOfProjectBuildThreads(), singleThreadedCompile(), generateIntermediateCodeFiles(), generateOptimizedIntermediateCodeFiles()
{
    sngxml::xmlser::FromXml(element, "numberOfProjectBuildThreads", numberOfProjectBuildThreads);
    sngxml::xmlser::FromXml(element, "singleThreadedCompile", singleThreadedCompile);
    sngxml::xmlser::FromXml(element, "generateIntermediateCodeFiles", generateIntermediateCodeFiles);
    sngxml::xmlser::FromXml(element, "generateOptimizedIntermediateCodeFiles", generateOptimizedIntermediateCodeFiles);
}

std::unique_ptr<sngxml::dom::Element> BuildSettings::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(numberOfProjectBuildThreads, "numberOfProjectBuildThreads").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(singleThreadedCompile, "singleThreadedCompile").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(generateIntermediateCodeFiles, "generateIntermediateCodeFiles").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(generateOptimizedIntermediateCodeFiles, "generateOptimizedIntermediateCodeFiles").release()));
    return element;
}

Configuration::Configuration()
    : buildSettings()
{
}

Configuration::Configuration(sngxml::dom::Element* element)
    : buildSettings()
{
    sngxml::xmlser::FromXml(element, "buildSettings", buildSettings);
}

std::unique_ptr<sngxml::dom::Element> Configuration::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(buildSettings, "buildSettings").release()));
    return element;
}

