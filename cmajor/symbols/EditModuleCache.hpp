// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_EDIT_MODULE_CACHE_INCLUDED
#define CMAJOR_SYMBOLS_EDIT_MODULE_CACHE_INCLUDED
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmajor { namespace symbols {

using ReadProjectFunc = std::unique_ptr<sngcm::ast::Project> (*)(const std::string&);

SYMBOLS_API void SetReadProjectFunction(ReadProjectFunc readProjectFunc);

SYMBOLS_API std::string MakeEditModuleKey(const std::string& projectFilePath, const std::string& backendStr, const std::string& configurationStr);

enum class LoadEditModuleResultKind
{
    moduleUpToDate, moduleLoaded, moduleReloaded
};

struct SYMBOLS_API LoadEditModuleResult
{
    LoadEditModuleResult() : ok(true), numberOfErrors(), error(), resultKind(LoadEditModuleResultKind::moduleUpToDate), startLoading(), startParsing(), endLoading()
    {
    }
    bool ok;
    int numberOfErrors;
    std::string error;
    std::string key;
    LoadEditModuleResultKind resultKind;
    std::chrono::steady_clock::time_point startLoading;
    std::chrono::steady_clock::time_point startParsing;
    std::chrono::steady_clock::time_point endParsing;
    std::chrono::steady_clock::time_point endLoading;
};

class SYMBOLS_API EditModuleCache
{
public:
    static void Init();
    static void Done();
    EditModuleCache(const EditModuleCache&) = delete;
    EditModuleCache(EditModuleCache&&) = delete;
    EditModuleCache& operator=(const EditModuleCache&) = delete;
    EditModuleCache& operator=(EditModuleCache&&) = delete;
    static EditModuleCache& Instance() { return *instance; }
    LoadEditModuleResult LoadEditModule(const std::string& projectFilePath, const std::string& backendStr, const std::string& configurationStr);
    Module* GetEditModule(const std::string& projectFilePath, const std::string& backendStr, const std::string& configurationStr);
private:
    static std::unique_ptr<EditModuleCache> instance;
    EditModuleCache();
    std::map<std::string, std::pair<Module*, std::string>> editModuleMap;
    std::vector<std::unique_ptr<Module>> editModules;
};

SYMBOLS_API void InitEditModuleCache();
SYMBOLS_API void DoneEditModuleCache();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_EDIT_MODULE_CACHE_INCLUDED
