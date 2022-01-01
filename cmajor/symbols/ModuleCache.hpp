// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
#define CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmajor { namespace symbols {

class SYMBOLS_API ModuleCache
{
public:
    ModuleCache();
    ModuleCache(const ModuleCache&) = delete;
    ModuleCache(ModuleCache&&) = delete;
    ModuleCache& operator=(const ModuleCache&) = delete;
    ModuleCache& operator=(ModuleCache&&) = delete;
    static void Init();
    static void Done();
    static std::unique_ptr<ModuleCache> Release();
    static ModuleCache& Instance() { return *instance; }
    Module* GetModule(const std::string& moduleFilePath);
    Module* GetModule(const boost::uuids::uuid& moduleId) const;
    void MapModule(Module* module);
    Module* GetCachedModule(const std::string& moduleFilePath) const;
    void PutModule(std::unique_ptr<Module>&& module);
    Module* ResetModule(const std::string& moduleFilePath);
    void ResetCacheEntries(Module* module);
    void RemoveModule(const std::string& moduleFilePath);
    void Restore(ModuleCache* prevCache);
    bool HasModule(const std::string& moduleFilePath) const;
    void SetModule(const std::string& moduleFilePath, std::unique_ptr<Module>&& module);
    void MoveNonSystemModulesTo(ModuleCache* cache);
    void Update();
private:
    static std::unique_ptr<ModuleCache> instance;
    std::unordered_map<std::string, int> moduleMap;
    std::vector<std::unique_ptr<Module>> modules;
    std::unordered_map<boost::uuids::uuid, Module*, boost::hash<boost::uuids::uuid>> moduleIdMap;
    void CollectModuleIndices(Module* module, std::unordered_set<int>& moduleIndeces);
};

SYMBOLS_API void PrepareModuleForCompilation(Module* rootModule, const std::vector<std::string>& references, sngcm::ast::Target target);
SYMBOLS_API Module* GetModuleFromModuleCache(const std::string& moduleFilePath);
SYMBOLS_API void PutModuleToModuleCache(std::unique_ptr<Module>&& module);
SYMBOLS_API Module* ResetCachedModule(const std::string& moduleFilePath);
SYMBOLS_API void InitModuleCache();
SYMBOLS_API void DoneModuleCache();
SYMBOLS_API void ResetModuleCache();
SYMBOLS_API void SetUseModuleCache(bool useModuleCache_);
SYMBOLS_API bool UseModuleCache();
SYMBOLS_API std::unique_ptr<ModuleCache> ReleaseModuleCache();
SYMBOLS_API void RestoreModulesFrom(ModuleCache* prevCache);
SYMBOLS_API bool IsModuleCached(const std::string& moduleFilePath);
SYMBOLS_API Module* GetCachedModule(const std::string& moduleFilePath);
SYMBOLS_API void SetCacheModule(const std::string& moduleFilePath, std::unique_ptr<Module>&& module);
SYMBOLS_API void RemoveModuleFromCache(const std::string& moduleFilePath);
SYMBOLS_API void MoveNonSystemModulesTo(std::unique_ptr<ModuleCache>& cachePtr);
SYMBOLS_API Module* GetModuleById(const boost::uuids::uuid& moduleId);
SYMBOLS_API void MapModule(Module* module);
SYMBOLS_API void UpdateModuleCache();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
