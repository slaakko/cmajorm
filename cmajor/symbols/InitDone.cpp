// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/Symbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/SymbolTable.hpp>

namespace cmajor { namespace symbols {

void Init()
{
    InitSymbol();
    InitFunctionSymbol();
    InitModule();
    InitModuleCache();
    InitSymbolTable();
}

void Done()
{
    DoneSymbolTable();
    DoneModuleCache();
    DoneModule();
    DoneFunctionSymbol();
    DoneSymbol();
}

} } // namespace cmajor::symbols
