// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_RESOURCE_TABLE_INCLUDED
#define CMAJOR_SYMBOLS_RESOURCE_TABLE_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <vector>
#include <set>

namespace cmajor { namespace symbols {

using soulng::util::BinaryReader;
using soulng::util::BinaryWriter;
using soulng::util::CodeFormatter;

struct SYMBOLS_API Resource
{
    enum class Type : int
    {
        cursor = 0, icon = 1, bitmap = 2
    };
    Resource();
    Resource(const std::u32string& name_, Type type_, const std::string& filePath_);
    void Write(BinaryWriter& writer, const std::string& cmajorRootDir);
    void Read(BinaryReader& reader, const std::string& cmajorRootDir);
    void Dump(CodeFormatter& formatter, int index);
    std::u32string name;
    Type type;
    std::string filePath;
};

std::string ResourceTypeStr(Resource::Type resourceType);

class SYMBOLS_API ResourceTable
{
public:
    bool Contains(const std::u32string& resourceName) const;
    void AddResource(const Resource& resource);
    const std::vector<Resource>& Resources() const { return resources; }
    void Write(BinaryWriter& writer);
    void Read(BinaryReader& reader);
    void Dump(CodeFormatter& formatter);
private:
    std::vector<Resource> resources;
    std::set<std::u32string> resourceNameSet;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_RESOURCE_TABLE_INCLUDED
