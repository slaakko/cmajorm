// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Config.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>
#include <fstream>
#include <stdexcept>

using namespace soulng::util;
using namespace soulng::unicode;

namespace {

std::string CmajorConfigDir()
{
    return Path::Combine(CmajorRoot(), "config");
}

} // namespace

namespace cmsx::machine {

class Configuration
{
public:
    static void Init();
    static void Done();
    static Configuration& Instance() { return *instance; }
    std::string FilePath() const { return filePath; }
    int NumProcessors() const { return numProcessors; }
    int NumLocalRegs() const { return numLocalRegs; }
    int ClockTickMilliseconds() const { return clockTickMilliseconds; }
    int MaxProcs() const { return maxProcs; }
    int MaxOpenFiles() const { return maxOpenFiles; }
    int MaxFiles() const { return maxFiles; }
    int MaxFilesystems() const { return maxFilesystems; }
    int MaxBlocks() const { return maxBlocks; }
private:
    Configuration();
    void Read();
    void Write();
    std::string filePath;
    int numProcessors;
    int numLocalRegs;
    int clockTickMilliseconds;
    int maxProcs;
    int maxOpenFiles;
    int maxFiles;
    int maxFilesystems;
    int maxBlocks;
    static std::unique_ptr<Configuration> instance;
};

std::unique_ptr<Configuration> Configuration::instance;

void Configuration::Init()
{
    instance.reset(new Configuration());
}

void Configuration::Done()
{
    instance.reset();
}

Configuration::Configuration() : 
    filePath(Path::Combine(CmajorConfigDir(), "sx.machine.config.xml")), 
    numProcessors(1), 
    numLocalRegs(8),
    clockTickMilliseconds(250),
    maxProcs(1024), 
    maxOpenFiles(256),
    maxFiles(65536),
    maxFilesystems(64),
    maxBlocks(256)
{
    if (!boost::filesystem::exists(filePath))
    {
        Write();
    }
    Read();
}

void Configuration::Read()
{
    std::unique_ptr<sngxml::dom::Document> configDoc = sngxml::dom::ReadDocument(filePath);
    std::unique_ptr<sngxml::xpath::XPathObject> configurationObject = sngxml::xpath::Evaluate(U"configuration", configDoc.get());
    if (configurationObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(configurationObject.get());
        int n = nodeSet->Length();
        if (n == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* configurationElement = static_cast<sngxml::dom::Element*>(node);
                std::u32string numProcessorsAttribute = configurationElement->GetAttribute(U"numProcessors");
                if (numProcessorsAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'numProcessors' attribute not found");
                }
                numProcessors = boost::lexical_cast<int>(ToUtf8(numProcessorsAttribute));
                std::u32string numLocalRegsAttribute = configurationElement->GetAttribute(U"numLocalRegs");
                if (numLocalRegsAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'numLocalRegs' attribute not found");
                }
                numLocalRegs = boost::lexical_cast<int>(ToUtf8(numLocalRegsAttribute));
                std::u32string clockTickMillisecondsAttribute = configurationElement->GetAttribute(U"clockTickMilliseconds");
                if (clockTickMillisecondsAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'clockTickMilliseconds' attribute not found");
                }
                clockTickMilliseconds = boost::lexical_cast<int>(ToUtf8(clockTickMillisecondsAttribute));
                std::u32string maxProcsAttribute = configurationElement->GetAttribute(U"maxProcs");
                if (maxProcsAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'maxProcs' attribute not found");
                }
                maxProcs = boost::lexical_cast<int>(ToUtf8(maxProcsAttribute));
                std::u32string maxOpenFilesAttribute = configurationElement->GetAttribute(U"maxOpenFiles");
                if (maxOpenFilesAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'maxOpenFiles' attribute not found");
                }
                maxOpenFiles = boost::lexical_cast<int>(ToUtf8(maxOpenFilesAttribute));
                std::u32string maxFilesAttribute = configurationElement->GetAttribute(U"maxFiles");
                if (maxFilesAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'maxFiles' attribute not found");
                }
                maxFiles = boost::lexical_cast<int>(ToUtf8(maxFilesAttribute));
                std::u32string maxFilesystemsAttribute = configurationElement->GetAttribute(U"maxFilesystems");
                if (maxFilesystemsAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'maxFilesystems' attribute not found");
                }
                maxFilesystems = boost::lexical_cast<int>(ToUtf8(maxFilesystemsAttribute));
                std::u32string maxBlocksAttribute = configurationElement->GetAttribute(U"maxBlocks");
                if (maxBlocksAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'maxBlocks' attribute not found");
                }
                maxBlocks = boost::lexical_cast<int>(ToUtf8(maxBlocksAttribute));
            }
        }
        else 
        {
            throw std::runtime_error("error reading configuration from '" + filePath + "': single 'configuration' element expected");
        }
    }
}

void Configuration::Write()
{
    std::ofstream file(filePath);
    sngxml::dom::Document configDoc;
    sngxml::dom::Element* configurationElement = new sngxml::dom::Element(U"configuration");
    configurationElement->SetAttribute(U"numProcessors", ToUtf32(std::to_string(numProcessors)));
    configurationElement->SetAttribute(U"numLocalRegs", ToUtf32(std::to_string(numLocalRegs)));
    configurationElement->SetAttribute(U"clockTickMilliseconds", ToUtf32(std::to_string(clockTickMilliseconds)));
    configurationElement->SetAttribute(U"maxProcs", ToUtf32(std::to_string(maxProcs)));
    configurationElement->SetAttribute(U"maxOpenFiles", ToUtf32(std::to_string(maxOpenFiles)));
    configurationElement->SetAttribute(U"maxFiles", ToUtf32(std::to_string(maxFiles)));
    configurationElement->SetAttribute(U"maxFilesystems", ToUtf32(std::to_string(maxFilesystems)));
    configurationElement->SetAttribute(U"maxBlocks", ToUtf32(std::to_string(maxBlocks)));
    configDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(configurationElement));
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    configDoc.Write(formatter);
}

int NumProcessors()
{
    return Configuration::Instance().NumProcessors();
}

int NumLocalRegs()
{
    return Configuration::Instance().NumLocalRegs();
}

int ClockTickMilliseconds()
{
    return Configuration::Instance().ClockTickMilliseconds();
}

int MaxProcs()
{
    return Configuration::Instance().MaxProcs();
}

int MaxOpenFiles()
{
    return Configuration::Instance().MaxOpenFiles();
}

int MaxFiles()
{
    return Configuration::Instance().MaxFiles();
}

int MaxFilesystems()
{
    return Configuration::Instance().MaxFilesystems();
}

int MaxBlocks()
{
    return Configuration::Instance().MaxBlocks();
}

std::string ConfigFilePath()
{
    return Configuration::Instance().FilePath();
}

void InitConfig()
{
    Configuration::Init();
}

void DoneConfig()
{
    Configuration::Done();
}

} // cmsx::machine
