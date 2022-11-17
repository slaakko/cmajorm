// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Config.hpp>
#include <cmajor/cmsvc/Config.hpp>
#include <sngxml/dom/Parser.hpp>
#include <boost/filesystem.hpp>
#include <soulng/util/Path.hpp>
#include <sstream>
#include <fstream>

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
    //return true;
}

bool ServerVerbose()
{
    return true;
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
        Configuration cfg;
        cfg.FromXml(configDoc->DocumentElement());
        configuration = Configuration(cfg);
        if (!configuration.options.defined)
        {
            configuration.options.defined = true;
            configuration.options.showStartupDialog = true;
            configuration.options.numberOfRecentSolutions = 5;
            configuration.options.codeCompletion = true;
            configuration.options.parsingFrequency = 1000;
        }
        if (!configuration.options.codeCompletionDefined)
        {
            configuration.options.codeCompletion = true;
            configuration.options.parsingFrequency = 1000;
        }
        if (!configuration.options.themeDefined)
        {
            configuration.options.theme = "light";
        }
    }
}

void Config::Save()
{
    std::string configXmlFilePath = Path::Combine(CmajorConfigDir(), "cmcode.config.xml");
    configuration.options.defined = true;
    configuration.options.codeCompletionDefined = true;
    configuration.options.themeDefined = true;
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

const Options& GetOptions()
{
    if (!Config::Instance().GetConfiguration().options.defined)
    {
        Config::Instance().GetConfiguration().options.defined = true;
        Config::Instance().GetConfiguration().options.showStartupDialog = true;
        Config::Instance().GetConfiguration().options.numberOfRecentSolutions = 5;
        Config::Instance().GetConfiguration().options.codeCompletion = true;
        Config::Instance().GetConfiguration().options.parsingFrequency = 1000;
    }
    if (!Config::Instance().GetConfiguration().options.codeCompletionDefined)
    {
        Config::Instance().GetConfiguration().options.codeCompletionDefined = true;
        Config::Instance().GetConfiguration().options.codeCompletion = true;
        Config::Instance().GetConfiguration().options.parsingFrequency = 1000;
    }
    if (!Config::Instance().GetConfiguration().options.themeDefined)
    {
        Config::Instance().GetConfiguration().options.themeDefined = true;
        Config::Instance().GetConfiguration().options.theme = "light";
    }
    return Config::Instance().GetConfiguration().options;
}

void SetOptions(const Options& options)
{
    Config::Instance().GetConfiguration().options = options;
}

const BuildSettings& GetBuildSettings()
{
    return Config::Instance().GetConfiguration().buildSettings;
}

void SetBuildSettings(const BuildSettings& buildSettings)
{
    Config::Instance().GetConfiguration().buildSettings = buildSettings;
}

void ConfigInit()
{
    Config::Init();
}

void ConfigDone()
{
    Config::Done();
}

WindowSettings& GetWindowSettings()
{
    return Config::Instance().GetConfiguration().windowSettings;
}

const std::vector<RecentSolution>& GetRecentSolutions() 
{
    std::vector<std::string> removeFilePaths;
    const std::vector<RecentSolution>& recentSolutions = Config::Instance().GetConfiguration().recentSolutions;
    for (const RecentSolution& recentSolution : recentSolutions)
    {
        if (!boost::filesystem::exists(recentSolution.filePath))
        {
            removeFilePaths.push_back(recentSolution.filePath);
        }
    }
    for (const std::string& removeFilePath : removeFilePaths)
    {
        RemoveRecentSolution(removeFilePath);
    }
    return Config::Instance().GetConfiguration().recentSolutions;
}

void AddRecentSolution(const std::string& solutionName, const std::string& solutionFilePath)
{
    std::string filePath = GetFullPath(solutionFilePath);
    RemoveRecentSolution(filePath);
    const Options& options = GetOptions();
    std::vector<RecentSolution>& recentSolutions = Config::Instance().GetConfiguration().recentSolutions;
    while (recentSolutions.size() >= options.numberOfRecentSolutions)
    {
        recentSolutions.pop_back();
    }
    RecentSolution recentSolution;
    recentSolution.name = solutionName;
    recentSolution.filePath = filePath;
    recentSolutions.insert(recentSolutions.begin(), recentSolution);
}

void RemoveRecentSolution(const std::string& solutionFilePath)
{
    std::vector<RecentSolution>& recentSolutions = Config::Instance().GetConfiguration().recentSolutions;
    std::string filePath = GetFullPath(solutionFilePath);
    recentSolutions.erase(std::remove_if(recentSolutions.begin(), recentSolutions.end(), [&](const RecentSolution& solution) { return solution.filePath == filePath; }), recentSolutions.end());
}

} // namespace cmcode
