// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
#define CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmajor { namespace symbols {

SYMBOLS_API void PrepareModuleForCompilation(Module* rootModule, const std::vector<std::string>& references, sngcm::ast::Target target);
SYMBOLS_API Module* GetModuleFromModuleCache(const std::string& moduleFilePath);
SYMBOLS_API void PutModuleToModuleCache(std::unique_ptr<Module>&& module);
SYMBOLS_API void InitModuleCache();
SYMBOLS_API void DoneModuleCache();
SYMBOLS_API void ResetModuleCache();
SYMBOLS_API void SetUseModuleCache(bool useModuleCache_);
SYMBOLS_API bool UseModuleCache();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
