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
#include <soulng/util/TextUtils.hpp>
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
    int MaxFilesystems() const { return maxFilesystems; }
    int NumCachedBlocks() const { return numCachedBlocks; }
    int NumBlockHashQueues() const { return numBlockHashQueues; }
    int NumCachedINodes() const { return numCachedINodes; }
    int NumINodeHashQueues() const { return numINodeHashQueues; }
    int KernelStackSize() const { return kernelStackSize; }
    int RootFSNumBlocks() const { return rootFSNumBlocks; }
    int RootFSMaxFiles() const { return rootFSMaxFiles; }
    int32_t UMask() const { return umask; }
    int32_t UID() const { return uid; }
    int32_t GID() const { return gid; }
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
    int maxFilesystems;
    int numCachedBlocks;
    int numBlockHashQueues;
    int numCachedINodes;
    int numINodeHashQueues;
    int kernelStackSize;
    int rootFSNumBlocks;
    int rootFSMaxFiles;
    int32_t umask;
    int32_t uid;
    int32_t gid;
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
    maxFilesystems(64),
    numCachedBlocks(256),
    numBlockHashQueues(16),
    numCachedINodes(256),
    numINodeHashQueues(16),
    kernelStackSize(16384),
    rootFSNumBlocks(16384),
    rootFSMaxFiles(1024),
    umask(0),
    uid(0),
    gid(0)
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
                std::u32string maxFilesystemsAttribute = configurationElement->GetAttribute(U"maxFilesystems");
                if (maxFilesystemsAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'maxFilesystems' attribute not found");
                }
                maxFilesystems = boost::lexical_cast<int>(ToUtf8(maxFilesystemsAttribute));
                std::u32string numCachedBlocksAttribute = configurationElement->GetAttribute(U"numCachedBlocks");
                if (numCachedBlocksAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'numCachedBlocks' attribute not found");
                }
                numCachedBlocks = boost::lexical_cast<int>(ToUtf8(numCachedBlocksAttribute));
                std::u32string numBlockHashQueuesAttribute = configurationElement->GetAttribute(U"numBlockHashQueues");
                if (numBlockHashQueuesAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'numBlockHashQueues' attribute not found");
                }
                numBlockHashQueues = boost::lexical_cast<int>(ToUtf8(numBlockHashQueuesAttribute));
                std::u32string numCachedINodesAttribute = configurationElement->GetAttribute(U"numCachedINodes");
                if (numCachedINodesAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'numCachedINodes' attribute not found");
                }
                numCachedINodes = boost::lexical_cast<int>(ToUtf8(numCachedINodesAttribute));
                std::u32string numINodeHashQueuesAttribute = configurationElement->GetAttribute(U"numINodeHashQueues");
                if (numINodeHashQueuesAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'numINodeHashQueues' attribute not found");
                }
                numINodeHashQueues = boost::lexical_cast<int>(ToUtf8(numINodeHashQueuesAttribute));
                std::u32string kernelStackSizeAttribute = configurationElement->GetAttribute(U"kernelStackSize");
                if (kernelStackSizeAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'kernelStackSize' attribute not found");
                }
                kernelStackSize = boost::lexical_cast<int>(ToUtf8(kernelStackSizeAttribute));
                std::u32string rootFSNumBlocksAttribute = configurationElement->GetAttribute(U"rootFSNumBlocks");
                if (rootFSNumBlocksAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'rootFSNumBlocks' attribute not found");
                }
                rootFSNumBlocks = boost::lexical_cast<int>(ToUtf8(rootFSNumBlocksAttribute));
                std::u32string rootFSMaxFilesAttribute = configurationElement->GetAttribute(U"rootFSMaxFiles");
                if (rootFSMaxFilesAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'rootFSMaxFiles' attribute not found");
                }
                rootFSMaxFiles = boost::lexical_cast<int>(ToUtf8(rootFSMaxFilesAttribute));
                std::u32string umaskAttribute = configurationElement->GetAttribute(U"umask");
                if (umaskAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'umask' attribute not found");
                }
                if (umaskAttribute.length() != 3 ||
                    umaskAttribute[0] < '0' || umaskAttribute[0] > '7' ||
                    umaskAttribute[1] < '0' || umaskAttribute[1] > '7' ||
                    umaskAttribute[2] < '0' || umaskAttribute[2] > '7')
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': invalid 'umask' attribute: three octal digits expected");
                }
                umask = ParseOctal(ToUtf8(umaskAttribute));
                std::u32string uidAttribute = configurationElement->GetAttribute(U"uid");
                if (uidAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'uid' attribute not found");
                }
                uid = boost::lexical_cast<int32_t>(ToUtf8(uidAttribute));
                std::u32string gidAttribute = configurationElement->GetAttribute(U"gid");
                if (gidAttribute.empty())
                {
                    throw std::runtime_error("error reading configuration from '" + filePath + "': 'gid' attribute not found");
                }
                gid = boost::lexical_cast<int32_t>(ToUtf8(gidAttribute));
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
    configurationElement->SetAttribute(U"maxFilesystems", ToUtf32(std::to_string(maxFilesystems)));
    configurationElement->SetAttribute(U"numCachedBlocks", ToUtf32(std::to_string(numCachedBlocks)));
    configurationElement->SetAttribute(U"numBlockHashQueues", ToUtf32(std::to_string(numBlockHashQueues)));
    configurationElement->SetAttribute(U"numCachedINodes", ToUtf32(std::to_string(numCachedINodes)));
    configurationElement->SetAttribute(U"numINodeHashQueues", ToUtf32(std::to_string(numINodeHashQueues)));
    configurationElement->SetAttribute(U"kernelStackSize", ToUtf32(std::to_string(kernelStackSize)));
    configurationElement->SetAttribute(U"rootFSNumBlocks", ToUtf32(std::to_string(rootFSNumBlocks)));
    configurationElement->SetAttribute(U"rootFSMaxFiles", ToUtf32(std::to_string(rootFSMaxFiles)));
    configurationElement->SetAttribute(U"umask", ToUtf32(ToOctalString(umask, 3)));
    configurationElement->SetAttribute(U"uid", ToUtf32(std::to_string(uid)));
    configurationElement->SetAttribute(U"gid", ToUtf32(std::to_string(gid)));
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

int MaxFilesystems()
{
    return Configuration::Instance().MaxFilesystems();
}

int NumCachedBlocks()
{
    return Configuration::Instance().NumCachedBlocks();
}

int NumBlockHashQueues()
{
    return Configuration::Instance().NumBlockHashQueues();
}

int NumCachedINodes()
{
    return Configuration::Instance().NumCachedINodes();
}

int NumINodeHashQueues()
{
    return Configuration::Instance().NumINodeHashQueues();
}

int KernelStackSize()
{
    return Configuration::Instance().KernelStackSize();
}

int RootFSNumBlocks()
{
    return Configuration::Instance().RootFSNumBlocks();
}

int RootFSMaxFiles()
{
    return Configuration::Instance().RootFSMaxFiles();
}

int32_t UID()
{
    return Configuration::Instance().UID();
}

int32_t GID()
{
    return Configuration::Instance().GID();
}

int32_t UMask()
{
    return Configuration::Instance().UMask();
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
