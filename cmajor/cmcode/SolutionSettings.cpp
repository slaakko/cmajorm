#include "SolutionSettings.hpp"
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/serialization/XmlImport.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::unicode;

SolutionBreakpoint::SolutionBreakpoint()
    : file(), line(), condition(), disabled()
{
}

SolutionBreakpoint::SolutionBreakpoint(sngxml::dom::Element* element)
    : file(), line(), condition(), disabled()
{
    sngxml::xmlser::FromXml(element, "file", file);
    sngxml::xmlser::FromXml(element, "line", line);
    sngxml::xmlser::FromXml(element, "condition", condition);
    sngxml::xmlser::FromXml(element, "disabled", disabled);
}

std::unique_ptr<sngxml::dom::Element> SolutionBreakpoint::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(file, "file").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(line, "line").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(condition, "condition").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(disabled, "disabled").release()));
    return element;
}

SolutionSettings::SolutionSettings()
    : callStackOpen(), localsViewOpen(), openFiles(), currentOpenFile(), currentCursorLine(), breakpoints(), expandedProjects()
{
}

SolutionSettings::SolutionSettings(sngxml::dom::Element* element)
    : callStackOpen(), localsViewOpen(), openFiles(), currentOpenFile(), currentCursorLine(), breakpoints(), expandedProjects()
{
    sngxml::xmlser::FromXml(element, "callStackOpen", callStackOpen);
    sngxml::xmlser::FromXml(element, "localsViewOpen", localsViewOpen);
    sngxml::xmlser::FromXml(element, "openFiles", openFiles);
    sngxml::xmlser::FromXml(element, "currentOpenFile", currentOpenFile);
    sngxml::xmlser::FromXml(element, "currentCursorLine", currentCursorLine);
    sngxml::xmlser::FromXml(element, "breakpoints", breakpoints);
    sngxml::xmlser::FromXml(element, "expandedProjects", expandedProjects);
}

std::unique_ptr<sngxml::dom::Element> SolutionSettings::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(callStackOpen, "callStackOpen").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(localsViewOpen, "localsViewOpen").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(openFiles, "openFiles").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(currentOpenFile, "currentOpenFile").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(currentCursorLine, "currentCursorLine").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpoints, "breakpoints").release()));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(expandedProjects, "expandedProjects").release()));
    return element;
}

