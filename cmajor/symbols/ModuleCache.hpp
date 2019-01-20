// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
#define CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
#include <cmajor/symbols/Module.hpp>

namespace cmajor { namespace symbols {

void PrepareModuleForCompilation(Module* rootModule, const std::vector<std::string>& references);
Module* GetModuleFromModuleCache(const std::string& moduleFilePath);
void PutModuleToModuleCache(std::unique_ptr<Module>&& module);
void InitModuleCache();
void DoneModuleCache();
void ResetModuleCache();
void SetUseModuleCache(bool useModuleCache_);
bool UseModuleCache();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_MODULE_CACHE_INCLUDED
