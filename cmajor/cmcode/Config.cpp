// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Config.hpp>
#include <cmajor/cmsvc/Config.hpp>
#include <sngxml/dom/Parser.hpp>
#include <boost/filesystem.hpp>
#include <soulng/util/Path.hpp>
#include <sstream>

namespace cmcode {

using namespace cmajor::service;
using namespace soulng::util;

bool UseDebugServers()
{
    //return true;
    return false;
}

bool ServerLogging()
{
    return true;
}

bool DebugWait()
{
    return false;
}

class Config
{
public:
    static void Init();
    static void Done();
    static Config& Instance() { return *instance; }
    Configuration& GetConfiguration() { return configuration; }
    void Load();
    void Save();
private:
    static std::unique_ptr<Config> instance;
    Configuration configuration;
};

std::unique_ptr<Config> Config::instance;

void Config::Init()
{
    instance.reset(new Config());
}

void Config::Done()
{
    instance.reset();
}

void Config::Load()
{
    std::string configXmlFilePath = Path::Combine(CmajorConfigDir(), "cmcode.config.xml");
    if (boost::filesystem::exists(configXmlFilePath))
    {
        std::unique_ptr<sngxml::dom::Document> configDoc = sngxml::dom::ReadDocument(configXmlFilePath);
        configuration = Configuration(configDoc->DocumentElement());
    }
}

void Config::Save()
{
    std::string configXmlFilePath = Path::Combine(CmajorConfigDir(), "cmcode.config.xml");
    std::unique_ptr<sngxml::dom::Element> configurationElement = configuration.ToXml("configuration");
    sngxml::dom::Document configDoc;
    configDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(configurationElement.release()));
    std::ofstream configFile(configXmlFilePath);
    CodeFormatter formatter(configFile);
    configDoc.Write(formatter);
}

void LoadConfiguration()
{
    Config::Instance().Load();
}

void SaveConfiguration()
{
    Config::Instance().Save();
}

const BuildSettings& GetBuildSettings()
{
    return Config::Instance().GetConfiguration().buildSettings;
}

void ConfigInit()
{
    Config::Init();
}

void ConfigDone()
{
    Config::Done();
}

} // namespace cmcode
