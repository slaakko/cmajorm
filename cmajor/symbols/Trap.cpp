// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Trap.hpp>
#include <cmajor/symbols/SymbolTable.hpp>

namespace cmajor { namespace symbols {

TrapFunction::TrapFunction(SymbolTable& symbolTable) : FunctionSymbol(SymbolType::trap, Span(), boost::uuids::nil_uuid(), U"trap")
{
    SetGroupName(U"trap");
    SetCDecl();
    SetVarArg();
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* b0Param = new ParameterSymbol(Span(), boost::uuids::nil_uuid(), U"b0");
    b0Param->SetType(symbolTable.GetTypeByName(U"byte"));
    AddMember(b0Param);
    ParameterSymbol* b1Param = new ParameterSymbol(Span(), boost::uuids::nil_uuid(), U"b1");
    b1Param->SetType(symbolTable.GetTypeByName(U"byte"));
    AddMember(b1Param);
    ParameterSymbol* b2Param = new ParameterSymbol(Span(), boost::uuids::nil_uuid(), U"b2");
    b2Param->SetType(symbolTable.GetTypeByName(U"byte"));
    AddMember(b2Param);
    SetReturnType(symbolTable.GetTypeByName(U"long"));
}

TrapFunction::TrapFunction(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : FunctionSymbol(SymbolType::trap, span_, sourceModuleId_, name_)
{
}

void TrapFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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
