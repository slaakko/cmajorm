// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Trap.hpp>
#include <cmajor/symbols/SymbolTable.hpp>

namespace cmajor { namespace symbols {

TrapFunction::TrapFunction(SymbolTable& symbolTable) : FunctionSymbol(SymbolType::trap, Span(), U"trap")
{
    SetGroupName(U"trap");
    SetCDecl();
    SetVarArg();
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* b0Param = new ParameterSymbol(Span(), U"b0");
    b0Param->SetType(symbolTable.GetTypeByName(U"byte"));
    AddMember(b0Param);
    ParameterSymbol* b1Param = new ParameterSymbol(Span(), U"b1");
    b1Param->SetType(symbolTable.GetTypeByName(U"byte"));
    AddMember(b1Param);
    ParameterSymbol* b2Param = new ParameterSymbol(Span(), U"b2");
    b2Param->SetType(symbolTable.GetTypeByName(U"byte"));
    AddMember(b2Param);
    SetReturnType(symbolTable.GetTypeByName(U"long"));
}

TrapFunction::TrapFunction(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::trap, span_, name_)
{
}

void TrapFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    int na = genObjects.size();
    for (int i = 0; i < na; ++i)
    {
        GenObject* genObject = genObjects[i];
        genObject->Load(emitter, flags & OperationFlags::functionCallFlags);
    }
    std::vector<void*> args;
    args.resize(na);
    for (int i = 0; i < na; ++i)
    {
        void* arg = emitter.Stack().Pop();
        args[na - i - 1] = arg;
    }
    emitter.Stack().Push(emitter.GenerateTrap(args));
}

void InitTrap(SymbolTable& symbolTable)
{
    symbolTable.AddFunctionSymbolToGlobalScope(new TrapFunction(symbolTable));
}

} } // namespace cmajor::symbols
