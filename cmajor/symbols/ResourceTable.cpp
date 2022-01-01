// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ResourceTable.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using sngcm::ast::CmajorRootDir;
using soulng::util::GetFullPath;
using soulng::util::Path;
using soulng::util::EndsWith;
using soulng::unicode::ToUtf8;

std::string ResourceTypeStr(Resource::Type resourceType)
{
    switch (resourceType)
    {
        case Resource::Type::bitmap: return "bitmap";
        case Resource::Type::icon: return "icon";
        case Resource::Type::cursor: return "cursor";
    }
    return std::string();
}

std::string MakeCmajorRootDirRelativeFilePath(const std::string& cmajorRootDir, const std::string& filePath)
{
    if (filePath.find(cmajorRootDir, 0) == 0)
    {
        return "$CMAJOR$" + filePath.substr(cmajorRootDir.size());
    }
    return filePath;
}

std::string MakeFullPathFromCmajorRootDirRelativeFilePath(const std::string& cmajorRootDir, const std::string& filePath)
{
    if (filePath.find("$CMAJOR$") == 0)
    {
        return Path::Combine(cmajorRootDir, filePath.substr(8));
    }
    return filePath;
}

Resource::Resource() : name(), type(), filePath()
{
}

Resource::Resource(const std::u32string& name_, Type type_, const std::string& filePath_) : name(name_), type(type_), filePath(GetFullPath(filePath_))
{
}

void Resource::Write(BinaryWriter& writer, const std::string& cmajorRootDir)
{
    writer.Write(name);
    writer.Write(static_cast<int32_t>(type));
    writer.Write(MakeCmajorRootDirRelativeFilePath(cmajorRootDir, filePath));
}

void Resource::Read(BinaryReader& reader, const std::string& cmajorRootDir)
{
    name = reader.ReadUtf32String();
    type = static_cast<Type>(reader.ReadInt());
    filePath = reader.ReadUtf8String();
    filePath = MakeFullPathFromCmajorRootDirRelativeFilePath(cmajorRootDir, filePath);
}

void Resource::Dump(CodeFormatter& formatter, int index)
{
    formatter.WriteLine("RESOURCE " + std::to_string(index));
    formatter.WriteLine("resource name: " + ToUtf8(name));
    formatter.WriteLine("resource type: " + ResourceTypeStr(type));
    formatter.WriteLine("resource file: " + filePath);
}

bool ResourceTable::Contains(const std::u32string& resourceName) const
{
    return resourceNameSet.find(resourceName) != resourceNameSet.cend();
}

void ResourceTable::AddResource(const Resource& resource)
{
    resourceNameSet.insert(resource.name);
    resources.push_back(resource);
}

void ResourceTable::Write(BinaryWriter& writer)
{
    int32_t n = resources.size();
    writer.Write(n);
    if (n > 0)
    {
        std::string cmajorRootDir = GetFullPath(CmajorRootDir());
        if (!EndsWith(cmajorRootDir, "/"))
        {
            cmajorRootDir.append("/");
        }
        for (int32_t i = 0; i < n; ++i)
        {
            resources[i].Write(writer, cmajorRootDir);
        }
    }
}

void ResourceTable::Read(BinaryReader& reader)
{
    int32_t n = reader.ReadInt();
    if (n > 0)
    {
        std::string cmajorRootDir = GetFullPath(CmajorRootDir());
        if (!EndsWith(cmajorRootDir, "/"))
        {
            cmajorRootDir.append("/");
        }
        for (int32_t i = 0; i < n; ++i)
        {
            Resource resource;
            resource.Read(reader, cmajorRootDir);
            resources.push_back(resource);
        }
    }
}

void ResourceTable::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine("RESOURCE TABLE:");
    int index = 0;
    for (Resource& resource : resources)
    {
        resource.Dump(formatter, index);
        ++index;
    }
}

} } // namespace cmajor::symbols
