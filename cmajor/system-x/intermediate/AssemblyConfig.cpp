// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/AssemblyConfig.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;
using namespace soulng::unicode;

namespace {

std::string CmajorConfigDir()
{
    return Path::Combine(CmajorRoot(), "config");
}

} // namespace

class AssemblyConfig
{
public:
    static void Init();
    static void Done();
    static AssemblyConfig& Instance() { return *instance; }
    int GetNumLocalRegs() const { return numLocalRegs; }
private:
    AssemblyConfig();
    void Read(const std::string& assemblyConfigFilePath);
    static std::unique_ptr<AssemblyConfig> instance;
    int numLocalRegs;
};

std::unique_ptr<AssemblyConfig> AssemblyConfig::instance;

void AssemblyConfig::Init()
{
    instance.reset(new AssemblyConfig());
}

void AssemblyConfig::Done()
{
    instance.reset();
}

AssemblyConfig::AssemblyConfig() : numLocalRegs(8)
{
    std::string assemblyConfigFilePath = Path::Combine(CmajorConfigDir(), "assembly-config.xml");
    Read(assemblyConfigFilePath);
    if (!boost::filesystem::exists(assemblyConfigFilePath))
    {
        sngxml::dom::Document assemblyConfigDoc;
        sngxml::dom::Element* rootElement = new sngxml::dom::Element(U"assembly-config");
        sngxml::dom::Element* localRegsElement = new sngxml::dom::Element(U"local-regs");
        localRegsElement->SetAttribute(U"count", ToUtf32(std::to_string(numLocalRegs)));
        rootElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(localRegsElement));
        assemblyConfigDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(rootElement));
        std::ofstream file(assemblyConfigFilePath);
        CodeFormatter formatter(file);
        formatter.SetIndentSize(1);
        assemblyConfigDoc.Write(formatter);
    }
}

void AssemblyConfig::Read(const std::string& assemblyConfigFilePath)
{
    if (boost::filesystem::exists(assemblyConfigFilePath))
    {
        std::unique_ptr<sngxml::dom::Document> assemblyConfigDoc = sngxml::dom::ReadDocument(assemblyConfigFilePath);
        std::unique_ptr<sngxml::xpath::XPathObject> localRegsObject = sngxml::xpath::Evaluate(U"/assembly-config/local-regs", assemblyConfigDoc.get());
        if (localRegsObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(localRegsObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string countAttribute = element->GetAttribute(U"count");
                    if (countAttribute.empty())
                    {
                        throw std::runtime_error("assembly-config/local-regs element has no 'count' attribute");
                    }
                    numLocalRegs = boost::lexical_cast<int>(ToUtf8(countAttribute));
                }
            }
            else
            {
                throw std::runtime_error("one assembly-config/local-regs element expected");
            }
        }
    }
}

int GetNumLocalRegs()
{
    return AssemblyConfig::Instance().GetNumLocalRegs();
}

void InitAssemblyConfig()
{
    AssemblyConfig::Init();
}

void DoneAssemblyConfig()
{
    AssemblyConfig::Done();
}

} // cmsx::intermediate
