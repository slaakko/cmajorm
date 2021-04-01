#ifndef PROJECTSETTINGS_HPP_617930ED94CBC650CEED43069367A84D6C2F8F6A
#define PROJECTSETTINGS_HPP_617930ED94CBC650CEED43069367A84D6C2F8F6A
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Time.hpp>
#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <stdint.h>

using date = soulng::util::Date;
using datetime = soulng::util::DateTime;
using timestamp = soulng::util::Timestamp;
using time_point = std::chrono::steady_clock::time_point;
using duration = std::chrono::steady_clock::duration;
using uuid = boost::uuids::uuid;

class ProjectBreakpoint
{
public:
    ProjectBreakpoint();
    ProjectBreakpoint(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string file;
    int32_t line;
    std::string condition;
    bool disabled;
};

class ProjectSettings
{
public:
    ProjectSettings();
    ProjectSettings(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string programArguments;
    std::vector<ProjectBreakpoint> breakpoints;
};

#endif // PROJECTSETTINGS_HPP_617930ED94CBC650CEED43069367A84D6C2F8F6A
