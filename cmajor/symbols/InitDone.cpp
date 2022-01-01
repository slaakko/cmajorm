// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/Symbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/EditModuleCache.hpp>
#include <cmajor/symbols/SourceFileModuleMap.hpp>
#include <cmajor/symbols/Sources.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/KeywordSymbol.hpp>

namespace cmajor { namespace symbols {

void Init()
{
    InitSymbol();
    InitFunctionSymbol();
    InitModule();
    InitModuleCache();
    InitSymbolTable();
    InitSourceFileModuleMap();
    InitEditModuleCache();
    InitKeywordSymbol();
    InitSources();
}

void Done()
{
    DoneSources();
    DoneKeywordSymbol();
    DoneEditModuleCache();
    DoneSourceFileModuleMap();
    DoneSymbolTable();
    DoneModuleCache();
    DoneModule();
    DoneFunctionSymbol();
    DoneSymbol();
}

} } // namespace cmajor::symbols

