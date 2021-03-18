#ifndef CONFIGURATION_HPP_56000706C340B369A09096983E04F15C5672838E
#define CONFIGURATION_HPP_56000706C340B369A09096983E04F15C5672838E
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

class BuildSettings
{
public:
    BuildSettings();
    BuildSettings(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    int32_t numberOfProjectBuildThreads;
    bool singleThreadedCompile;
    bool generateIntermediateCodeFiles;
    bool generateOptimizedIntermediateCodeFiles;
};

class Configuration
{
public:
    Configuration();
    Configuration(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    BuildSettings buildSettings;
};

#endif // CONFIGURATION_HPP_56000706C340B369A09096983E04F15C5672838E
