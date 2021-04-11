#include "Configuration.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

RecentSolution::RecentSolution()
    : name(), filePath()
{
}

RecentSolution::RecentSolution(sngxml::dom::Element* element)
    : name(), filePath()
{
    sngxml::xmlser::FromXml(element, "name", name);
    sngxml::xmlser::FromXml(element, "filePath", filePath);
}

std::unique_ptr<sngxml::dom::Element> RecentSolution::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(name, "name").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(filePath, "filePath").release()));
    return element;
}

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

Options::Options()
    : defined(), showStartupDialog(), numberOfRecentSolutions()
{
}

Options::Options(sngxml::dom::Element* element)
    : defined(), showStartupDialog(), numberOfRecentSolutions()
{
    sngxml::xmlser::FromXml(element, "defined", defined);
    sngxml::xmlser::FromXml(element, "showStartupDialog", showStartupDialog);
    sngxml::xmlser::FromXml(element, "numberOfRecentSolutions", numberOfRecentSolutions);
}

std::unique_ptr<sngxml::dom::Element> Options::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(defined, "defined").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(showStartupDialog, "showStartupDialog").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(numberOfRecentSolutions, "numberOfRecentSolutions").release()));
    return element;
}

WindowLocation::WindowLocation()
    : x(), y()
{
}

WindowLocation::WindowLocation(sngxml::dom::Element* element)
    : x(), y()
{
    sngxml::xmlser::FromXml(element, "x", x);
    sngxml::xmlser::FromXml(element, "y", y);
}

std::unique_ptr<sngxml::dom::Element> WindowLocation::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(x, "x").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(y, "y").release()));
    return element;
}

WindowSize::WindowSize()
    : width(), height()
{
}

WindowSize::WindowSize(sngxml::dom::Element* element)
    : width(), height()
{
    sngxml::xmlser::FromXml(element, "width", width);
    sngxml::xmlser::FromXml(element, "height", height);
}

std::unique_ptr<sngxml::dom::Element> WindowSize::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(width, "width").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(height, "height").release()));
    return element;
}

WindowSettings::WindowSettings()
    : defined(), windowState(), location(), size(), maximizedHorizontalSplitterDistance(), maximizedVerticalSplitterDistance(), normalHorizontalSplitterDistance(), normalVerticalSplitterDistance()
{
}

WindowSettings::WindowSettings(sngxml::dom::Element* element)
    : defined(), windowState(), location(), size(), maximizedHorizontalSplitterDistance(), maximizedVerticalSplitterDistance(), normalHorizontalSplitterDistance(), normalVerticalSplitterDistance()
{
    sngxml::xmlser::FromXml(element, "defined", defined);
    sngxml::xmlser::FromXml(element, "windowState", windowState);
    sngxml::xmlser::FromXml(element, "location", location);
    sngxml::xmlser::FromXml(element, "size", size);
    sngxml::xmlser::FromXml(element, "maximizedHorizontalSplitterDistance", maximizedHorizontalSplitterDistance);
    sngxml::xmlser::FromXml(element, "maximizedVerticalSplitterDistance", maximizedVerticalSplitterDistance);
    sngxml::xmlser::FromXml(element, "normalHorizontalSplitterDistance", normalHorizontalSplitterDistance);
    sngxml::xmlser::FromXml(element, "normalVerticalSplitterDistance", normalVerticalSplitterDistance);
}

std::unique_ptr<sngxml::dom::Element> WindowSettings::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
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

Configuration::Configuration()
    : recentSolutions(), options(), buildSettings(), windowSettings()
{
}

Configuration::Configuration(sngxml::dom::Element* element)
    : recentSolutions(), options(), buildSettings(), windowSettings()
{
    sngxml::xmlser::FromXml(element, "recentSolutions", recentSolutions);
    sngxml::xmlser::FromXml(element, "options", options);
    sngxml::xmlser::FromXml(element, "buildSettings", buildSettings);
    sngxml::xmlser::FromXml(element, "windowSettings", windowSettings);
}

std::unique_ptr<sngxml::dom::Element> Configuration::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(recentSolutions, "recentSolutions").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(options, "options").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(buildSettings, "buildSettings").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(windowSettings, "windowSettings").release()));
    return element;
}

