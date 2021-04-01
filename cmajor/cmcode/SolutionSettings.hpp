#ifndef SOLUTIONSETTINGS_HPP_CBFFFB300F3FB8B5357239390EFF01476489416B
#define SOLUTIONSETTINGS_HPP_CBFFFB300F3FB8B5357239390EFF01476489416B
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

class SolutionBreakpoint
{
public:
    SolutionBreakpoint();
    SolutionBreakpoint(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string file;
    int32_t line;
    std::string condition;
    bool disabled;
};

class SolutionSettings
{
public:
    SolutionSettings();
    SolutionSettings(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::vector<SolutionBreakpoint> breakpoints;
};

#endif // SOLUTIONSETTINGS_HPP_CBFFFB300F3FB8B5357239390EFF01476489416B
