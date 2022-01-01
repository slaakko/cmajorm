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
    int MaxProcs() const { return maxProcs; }
private:
    Configuration();
    void Read();
    void Write();
    std::string filePath;
    int maxProcs;
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

Configuration::Configuration() : filePath(Path::Combine(CmajorConfigDir(), "sx.machine.config.xml")), maxProcs(1024)
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
                std::u32string maxProcsAttribute = configurationElement->GetAttribute(U"maxProcs");
                if (maxProcsAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'maxProcs' attribute not found");
                }
                maxProcs = boost::lexical_cast<int>(ToUtf8(maxProcsAttribute));
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
    configurationElement->SetAttribute(U"maxProcs", ToUtf32(std::to_string(maxProcs)));
    configDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(configurationElement));
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    configDoc.Write(formatter);
}

int MaxProcs()
{
    return Configuration::Instance().MaxProcs();
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
