// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_TRAP_INCLUDED
#define CMAJOR_SYMBOLS_TRAP_INCLUDED
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace symbols {

class SymbolTable;

class TrapFunction : public FunctionSymbol
{
public:
    TrapFunction(SymbolTable& symbolTable);
    TrapFunction(const Span& span_, const boost::uuids::uuid& sourceModuleId, const std::u32string& name_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
};

void InitTrap(SymbolTable& symbolTable);

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_TRAP_INCLUDED
