// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Resource.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BinaryStreamReader.hpp>
#include <boost/filesystem.hpp>

namespace cmsx::object {

using namespace soulng::unicode;

ResourceType ParseResourceType(const std::string& typeStr, const std::string& resourceXmlFilePath)
{
    if (typeStr == "binary")
    {
        return ResourceType::binary;
    }
    else
    {
        throw std::runtime_error("unknown resource type '" + typeStr + "' in resource file '" + resourceXmlFilePath + "'");
    }
}

Resource::Resource() : name(), type(), filePath()
{
}

Resource::Resource(const std::string& name_, ResourceType type_, const std::string& filePath_) : name(name_), type(type_), filePath(filePath_)
{
}

void Resource::Emit(ResourceSection& resourceSection)
{
    resourceSection.EmitTetra(static_cast<int32_t>(type));
    resourceSection.EmitString(name);
    resourceSection.EmitString(filePath);
}

void Resource::Read(ResourceSection& resourceSection)
{
    type = static_cast<ResourceType>(resourceSection.ReadTetra());
    name = resourceSection.ReadString();
    filePath = resourceSection.ReadString();
}

ResourceInfo::ResourceInfo(int32_t symbolIndex_, int64_t offset_) : symbolIndex(symbolIndex_), offset(offset_)
{
}

std::string GenerateResourceUnit(const std::string& resourceXmlFilePath, const std::string& libraryDir)
{
    std::string objectFilePath = Path::Combine(libraryDir, Path::GetFileName(resourceXmlFilePath) + ".o");
    std::unique_ptr<sngxml::dom::Document> resourceDoc = sngxml::dom::ReadDocument(resourceXmlFilePath);
    std::unique_ptr<sngxml::xpath::XPathObject> resourceObject = sngxml::xpath::Evaluate(U"/resources/resource", resourceDoc.get());
    if (resourceObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(resourceObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                std::u32string nameAttr = element->GetAttribute(U"name");
                if (nameAttr.empty())
                {
                    throw std::runtime_error("'name' attribute of resource not set in resource file '" + resourceXmlFilePath + "'");
                }
                std::string resourceName = ToUtf8(nameAttr);
                std::u32string typeAttr = element->GetAttribute(U"type");
                if (typeAttr.empty())
                {
                    throw std::runtime_error("'type' attribute of resource not set in resource file '" + resourceXmlFilePath + "'");
                }
                ResourceType resourceType = ParseResourceType(ToUtf8(typeAttr), resourceXmlFilePath);
                std::u32string fileAttr = element->GetAttribute(U"file");
                if (fileAttr.empty())
                {
                    throw std::runtime_error("'file' attribute of resource not set in resource file '" + resourceXmlFilePath + "'");
                }
                std::string filePath = Path::Combine(Path::GetDirectoryName(resourceXmlFilePath), ToUtf8(fileAttr));
                std::unique_ptr<Resource> resource(new Resource(resourceName, resourceType, filePath));
                std::unique_ptr<ObjectFile> objectFile(new ObjectFile(objectFilePath));
                objectFile->CreateSections();
                objectFile->GetResourceSection()->AddResource(resource.release());
                objectFile->Finalize();
                objectFile->WriteFile();
            }
        }
    }
    return objectFilePath;
}

void ProcessBinaryResource(Resource* resource, ObjectFile* objectFile, ExecutableFile& executable)
{
    const std::string& resourceFilePath = resource->FilePath();
    int64_t length = boost::filesystem::file_size(resourceFilePath);
    std::unique_ptr<Symbol> resourceSymbol(new Symbol(SymbolKind::global, "", "@resource." + resource->Name()));
    resourceSymbol->SetLength(length);
    FileStream fileStream(resourceFilePath, OpenMode::read | OpenMode::binary);
    BufferedStream bufferedStream(fileStream);
    BinaryStreamReader reader(bufferedStream);
    int64_t offset = executable.GetResourceSection()->Address();
    for (int64_t i = 0; i < length; ++i)
    {
        uint8_t x = reader.ReadByte();
        executable.GetResourceSection()->EmitByte(x);
    }
    Symbol* rs = resourceSymbol.get();
    executable.GetSymbolTable().AddSymbol(resourceSymbol.release());
    ResourceInfo resourceInfo(rs->Index(), offset);
    executable.GetResourceSection()->AddResourceInfo(resourceInfo);
}

void ProcessResource(Resource* resource, ObjectFile* objectFile, ExecutableFile& executable)
{
    switch (resource->Type())
    {
        case ResourceType::binary:
        {
            ProcessBinaryResource(resource, objectFile, executable);
            break;
        }
        default:
        {
            throw std::runtime_error("unknown resource type in resource section of object file '" + objectFile->FilePath() + "'");
        }
    }
}

void ProcessResourceInObjectFile(ObjectFile* objectFile, ExecutableFile& executable)
{
    ResourceSection* resourceSection = objectFile->GetResourceSection();
    if (resourceSection)
    {
        for (const auto& resource : resourceSection->Resources())
        {
            ProcessResource(resource.get(), objectFile, executable);
        }
    }
}

void ProcessResources(const std::vector<std::unique_ptr<BinaryFile>>& binaryFiles, ExecutableFile& executable)
{
    for (const auto& binaryFile : binaryFiles)
    {
        switch (binaryFile->Kind())
        {
            case BinaryFileKind::objectFile:
            {
                ObjectFile* objectFile = static_cast<ObjectFile*>(binaryFile.get());
                ProcessResourceInObjectFile(objectFile, executable);
                break;
            }
            case BinaryFileKind::archiveFile:
            {
                ArchiveFile* archiveFile = static_cast<ArchiveFile*>(binaryFile.get());
                for (const auto& objectFile : archiveFile->ObjectFiles())
                {
                    ProcessResourceInObjectFile(objectFile.get(), executable);
                }
                break;
            }
        }
    }
}

} // namespace cmsx::object
