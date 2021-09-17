// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_set>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
    }
    ~InitDone()
    {
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;
using namespace soulng::unicode;

struct Resource
{
    Resource(const std::string& name_, const std::string& type_, const std::string& file_);
    std::string name;
    std::string type;
    std::string file;
};

Resource::Resource(const std::string& name_, const std::string& type_, const std::string& file_) : name(name_), type(type_), file(file_)
{
}

void ProcessFile(const std::string& filePath, bool verbose, std::vector<Resource>& resources, std::unordered_set<std::string>& names)
{
    if (verbose)
    {
        std::cout << "> " << filePath << std::endl;
    }
    std::string dir = Path::GetDirectoryName(filePath);
    std::unique_ptr<sngxml::dom::Document> resourceDoc = sngxml::dom::ReadDocument(filePath);
    std::unique_ptr<sngxml::xpath::XPathObject> includeResult = sngxml::xpath::Evaluate(U"/resources/include", resourceDoc.get());
    if (includeResult)
    {
        if (includeResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(includeResult.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string fileAttribute = element->GetAttribute(U"file");
                    if (fileAttribute.empty())
                    {
                        throw std::runtime_error("error: include element " + std::to_string(i) + " has no file attribute in resource XML file '" + filePath + "'");
                    }
                    std::string includeFilePath = GetFullPath(Path::Combine(dir, ToUtf8(fileAttribute)));
                    if (boost::filesystem::exists(includeFilePath))
                    {
                        ProcessFile(includeFilePath, verbose, resources, names);
                    }
                    else
                    {
                        throw std::runtime_error("error: include file '" + includeFilePath + "' in resource XML file '" + filePath + "' does not exist");
                    }
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> resourceResult = sngxml::xpath::Evaluate(U"/resources/resource", resourceDoc.get());
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
                    std::u32string nameAttribute = element->GetAttribute(U"name");
                    if (nameAttribute.empty())
                    {
                        throw std::runtime_error("error: resource element " + std::to_string(i) + " has no name attribute in resource XML file '" + filePath + "'");
                    }
                    std::u32string typeAttribute = element->GetAttribute(U"type");
                    if (typeAttribute.empty())
                    {
                        throw std::runtime_error("error: resource element " + std::to_string(i) + " has no type attribute in resource XML file '" + filePath + "'");
                    }
                    std::u32string fileAttribute = element->GetAttribute(U"file");
                    if (fileAttribute.empty())
                    {
                        throw std::runtime_error("error: resource element " + std::to_string(i) + " has no file attribute in resource XML file '" + filePath + "'");
                    }
                    std::string name = ToUtf8(nameAttribute);
                    if (names.find(name) != names.cend())
                    {
                        throw std::runtime_error("resource name '" + name + "' not unique in resource XML file '" + filePath + "'");
                    }
                    names.insert(name);
                    Resource resource(name, ToUtf8(typeAttribute), GetFullPath(Path::Combine(dir, ToUtf8(fileAttribute))));
                    resources.push_back(resource);
                }
            }
        }
    }
}

std::string GetResourceTypeName(const std::string& type)
{
    if (type == "cursor")
    {
        return "CURSOR";
    }
    else if (type == "icon")
    {
        return "ICON";
    }
    else if (type == "bitmap")
    {
        return "BITMAP";
    }
    else
    {
        throw std::runtime_error("unknown resource type '" + type + "'");
    }
}

void CreateResourceScriptFile(const std::vector<Resource>& resources, const std::string& resourceScriptFileName, bool verbose)
{
    std::ofstream resourceScriptFile(resourceScriptFileName);
    CodeFormatter formatter(resourceScriptFile);
    for (const Resource& resource : resources)
    {
        std::string line;
        line.append(resource.name).append(1, ' ');
        line.append(GetResourceTypeName(resource.type)).append(1, ' ');
        line.append(1, '"').append(resource.file).append(1, '"');
        formatter.WriteLine(line);
    }
    if (verbose)
    {
        std::cout << "==> " + resourceScriptFileName << std::endl;
    }
}

const char* version = "4.2.0";

void PrintHelp()
{
    std::cout << "Cmajor resource compiler version " << version << std::endl;
    std::cout << "Usage cmrc [options] {RESOURCE.FILE.XML}" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        bool verbose = false;
        std::vector<std::string> files;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (StartsWith(arg, "-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'v':
                        {
                            verbose = true;
                            break;
                        }
                        case 'h':
                        {
                            PrintHelp();
                            return 1;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                files.push_back(GetFullPath(arg));
            }
        }
        for (const std::string& file : files)
        {
            std::vector<Resource> resources;
            std::unordered_set<std::string> names;
            ProcessFile(file, verbose, resources, names);
            std::string resourceScriptFile = Path::ChangeExtension(file, ".rc");
            CreateResourceScriptFile(resources, resourceScriptFile, verbose);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}