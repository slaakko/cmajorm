// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#ifdef _WIN32
#include <cmajor/symbols/Trap.hpp>
#endif
#include <sngcm/ast/Project.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Path.hpp>
#include <boost/filesystem.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;
using namespace soulng::util;
using namespace sngcm::ast;

bool useModuleCache = false;

bool UseModuleCache()
{
    return useModuleCache;
}

std::unique_ptr<ModuleCache> ModuleCache::instance;

void ModuleCache::Init()
{
    instance.reset(new ModuleCache());
}

void ModuleCache::Done()
{
    instance.reset();
}

std::unique_ptr<ModuleCache> ModuleCache::Release()
{
    return std::move(instance);
}

ModuleCache::ModuleCache() : moduleMap(), modules()
{
}

void ModuleCache::CollectModuleIndices(Module* module, std::unordered_set<int>& moduleIndices)
{
    for (Module* referencedModule : module->ReferencedModules())
    {
        CollectModuleIndices(referencedModule, moduleIndices);
    }
    if (!module->IsRootModule())
    {
        int moduleIndex = module->Index();
        if (moduleIndex < 0 || moduleIndex >= modules.size())
        {
            throw std::runtime_error("module cache: invalid module index");
        }
        moduleIndices.insert(moduleIndex);
    }
}

void ModuleCache::ResetCacheEntries(Module* module)
{
    std::unordered_set<int> moduleIndices;
    CollectModuleIndices(module, moduleIndices);
    for (int moduleIndex : moduleIndices)
    {
        modules[moduleIndex].reset();
    }
}

void ModuleCache::RemoveModule(const std::string& moduleFilePath)
{
    auto it = moduleMap.find(moduleFilePath);
    if (it != moduleMap.cend())
    {
        int moduleIndex = it->second;
        modules[moduleIndex].reset();
        moduleMap.erase(moduleFilePath);
    }
}

Module* ModuleCache::GetModule(const std::string& moduleFilePath)
{
    auto it = moduleMap.find(moduleFilePath);
    if (useModuleCache && it != moduleMap.cend())
    {
        int moduleIndex = it->second;
        if (moduleIndex < 0 || moduleIndex >= modules.size())
        {
            throw std::runtime_error("module cache: invalid module index, module file path=" + moduleFilePath);
        }
        return modules[moduleIndex].get();
    }
    else
    {
        std::unique_ptr<Module> module(new Module());
        int moduleIndex = modules.size();
        module->SetIndex(moduleIndex);
        Module* m = module.get();
        modules.push_back(std::move(module));
        moduleMap[moduleFilePath] = moduleIndex;
        return m;
    }
}

