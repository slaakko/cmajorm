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
    : breakpoints()
{
}

SolutionSettings::SolutionSettings(sngxml::dom::Element* element)
    : breakpoints()
{
    sngxml::xmlser::FromXml(element, "breakpoints", breakpoints);
}

std::unique_ptr<sngxml::dom::Element> SolutionSettings::ToXml(const std::string& fieldName) const
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));
    element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(breakpoints, "breakpoints").release()));
    return element;
}

