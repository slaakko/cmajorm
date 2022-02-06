// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_RESOURCE_INCLUDED
#define CMSX_OBJECT_RESOURCE_INCLUDED
#include <system-x/object/Api.hpp>
#include <string>
#include <vector>
#include <memory>

namespace cmsx::object {

class ResourceSection;
class BinaryFile;
class ExecutableFile;

enum ResourceType : int32_t
{
    binary = 0
};

class CMSX_OBJECT_API Resource
{
public:
    Resource();
    Resource(const std::string& name_, ResourceType type_, const std::string& filePath_);
    const std::string& Name() const { return name; }
    ResourceType Type() const { return type; }
    const std::string& FilePath() const { return filePath; }
    void Emit(ResourceSection& resourceSection);
    void Read(ResourceSection& resourceSection);
private:
    std::string name;
    ResourceType type;
    std::string filePath;
};

class CMSX_OBJECT_API ResourceInfo
{
public:
    ResourceInfo(int32_t symbolIndex_, int64_t offset_);
    int32_t SymbolIndex() const { return symbolIndex; }
    int64_t Offset() const { return offset; }
private:
    int32_t symbolIndex;
    int64_t offset;
};

CMSX_OBJECT_API std::string GenerateResourceUnit(const std::string& resourceXmlFilePath, const std::string& libraryDir);
CMSX_OBJECT_API void ProcessResources(const std::vector<std::unique_ptr<BinaryFile>>& binaryFiles, ExecutableFile& executable);

} // namespace cmsx::object

#endif // CMSX_OBJECT_RESOURCE_INCLUDED