Module* ModuleCache::GetModule(const boost::uuids::uuid& moduleId) const
{
    auto it = moduleIdMap.find(moduleId);
    if (it != moduleIdMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void ModuleCache::MapModule(Module* module)
{
    moduleIdMap[module->Id()] = module;
}

Module* ModuleCache::GetCachedModule(const std::string& moduleFilePath) const
{
    auto it = moduleMap.find(moduleFilePath);
    if (it != moduleMap.cend())
    {
        int moduleIndex = it->second;
        if (moduleIndex < 0 || moduleIndex >= modules.size())
        {
            throw std::runtime_error("module cache: invalid module index, module file path=" + moduleFilePath);
        }
        return modules[moduleIndex].get();
    }
    else
    {
        return nullptr;
    }
}

void ModuleCache::PutModule(std::unique_ptr<Module>&& module)
{
    if (useModuleCache)
    {
        auto it = moduleMap.find(module->OriginalFilePath());
        if (it != moduleMap.cend())
        {
            int moduleIndex = it->second;
            if (moduleIndex < 0 || moduleIndex >= modules.size())
            {
                throw std::runtime_error("module cache: invalid module index");
            }
            module->SetFlag(ModuleFlags::readFromModuleFile); 
            modules[moduleIndex] = std::move(module);
        }
        else
        {
            int moduleIndex = modules.size();
            moduleMap[module->OriginalFilePath()] = moduleIndex;
            module->SetIndex(moduleIndex); 
            module->SetFlag(ModuleFlags::readFromModuleFile); 
            modules.push_back(std::move(module)); 
        }
    }
    else
    {
        ResetCacheEntries(module.get());
    }
    module.reset();
}

Module* ModuleCache::ResetModule(const std::string& moduleFilePath)
{
    if (useModuleCache)
    {
        auto it = moduleMap.find(moduleFilePath);
        if (it != moduleMap.cend())
        {
            int moduleIndex = it->second;
            if (moduleIndex < 0 || moduleIndex >= modules.size())
            {
                throw std::runtime_error("module cache: invalid module index");
            }
            modules[moduleIndex].reset(new Module());
            modules[moduleIndex]->SetIndex(moduleIndex); 
            return modules[moduleIndex].get();
        }
        else
        {
            return GetModule(moduleFilePath);
        }
    }
    else
    {
        return GetModule(moduleFilePath);
    }
}

void ModuleCache::Restore(ModuleCache* prevCache)
{
    for (const std::pair<std::string, int>& prevEntry : prevCache->moduleMap)
    {
        const std::string& moduleFilePath = prevEntry.first;
        bool moduleNotFoundOrEmpty = false;
        auto it = moduleMap.find(moduleFilePath);
        if (it == moduleMap.cend())
        {
            moduleNotFoundOrEmpty = true;
        }
        else
        {
            int index = it->second;
            Module* module = modules[index].get();
            if (!module || !module->HasSymbolTable())
            {
                moduleNotFoundOrEmpty = true;
            }
        }
        if (moduleNotFoundOrEmpty)
        {
            int index = prevEntry.second;
            std::unique_ptr<Module> module(prevCache->modules[index].release());
            if (module && module->HasSymbolTable())
            {
                int moduleIndex = modules.size();
                moduleMap[moduleFilePath] = moduleIndex;
                module->SetIndex(moduleIndex);
                moduleIdMap[module->Id()] = module.get();
                modules.push_back(std::move(module));
            }
        }
    }
}

bool ModuleCache::HasModule(const std::string& moduleFilePath) const
{
    auto it = moduleMap.find(moduleFilePath);
    if (it != moduleMap.cend())
    {
        int index = it->second;
        Module* module = modules[index].get();
        if (module && module->HasSymbolTable())
        {
            return true;
        }
    }
    return false;
}

void ModuleCache::SetModule(const std::string& moduleFilePath, std::unique_ptr<Module>&& module)
{
    int moduleIndex = modules.size();
    moduleMap[moduleFilePath] = moduleIndex;
    module->SetIndex(moduleIndex);
    moduleIdMap[module->Id()] = module.get();
    modules.push_back(std::move(module));
}

struct IsNonsystemModule
{
    bool operator()(const std::unique_ptr<Module>& module) const
    {
        if (module)
        {
            return !module->IsSystemModule();
        }
        else
        {
            return false;
        }
    }
};

void ModuleCache::MoveNonSystemModulesTo(ModuleCache* cache)
{
    std::vector<std::unique_ptr<Module>>::iterator begin = std::remove_if(modules.begin(), modules.end(), IsNonsystemModule());
    for (std::vector<std::unique_ptr<Module>>::iterator it = begin; it != modules.end(); ++it)
    {
        std::unique_ptr<Module>& module = *it;
        if (module)
        {
            moduleMap.erase(module->OriginalFilePath());
            moduleIdMap.erase(module->Id());
            cache->SetModule(module->OriginalFilePath(), std::move(module));
        }
    }
    modules.erase(begin, modules.end());
}

std::recursive_mutex mtx;

void PrepareModuleForCompilation(Module* rootModule, const std::vector<std::string>& references, sngcm::ast::Target target)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    rootModule->PrepareForCompilation(references, target);
    cmajor::symbols::MetaInit(rootModule->GetSymbolTable());
#ifdef _WIN32
    if (GetBackEnd() == BackEnd::cmsx && rootModule->Name() == U"System.Core")
    {
        cmajor::symbols::InitTrap(rootModule->GetSymbolTable());
    }
#endif
}

Module* GetModuleFromModuleCache(const std::string& moduleFilePath)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    Module* module = ModuleCache::Instance().GetModule(moduleFilePath);
    return module;
}

void PutModuleToModuleCache(std::unique_ptr<Module>&& module)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    ModuleCache::Instance().PutModule(std::move(module));
}

Module* ResetCachedModule(const std::string& moduleFilePath)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return ModuleCache::Instance().ResetModule(moduleFilePath);
}

void InitModuleCache()
{
    ModuleCache::Init();
}

void DoneModuleCache()
{
    ModuleCache::Done();
}

void ResetModuleCache()
{
    DoneModuleCache();
    InitModuleCache();
}

void SetUseModuleCache(bool useModuleCache_)
{
    useModuleCache = useModuleCache_;
}

std::unique_ptr<ModuleCache> ReleaseModuleCache()
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return ModuleCache::Release();
}

void RestoreModulesFrom(ModuleCache* prevCache)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (prevCache)
    {
        ModuleCache::Instance().Restore(prevCache);
    }
}

bool IsModuleCached(const std::string& moduleFilePath)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return ModuleCache::Instance().HasModule(moduleFilePath);
}

Module* GetCachedModule(const std::string& moduleFilePath)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return ModuleCache::Instance().GetCachedModule(moduleFilePath);
}

void SetCacheModule(const std::string& moduleFilePath, std::unique_ptr<Module>&& module)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    ModuleCache::Instance().SetModule(moduleFilePath, std::move(module));
}

void MoveNonSystemModulesTo(std::unique_ptr<ModuleCache>& cachePtr)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (!cachePtr)
    {
        cachePtr.reset(new ModuleCache());
    }
    ModuleCache::Instance().MoveNonSystemModulesTo(cachePtr.get());
}

void RemoveModuleFromCache(const std::string& moduleFilePath)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    ModuleCache::Instance().RemoveModule(moduleFilePath);
}

Module* GetModuleById(const boost::uuids::uuid& moduleId)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return ModuleCache::Instance().GetModule(moduleId);
}

void MapModule(Module* module)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    ModuleCache::Instance().MapModule(module);
}

} } // namespace cmajor::symbols
