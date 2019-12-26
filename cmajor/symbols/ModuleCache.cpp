// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#ifdef _WIN32
#include <cmajor/symbols/Trap.hpp>
#endif
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;
using namespace soulng::util;

bool useModuleCache = false;

bool UseModuleCache()
{
    return useModuleCache;
}

class ModuleCache
{
public:
    static void Init();
    static void Done();
    static ModuleCache& Instance() { return *instance; }
    Module* GetModule(const std::string& moduleFilePath);
    void PutModule(std::unique_ptr<Module>&& module);
private:
    static std::unique_ptr<ModuleCache> instance;
    std::unordered_map<std::string, int> moduleMap;
    std::vector<std::unique_ptr<Module>> modules;
    void CollectModuleIndices(Module* module, std::unordered_set<int>& moduleIndeces);
    void ResetCacheEntries(Module* module);
};

std::unique_ptr<ModuleCache> ModuleCache::instance;

void ModuleCache::Init()
{
    instance.reset(new ModuleCache());
}

void ModuleCache::Done()
{
    instance.reset();
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
            modules[moduleIndex].reset(new Module());
        }
        else
        {
            int moduleIndex = modules.size();
            moduleMap[module->OriginalFilePath()] = moduleIndex;
            std::unique_ptr<Module> m(new Module());
            m->SetIndex(moduleIndex);
            modules.push_back(std::move(m));
        }
    }
    else
    {
        ResetCacheEntries(module.get());
    }
    module.reset();
}

std::recursive_mutex mtx;

void PrepareModuleForCompilation(Module* rootModule, const std::vector<std::string>& references)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    rootModule->PrepareForCompilation(references);
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

} } // namespace cmajor::symbols
