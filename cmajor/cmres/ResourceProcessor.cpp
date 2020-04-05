// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmres/ResourceProcessor.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <fstream>

namespace cmajor { namespace resources {

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;

struct Resource
{
    enum class Type
    {
        cursor = 0, icon = 1, bitmap = 2
    };
    Resource(const std::u32string& name_, Type type_, const std::string& file_) : name(name_), type(type_), file(file_) {}
    std::u32string name;
    Type type;
    std::string file;
};

class ResourceTypeNameRegistry
{
public:
    static void Init();
    static void Done();
    static ResourceTypeNameRegistry& Instance() { return *instance; }
    Resource::Type GetResourceType(const std::u32string& resourceTypeName) const;
    const std::string& GetResourceTypeName(Resource::Type resourceType) const;
private:
    static std::unique_ptr<ResourceTypeNameRegistry> instance;
    ResourceTypeNameRegistry();
    std::unordered_map<std::u32string, Resource::Type> resourceTypeMap;
    std::vector<std::string> resourceTypeNames;
};

std::unique_ptr<ResourceTypeNameRegistry> ResourceTypeNameRegistry::instance;

ResourceTypeNameRegistry::ResourceTypeNameRegistry()
{
    resourceTypeMap[U"cursor"] = Resource::Type::cursor;
    resourceTypeMap[U"icon"] = Resource::Type::icon;
    resourceTypeMap[U"bitmap"] = Resource::Type::bitmap;
    resourceTypeNames.push_back("CURSOR");
    resourceTypeNames.push_back("ICON");
    resourceTypeNames.push_back("BITMAP");
}

Resource::Type ResourceTypeNameRegistry::GetResourceType(const std::u32string& resourceTypeName) const
{
    auto it = resourceTypeMap.find(resourceTypeName);
    if (it != resourceTypeMap.cend())
    {
        return it->second;
    }
    throw std::runtime_error("resource type name '" + ToUtf8(resourceTypeName) + "' not found from resource type name registry");
}

const std::string& ResourceTypeNameRegistry::GetResourceTypeName(Resource::Type resourceType) const
{
    int resourceTypeIndex = static_cast<int>(resourceType);
    if (resourceTypeIndex >= resourceTypeNames.size())
    {
        throw std::runtime_error("internal error in resource processor");
    }
    return resourceTypeNames[resourceTypeIndex];
}

void ResourceTypeNameRegistry::Init()
{
    instance.reset(new ResourceTypeNameRegistry());
}

void ResourceTypeNameRegistry::Done()
{
    instance.reset();
}

Resource::Type GetResourceType(const std::u32string& resourceTypeName)
{
    return ResourceTypeNameRegistry::Instance().GetResourceType(resourceTypeName);
}

const std::string& GetResourceTypeName(Resource::Type resourceType)
{
    return ResourceTypeNameRegistry::Instance().GetResourceTypeName(resourceType);
}

void CreateResourceScriptFile(cmajor::symbols::Module& currentModule, const std::string& resourceScriptFileName, const std::vector<Resource>& resources)
{
    std::ofstream resourceScriptFile(resourceScriptFileName);
    CodeFormatter formatter(resourceScriptFile);
    for (const Resource& resource : resources)
    {
        std::string line;
        line.append(ToUtf8(resource.name)).append(1, ' ');
        line.append(GetResourceTypeName(resource.type)).append(1, ' ');
        line.append(1, '"').append(resource.file).append(1, '"');
        formatter.WriteLine(line);
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(currentModule.LogStreamId(), "==> " + resourceScriptFileName);
    }
}

void CompileResourceScriptFile(cmajor::symbols::Module& currentModule, const std::string& resourceScriptFileName)
{
    std::string resourceFilePath = currentModule.ResourceFilePath();
    std::string errorFilePath = Path::Combine(Path::GetDirectoryName(resourceScriptFileName), "llvm-rc.error");
    std::string commandLine;
    commandLine.append("cmfileredirector -2 ").append(QuotedPath(errorFilePath)).append(" llvm-rc /V /FO ").append(QuotedPath(resourceFilePath));
    commandLine.append(1, ' ').append(QuotedPath(resourceScriptFileName));
    try
    {
        System(commandLine);
        boost::filesystem::remove(boost::filesystem::path(errorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(errorFilePath);
        throw std::runtime_error("compiling resource script '" + resourceScriptFileName + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(currentModule.LogStreamId(), "==> " + resourceFilePath);
    }
    currentModule.AddResourceFilePathToResourceFilePaths();
}

std::vector<Resource> GetResourcesInProject(sngcm::ast::Project& project, cmajor::symbols::Module& currentModule)
{
    std::string cmajorResourceDir = sngcm::ast::CmajorResourceDir();
    std::vector<Resource> resources;
    std::set<std::u32string> resourceNameSet;
    currentModule.CollectResourceNames(resourceNameSet);
    int n = project.ResourceFilePaths().size();
    for (int i = 0; i < n; ++i)
    {
        const std::string& relativeResourceFilePath = project.RelativeResourceFilePaths()[i];
        const std::string& resourceFilePath = project.ResourceFilePaths()[i];
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            LogMessage(currentModule.LogStreamId(), "> " + relativeResourceFilePath);
        }
        std::unique_ptr<sngxml::dom::Document> resourceXmlDoc = sngxml::dom::ReadDocument(resourceFilePath);
        std::unique_ptr<sngxml::xpath::XPathObject> resourceResult = sngxml::xpath::Evaluate(U"/resources/resource", resourceXmlDoc.get());
        if (resourceResult)
        {
            if (resourceResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(resourceResult.get());
                int n = nodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    sngxml::dom::Node* node = (*nodeSet)[i];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                        std::u32string resourceName = element->GetAttribute(U"name");
                        if (resourceName.empty())
                        {
                            throw std::runtime_error(std::to_string(i) + "'th resource element has no name attribute in resource XML file '" + resourceFilePath + "' in project '" + project.FilePath() + "'.");
                        }
                        if (resourceNameSet.find(resourceName) != resourceNameSet.cend())
                        {
                            throw std::runtime_error(std::to_string(i) + "Resource name '" + ToUtf8(resourceName) + "' not unique. Detected when processing resource XML file '" + resourceFilePath + "' in project '" + project.FilePath() + "'.");
                        }
                        resourceNameSet.insert(resourceName);
                        std::u32string resourceType = element->GetAttribute(U"type");
                        if (resourceType.empty())
                        {
                            throw std::runtime_error(std::to_string(i) + "'th resource element has no type attribute in resource XML file '" + resourceFilePath + "' in project '" + project.FilePath() + "'.");
                        }
                        std::u32string resourceFile = element->GetAttribute(U"file");
                        if (resourceFile.empty())
                        {
                            throw std::runtime_error(std::to_string(i) + "'th resource element has no file attribute in resource XML file '" + resourceFilePath + "' in project '" + project.FilePath() + "'.");
                        }
                        currentModule.AddResourceName(resourceName);
                        std::string resourceFilePath = Path::MakeCanonical(ToUtf8(resourceFile));
                        std::string fullResourceFilePath = resourceFilePath;
                        if (Path::IsRelative(resourceFilePath))
                        {
                            fullResourceFilePath = Path::Combine(cmajorResourceDir, resourceFilePath);
                        }
                        fullResourceFilePath = GetFullPath(fullResourceFilePath);
                        if (!boost::filesystem::exists(fullResourceFilePath))
                        {
                            fullResourceFilePath = GetFullPath(Path::Combine(project.SourceBasePath().generic_string(), resourceFilePath));
                        }
                        if (boost::filesystem::exists(fullResourceFilePath))
                        {
                            Resource resource(resourceName, GetResourceType(resourceType), fullResourceFilePath);
                            resources.push_back(std::move(resource));
                        }
                        else
                        {
                            throw std::runtime_error("resource file '" + ToUtf8(resourceFile) + "' not found when processing resource XML file '" + resourceFilePath + "' in project '" + project.FilePath() + "'.");
                        }
                    }
                }
            }
        }
    }
    return resources;
}

void ProcessResourcesInProject(sngcm::ast::Project& project, cmajor::symbols::Module& currentModule)
{
    if (project.ResourceFilePaths().empty()) return;
    currentModule.SetHasResourceFile();
    std::vector<Resource> resources = GetResourcesInProject(project, currentModule);
    std::string resourceScriptFileName = Path::ChangeExtension(project.ModuleFilePath(), ".rc");
    CreateResourceScriptFile(currentModule, resourceScriptFileName, resources);
    CompileResourceScriptFile(currentModule, resourceScriptFileName);
}

void InitResources()
{
    ResourceTypeNameRegistry::Init();
}

void DoneResources()
{
    ResourceTypeNameRegistry::Done();
}

} } // namespace cmajor::resources
