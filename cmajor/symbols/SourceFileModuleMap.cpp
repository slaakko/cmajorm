// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/SourceFileModuleMap.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <map>

namespace cmajor { namespace symbols {

struct SourceFileModuleMapKey
{
    SourceFileModuleMapKey(sngcm::ast::BackEnd backend_, sngcm::ast::Config config_, const std::string& sourceFilePath_) : backend(backend_), config(config_), sourceFilePath(sourceFilePath_) {}
    sngcm::ast::BackEnd backend;
    sngcm::ast::Config config;
    std::string sourceFilePath;
};

bool operator==(const SourceFileModuleMapKey& left, const SourceFileModuleMapKey& right)
{
    return left.backend == right.backend && left.config == right.config && left.sourceFilePath == right.sourceFilePath;
}

bool operator<(const SourceFileModuleMapKey& left, const SourceFileModuleMapKey& right)
{
    if (left.backend < right.backend) return true;
    if (left.backend > right.backend) return false;
    if (left.config < right.config) return true;
    if (left.config > right.config) return false;
    return left.sourceFilePath < right.sourceFilePath;
}

class SourceFileModuleMap
{
public:
    static void Init();
    static void Done();
    static SourceFileModuleMap& Instance() { return *instance; }
    void SetModuleId(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath, const boost::uuids::uuid& moduleId);
    boost::uuids::uuid GetModuleId(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath) const;
private:
    static std::unique_ptr<SourceFileModuleMap> instance;
    std::map<SourceFileModuleMapKey, boost::uuids::uuid> sourceFileModuleMap;
    SourceFileModuleMap();
};

std::unique_ptr<SourceFileModuleMap> SourceFileModuleMap::instance;

SourceFileModuleMap::SourceFileModuleMap()
{
}

void SourceFileModuleMap::Init()
{
    instance.reset(new SourceFileModuleMap());
}

void SourceFileModuleMap::Done()
{
    instance.reset();
}

void SourceFileModuleMap::SetModuleId(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath, const boost::uuids::uuid& moduleId)
{
    SourceFileModuleMapKey key(backend, config, sourceFilePath);
    sourceFileModuleMap[key] = moduleId;
}

boost::uuids::uuid SourceFileModuleMap::GetModuleId(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath) const
{
    SourceFileModuleMapKey key(backend, config, sourceFilePath);
    auto it = sourceFileModuleMap.find(key);
    if (it != sourceFileModuleMap.cend())
    {
        return it->second;
    }
    else
    {
        return boost::uuids::nil_uuid();
    }
}

void MapSourceFileToModuleId(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath, const boost::uuids::uuid& moduleId)
{
    SourceFileModuleMap::Instance().SetModuleId(backend, config, sourceFilePath, moduleId);
}

Module* GetModuleBySourceFile(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath)
{
    boost::uuids::uuid moduleId = SourceFileModuleMap::Instance().GetModuleId(backend, config, sourceFilePath);
    if (moduleId.is_nil())
    {
        return nullptr;
    }
    else
    {
        return GetModuleById(moduleId);
    }
}

void InitSourceFileModuleMap()
{
    SourceFileModuleMap::Init();
}

void DoneSourceFileModuleMap()
{
    SourceFileModuleMap::Done();
}

} } // namespace cmajor::symbols;
