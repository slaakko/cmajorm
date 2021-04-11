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

class RecentSolution
{
public:
    RecentSolution();
    RecentSolution(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::string name;
    std::string filePath;
};

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

class Options
{
public:
    Options();
    Options(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool defined;
    bool showStartupDialog;
    int32_t numberOfRecentSolutions;
};

class WindowLocation
{
public:
    WindowLocation();
    WindowLocation(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    int32_t x;
    int32_t y;
};

class WindowSize
{
public:
    WindowSize();
    WindowSize(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    int32_t width;
    int32_t height;
};

class WindowSettings
{
public:
    WindowSettings();
    WindowSettings(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    bool defined;
    int32_t windowState;
    WindowLocation location;
    WindowSize size;
    int32_t maximizedHorizontalSplitterDistance;
    int32_t maximizedVerticalSplitterDistance;
    int32_t normalHorizontalSplitterDistance;
    int32_t normalVerticalSplitterDistance;
};

class Configuration
{
public:
    Configuration();
    Configuration(sngxml::dom::Element* element);
    std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;
public:
    std::vector<RecentSolution> recentSolutions;
    Options options;
    BuildSettings buildSettings;
    WindowSettings windowSettings;
};

#endif // CONFIGURATION_HPP_56000706C340B369A09096983E04F15C5672838E
