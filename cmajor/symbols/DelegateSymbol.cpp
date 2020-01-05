// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/DelegateSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

DelegateTypeSymbol::DelegateTypeSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(SymbolType::delegateTypeSymbol, span_, name_), returnType(), parameters()
{
}

void DelegateTypeSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    const boost::uuids::uuid& returnTypeId = returnType->TypeId();
    writer.GetBinaryWriter().Write(returnTypeId);
    bool hasReturnParam = returnParam != nullptr;
    writer.GetBinaryWriter().Write(hasReturnParam);
    if (hasReturnParam)
    {
        writer.Write(returnParam.get());
    }
}

void DelegateTypeSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    boost::uuids::uuid returnTypeId;
    reader.GetBinaryReader().ReadUuid(returnTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, returnTypeId, 0);
    bool hasReturnParam = reader.GetBinaryReader().ReadBool();
    if (hasReturnParam)
    {
        returnParam.reset(reader.ReadParameterSymbol(this));
    }
}

void DelegateTypeSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    Assert(index == 0, "invalid emplace type index");
    returnType = typeSymbol;
}

void DelegateTypeSymbol::AddMember(Symbol* member)
{
    TypeSymbol::AddMember(member);
    if (member->GetSymbolType() == SymbolType::parameterSymbol)
    {
        parameters.push_back(static_cast<ParameterSymbol*>(member));
    }
}

std::string DelegateTypeSymbol::Syntax() const
{
    std::string syntax = GetSpecifierStr();
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    syntax.append("delegate ");
    syntax.append(ToUtf8(ReturnType()->DocName()));
    syntax.append(1, ' ');
    syntax.append(ToUtf8(DocName()));
    syntax.append(1, '(');
    bool first = true;
    for (ParameterSymbol* param : parameters)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            syntax.append(", ");
        }
        syntax.append(ToUtf8(param->GetType()->DocName()));
        syntax.append(1, ' ');
        syntax.append(ToUtf8(param->DocName()));
    }
    syntax.append(");");
    return syntax;
}

std::u32string DelegateTypeSymbol::Id() const
{
    return MangledName();
}

void DelegateTypeSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_)
    {
        collector->AddDelegate(this);
    }
}

void DelegateTypeSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    formatter.WriteLine("typeid: " + boost::uuids::to_string(TypeId()));
}

void* DelegateTypeSymbol::IrType(Emitter& emitter)
{
    void* localIrType = emitter.GetIrTypeByTypeId(TypeId());
    if (!localIrType)
    {
        void* retType = emitter.GetIrTypeForVoid();
        if (!returnType->IsVoidType() && !ReturnsClassInterfaceOrClassDelegateByValue())
        {
            retType = returnType->IrType(emitter);
        }
        std::vector<void*> paramTypes;
        int np = parameters.size();
        for (int i = 0; i < np; ++i)
        {
            ParameterSymbol* parameter = parameters[i];
            paramTypes.push_back(parameter->GetType()->IrType(emitter));
        }
        if (returnParam)
        {
            paramTypes.push_back(returnParam->GetType()->IrType(emitter));
        }
        localIrType = emitter.GetIrTypeForDelegateType(retType, paramTypes);
        emitter.SetIrTypeByTypeId(TypeId(), localIrType);
    }
    return localIrType;
}

void* DelegateTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    return emitter.CreateDefaultIrValueForDelegateType(IrType(emitter));
}

void DelegateTypeSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be throw and nothrow at the same time", GetSpan());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "delegate cannot be unit_test", GetSpan());
    }
}

bool DelegateTypeSymbol::ReturnsClassInterfaceOrClassDelegateByValue() const
{
    return returnType->IsClassTypeSymbol() || returnType->GetSymbolType() == SymbolType::classDelegateTypeSymbol || returnType->GetSymbolType() == SymbolType::interfaceTypeSymbol;
}

void DelegateTypeSymbol::SetReturnParam(ParameterSymbol* returnParam_)
{
    returnParam.reset(returnParam_);
}

void DelegateTypeSymbol::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    void* callee = nullptr;
    int na = genObjects.size();
    for (int i = 0; i < na; ++i)
    {
        GenObject* genObject = genObjects[i];
        genObject->Load(emitter, flags & OperationFlags::functionCallFlags);
        if (i == 0)
        {
            callee = emitter.Stack().Pop();
        }
    }
    std::vector<void*> args;
    int n = parameters.size();
    if (ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ++n;
    }
    args.resize(n);
    for (int i = 0; i < n; ++i)
    {
        void* arg = emitter.Stack().Pop();
        args[n - i - 1] = arg;
    }
    emitter.SetCurrentDebugLocation(span);
    void* handlerBlock = emitter.HandlerBlock();
    void* cleanupBlock = emitter.CleanupBlock();
    bool newCleanupNeeded = emitter.NewCleanupNeeded();
    Pad* currentPad = emitter.CurrentPad();
    std::vector<void*> bundles;
    if (currentPad != nullptr)
    {
        bundles.push_back(currentPad->value);
    }
    if (returnType->GetSymbolType() != SymbolType::voidTypeSymbol && !ReturnsClassInterfaceOrClassDelegateByValue())
    {
        if (IsNothrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.CreateCall(callee, args));
            }
            else
            {
                void* callInst = emitter.CreateCallInst(callee, args, bundles, span);
                emitter.Stack().Push(callInst);
            }
        }
        else
        {
            void* nextBlock = nullptr;
            if (GetBackEnd() == BackEnd::llvm) 
            {
                nextBlock = emitter.CreateBasicBlock("next");
            }
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            void* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.CreateInvoke(callee, nextBlock, unwindBlock, args));
            }
            else
            {
                void* invokeInst = emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span);
                emitter.Stack().Push(invokeInst);
            }
            if (GetBackEnd() == BackEnd::llvm)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
    else
    {
        if (IsNothrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.CreateCall(callee, args);
            }
            else
            {
                emitter.CreateCallInst(callee, args, bundles, span);
            }
        }
        else
        {
            void* nextBlock = nullptr;
            if (GetBackEnd() == BackEnd::llvm)
            {
                nextBlock = emitter.CreateBasicBlock("next");
            }
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            void* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.CreateInvoke(callee, nextBlock, unwindBlock, args);
            }
            else
            {
                emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span);
            }
            if (GetBackEnd() == BackEnd::llvm)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
}

void DelegateTypeSymbol::Check()
{
    TypeSymbol::Check();
    if (!returnType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "delegate type symbol has no return type", GetSpan());
    }

}

DelegateTypeDefaultConstructor::DelegateTypeDefaultConstructor(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::delegateTypeDefaultConstructor, span_, name_)
{
}

DelegateTypeDefaultConstructor::DelegateTypeDefaultConstructor(DelegateTypeSymbol* delegateType_) : 
    FunctionSymbol(SymbolType::delegateTypeDefaultConstructor, Span(), U"@constructor"), delegateType(delegateType_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(delegateType->AddPointer(Span()));
    AddMember(thisParam);
    ComputeName();
}

void DelegateTypeDefaultConstructor::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(delegateType->TypeId());
}

void DelegateTypeDefaultConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void DelegateTypeDefaultConstructor::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::delegateTypeSymbol, "delegate type symbol expected");
        delegateType = static_cast<DelegateTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void DelegateTypeDefaultConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "default constructor needs one object");
    emitter.Stack().Push(delegateType->CreateDefaultIrValue(emitter));
    genObjects[0]->Store(emitter, OperationFlags::none);
}

void DelegateTypeDefaultConstructor::Check()
{
    FunctionSymbol::Check();
    if (!delegateType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "delegate type default constructor has no delegate type", GetSpan());
    }
}

DelegateTypeCopyConstructor::DelegateTypeCopyConstructor(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::delegateTypeCopyConstructor, span_, name_)
{
}

DelegateTypeCopyConstructor::DelegateTypeCopyConstructor(DelegateTypeSymbol* delegateType) : FunctionSymbol(SymbolType::delegateTypeCopyConstructor, Span(), U"@constructor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(delegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(delegateType);
    AddMember(thatParam);
    ComputeName();
}

void DelegateTypeCopyConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

DelegateTypeMoveConstructor::DelegateTypeMoveConstructor(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::delegateTypeMoveConstructor, span_, name_)
{
}

DelegateTypeMoveConstructor::DelegateTypeMoveConstructor(DelegateTypeSymbol* delegateType) : FunctionSymbol(SymbolType::delegateTypeMoveConstructor, Span(), U"@constructor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(delegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(delegateType->AddRvalueReference(Span()));
    AddMember(thatParam);
    ComputeName();
}

void DelegateTypeMoveConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "move constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    genObjects[0]->Store(emitter, OperationFlags::none);
}

DelegateTypeCopyAssignment::DelegateTypeCopyAssignment(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::delegateTypeCopyAssignment, span_, name_)
{
}

DelegateTypeCopyAssignment::DelegateTypeCopyAssignment(DelegateTypeSymbol* delegateType, TypeSymbol* voidType) : FunctionSymbol(SymbolType::delegateTypeCopyAssignment, Span(), U"operator=")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(delegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(delegateType);
    AddMember(thatParam);
    SetReturnType(voidType);
    ComputeName();
}

void DelegateTypeCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

DelegateTypeMoveAssignment::DelegateTypeMoveAssignment(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::delegateTypeMoveAssignment, span_, name_)
{
}

DelegateTypeMoveAssignment::DelegateTypeMoveAssignment(DelegateTypeSymbol* delegateType, TypeSymbol* voidType) : FunctionSymbol(SymbolType::delegateTypeMoveAssignment, Span(), U"operator=")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(delegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(delegateType->AddRvalueReference(Span()));
    AddMember(thatParam);
    SetReturnType(voidType);
    ComputeName();
}

void DelegateTypeMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "move assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    genObjects[0]->Store(emitter, OperationFlags::none);
}

DelegateTypeReturn::DelegateTypeReturn(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::delegateTypeReturn, span_, name_)
{
}

DelegateTypeReturn::DelegateTypeReturn(DelegateTypeSymbol* delegateType) : FunctionSymbol(SymbolType::delegateTypeReturn, Span(), U"@return")
{
    SetGroupName(U"@return");
    ParameterSymbol* valueParam = new ParameterSymbol(Span(), U"value");
    valueParam->SetType(delegateType);
    AddMember(valueParam);
    SetReturnType(delegateType);
    ComputeName();
}

void DelegateTypeReturn::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "return needs one object");
    genObjects[0]->Load(emitter, OperationFlags::none);
}

DelegateTypeEquality::DelegateTypeEquality(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::delegateTypeEquality, span_, name_)
{
}

DelegateTypeEquality::DelegateTypeEquality(DelegateTypeSymbol* delegateType, TypeSymbol* boolType) : FunctionSymbol(SymbolType::delegateTypeEquality, Span(), U"operator==")
{
    SetGroupName(U"operator==");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(Span(), U"left");
    leftParam->SetType(delegateType);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(Span(), U"right");
    rightParam->SetType(delegateType);
    AddMember(rightParam);
    SetReturnType(boolType);
    ComputeName();
}

void DelegateTypeEquality::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "operator== needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* left = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* right = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateICmpEQ(left, right));
}

FunctionToDelegateConversion::FunctionToDelegateConversion(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::functionToDelegateSymbol, span_, name_)
{
}

FunctionToDelegateConversion::FunctionToDelegateConversion(TypeSymbol* sourceType_, TypeSymbol* targetType_, FunctionSymbol* function_) :
    FunctionSymbol(SymbolType::functionToDelegateSymbol, Span(), U"@conversion"), sourceType(sourceType_), targetType(targetType_), function(function_)
{
    SetConversion();
}

void FunctionToDelegateConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    emitter.Stack().Pop();
    emitter.Stack().Push(emitter.GetOrInsertFunction(ToUtf8(function->MangledName()), function->IrType(emitter)));
}

void FunctionToDelegateConversion::Check()
{
    FunctionSymbol::Check();
    if (!sourceType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "function to delegate conversion has no source type", GetSpan());
    }
    if (!targetType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "function to delegate conversion has no target type", GetSpan());
    }
    if (!function)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "function to delegate conversion has no function", GetSpan());
    }
}

DelegateToVoidPtrConversion::DelegateToVoidPtrConversion(TypeSymbol* delegateType_, TypeSymbol* voidPtrType_) :
    FunctionSymbol(Span(), U"dlg2voidptr"), delegateType(delegateType_), voidPtrType(voidPtrType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
}

void DelegateToVoidPtrConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateBitCast(value, voidPtrType->IrType(emitter)));
}

VoidPtrToDelegateConversion::VoidPtrToDelegateConversion(TypeSymbol* voidPtrType_, TypeSymbol* delegateType_, TypeSymbol* ulongType_) :
    FunctionSymbol(Span(), U"voidptr2dlg"), voidPtrType(voidPtrType_), delegateType(delegateType_), ulongType(ulongType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
}

void VoidPtrToDelegateConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    void* ulongValue = emitter.CreatePtrToInt(value, ulongType->IrType(emitter));
    emitter.Stack().Push(emitter.CreateIntToPtr(ulongValue, delegateType->IrType(emitter)));
}

ClassDelegateTypeSymbol::ClassDelegateTypeSymbol(const Span& span_, const std::u32string& name_) : 
    TypeSymbol(SymbolType::classDelegateTypeSymbol, span_, name_), returnType(nullptr), parameters(), delegateType(nullptr), objectDelegatePairType(nullptr), 
    copyConstructor(nullptr)
{
}

void ClassDelegateTypeSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    const boost::uuids::uuid& returnTypeId = returnType->TypeId();
    writer.GetBinaryWriter().Write(returnTypeId);
}

void ClassDelegateTypeSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    boost::uuids::uuid returnTypeId;
    reader.GetBinaryReader().ReadUuid(returnTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, returnTypeId, -1);
}

void ClassDelegateTypeSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == -1)
    {
        returnType = typeSymbol;
    }
    else
    {
        TypeSymbol::EmplaceType(typeSymbol, index);
    }
}

void ClassDelegateTypeSymbol::AddMember(Symbol* member)
{
    TypeSymbol::AddMember(member);
    if (member->GetSymbolType() == SymbolType::parameterSymbol)
    {
        parameters.push_back(static_cast<ParameterSymbol*>(member));
    }
    else if (member->GetSymbolType() == SymbolType::delegateTypeSymbol)
    {
        delegateType = static_cast<DelegateTypeSymbol*>(member);
    }
    else if (member->GetSymbolType() == SymbolType::classTypeSymbol)
    {
        objectDelegatePairType = static_cast<ClassTypeSymbol*>(member);
    }
    else if (member->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(member);
        if (functionSymbol->IsClassDelegateCopyConstructor())
        {
            copyConstructor = functionSymbol;
        }
    }
}

std::string ClassDelegateTypeSymbol::Syntax() const
{
    std::string syntax = GetSpecifierStr();
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    syntax.append("class delegate ");
    syntax.append(ToUtf8(ReturnType()->DocName()));
    syntax.append(1, ' ');
    syntax.append(ToUtf8(DocName()));
    syntax.append(1, '(');
    bool first = true;
    for (ParameterSymbol* param : parameters)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            syntax.append(", ");
        }
        syntax.append(ToUtf8(param->GetType()->DocName()));
        syntax.append(1, ' ');
        syntax.append(ToUtf8(param->DocName()));
    }
    syntax.append(");");
    return syntax;
}

std::u32string ClassDelegateTypeSymbol::Id() const
{
    return MangledName();
}

void ClassDelegateTypeSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_)
    {
        collector->AddClassDelegate(this);
    }
}

void ClassDelegateTypeSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    formatter.WriteLine("typeid: " + boost::uuids::to_string(TypeId()));
}

void* ClassDelegateTypeSymbol::IrType(Emitter& emitter)
{
    void* localIrType = emitter.GetIrTypeByTypeId(TypeId());
    if (!localIrType)
    {
        std::vector<void*> elementTypes;
        elementTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        elementTypes.push_back(delegateType->IrType(emitter));
        localIrType = emitter.GetIrTypeForStructType(elementTypes);
        emitter.SetIrTypeByTypeId(TypeId(), localIrType);
    }
    return localIrType;
}

void* ClassDelegateTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    std::vector<void*> constants;
    constants.push_back(emitter.CreateDefaultIrValueForVoidPtrType());
    constants.push_back(delegateType->CreateDefaultIrValue(emitter));
    return emitter.CreateDefaultIrValueForStruct(IrType(emitter), constants);
}

bool ClassDelegateTypeSymbol::ReturnsClassInterfaceOrClassDelegateByValue() const
{
    return returnType->IsClassTypeSymbol() || returnType->GetSymbolType() == SymbolType::classDelegateTypeSymbol || returnType->GetSymbolType() == SymbolType::interfaceTypeSymbol;
}

void ClassDelegateTypeSymbol::SetReturnParam(ParameterSymbol* returnParam_)
{
    returnParam.reset(returnParam_);
}

void ClassDelegateTypeSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be throw and nothrow at the same time", GetSpan());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class delegate cannot be unit_test", GetSpan());
    }
}

void ClassDelegateTypeSymbol::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(!genObjects.empty(), "gen objects is empty");
    genObjects[0]->Load(emitter, flags);
    void* classDelegatePtr = emitter.Stack().Pop();
    void* delegatePtr = emitter.GetDelegateFromClassDelegate(classDelegatePtr);
    void* callee = emitter.CreateLoad(delegatePtr);
    NativeValue calleeValue(callee);
    void* objectPtr = emitter.GetObjectFromClassDelegate(classDelegatePtr);
    void* object = emitter.CreateLoad(objectPtr);
    NativeValue objectValue(object);
    std::vector<GenObject*> classDelegateCallObjects;
    classDelegateCallObjects.push_back(&calleeValue);
    classDelegateCallObjects.push_back(&objectValue);
    int na = genObjects.size();
    for (int i = 1; i < na; ++i)
    {
        GenObject* genObject = genObjects[i];
        classDelegateCallObjects.push_back(genObject);
    }
    delegateType->GenerateCall(emitter, classDelegateCallObjects, flags, span);
}

void ClassDelegateTypeSymbol::Check()
{
    TypeSymbol::Check();
    if (!returnType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class delegate type symbol has no return type", GetSpan());
    }
    if (!delegateType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class delegate type symbol has no delegate type", GetSpan());
    }
    if (!objectDelegatePairType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class delegate type symbol has no object delegate pair type", GetSpan());
    }
    if (!copyConstructor)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class delegate type symbol has no copy constructor", GetSpan());
    }
}

ClassDelegateTypeDefaultConstructor::ClassDelegateTypeDefaultConstructor(const Span& span_, const std::u32string& name_) : 
    FunctionSymbol(SymbolType::classDelegateTypeDefaultConstructor, span_, name_)
{
}

ClassDelegateTypeDefaultConstructor::ClassDelegateTypeDefaultConstructor(ClassDelegateTypeSymbol* classDelegateType_) : 
    FunctionSymbol(SymbolType::classDelegateTypeDefaultConstructor, Span(), U"@constructor"), classDelegateType(classDelegateType_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(classDelegateType->AddPointer(Span()));
    AddMember(thisParam);
    ComputeName();
}

void ClassDelegateTypeDefaultConstructor::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(classDelegateType->TypeId());
}

void ClassDelegateTypeDefaultConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void ClassDelegateTypeDefaultConstructor::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::classDelegateTypeSymbol, "class delegate type symbol expected");
        classDelegateType = static_cast<ClassDelegateTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void ClassDelegateTypeDefaultConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "default constructor needs one object");
    void* objectValue = emitter.CreateDefaultIrValueForVoidPtrType();
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* ptr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectFromClassDelegate(ptr);
    emitter.CreateStore(objectValue, objectPtr);
    void* delegateValue = classDelegateType->DelegateType()->CreateDefaultIrValue(emitter);
    void* delegatePtr = emitter.GetDelegateFromClassDelegate(ptr);
    emitter.CreateStore(delegateValue, delegatePtr);
}

void ClassDelegateTypeDefaultConstructor::Check()
{
    FunctionSymbol::Check();
    if (!classDelegateType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class delegate type default constructor has no class delegate type", GetSpan());
    }
}

ClassDelegateTypeCopyConstructor::ClassDelegateTypeCopyConstructor(const Span& span_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::classDelegateTypeCopyConstructor, span_, name_)
{
}

ClassDelegateTypeCopyConstructor::ClassDelegateTypeCopyConstructor(ClassDelegateTypeSymbol* classDelegateType) :
    FunctionSymbol(SymbolType::classDelegateTypeCopyConstructor, Span(), U"@constructor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(classDelegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(classDelegateType->AddConst(Span())->AddLvalueReference(Span()));
    AddMember(thatParam);
    ComputeName();
}

void ClassDelegateTypeCopyConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* thatObjectPtr = emitter.GetObjectFromClassDelegate(thatPtr);
    void* objectValue = emitter.CreateLoad(thatObjectPtr);
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* thisPtr = emitter.Stack().Pop();
    void* thisObjectPtr = emitter.GetObjectFromClassDelegate(thisPtr);
    emitter.CreateStore(objectValue, thisObjectPtr);
    void* thatDelegatePtr = emitter.GetDelegateFromClassDelegate(thatPtr);
    void* delegateValue = emitter.CreateLoad(thatDelegatePtr);
    void* thisDelegatePtr = emitter.GetDelegateFromClassDelegate(thisPtr);
    emitter.CreateStore(delegateValue, thisDelegatePtr);
}

ClassDelegateTypeMoveConstructor::ClassDelegateTypeMoveConstructor(const Span& span_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::classDelegateTypeMoveConstructor, span_, name_)
{
}

ClassDelegateTypeMoveConstructor::ClassDelegateTypeMoveConstructor(ClassDelegateTypeSymbol* classDelegateType) :
    FunctionSymbol(SymbolType::classDelegateTypeCopyConstructor, Span(), U"@constructor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(classDelegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(classDelegateType->AddRvalueReference(Span()));
    AddMember(thatParam);
    ComputeName();
}

void ClassDelegateTypeMoveConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* thatObjectPtr = emitter.GetObjectFromClassDelegate(thatPtr);
    void* objectValue = emitter.CreateLoad(thatObjectPtr);
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* thisPtr = emitter.Stack().Pop();
    void* thisObjectPtr = emitter.GetObjectFromClassDelegate(thisPtr);
    emitter.CreateStore(objectValue, thisObjectPtr);
    void* thatDelegatePtr = emitter.GetDelegateFromClassDelegate(thatPtr);
    void* delegateValue = emitter.CreateLoad(thatDelegatePtr);
    void* thisDelegatePtr = emitter.GetDelegateFromClassDelegate(thisPtr);
    emitter.CreateStore(delegateValue, thisDelegatePtr);
}

ClassDelegateTypeCopyAssignment::ClassDelegateTypeCopyAssignment(const Span& span_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::classDelegateTypeCopyAssignment, span_, name_)
{
}

ClassDelegateTypeCopyAssignment::ClassDelegateTypeCopyAssignment(ClassDelegateTypeSymbol* classDelegateType, TypeSymbol* voidType) :
    FunctionSymbol(SymbolType::classDelegateTypeCopyAssignment, Span(), U"operator=")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(classDelegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(classDelegateType->AddConst(Span())->AddLvalueReference(Span()));
    AddMember(thatParam);
    SetReturnType(voidType);
    ComputeName();
}

void ClassDelegateTypeCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* thatObjectPtr = emitter.GetObjectFromClassDelegate(thatPtr);
    void* objectValue = emitter.CreateLoad(thatObjectPtr);
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* thisPtr = emitter.Stack().Pop();
    void* thisObjectPtr = emitter.GetObjectFromClassDelegate(thisPtr);
    emitter.CreateStore(objectValue, thisObjectPtr);
    void* thatDelegatePtr = emitter.GetDelegateFromClassDelegate(thatPtr);
    void* delegateValue = emitter.CreateLoad(thatDelegatePtr);
    void* thisDelegatePtr = emitter.GetDelegateFromClassDelegate(thisPtr);
    emitter.CreateStore(delegateValue, thisDelegatePtr);
}

ClassDelegateTypeMoveAssignment::ClassDelegateTypeMoveAssignment(const Span& span_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::classDelegateTypeMoveAssignment, span_, name_)
{
}

ClassDelegateTypeMoveAssignment::ClassDelegateTypeMoveAssignment(ClassDelegateTypeSymbol* classDelegateType, TypeSymbol* voidType) :
    FunctionSymbol(SymbolType::classDelegateTypeMoveAssignment, Span(), U"operator=")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), U"this");
    thisParam->SetType(classDelegateType->AddPointer(Span()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(Span(), U"that");
    thatParam->SetType(classDelegateType->AddRvalueReference(Span()));
    AddMember(thatParam);
    SetReturnType(voidType);
    ComputeName();
}

void ClassDelegateTypeMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* thatObjectPtr = emitter.GetObjectFromClassDelegate(thatPtr);
    void* objectValue = emitter.CreateLoad(thatObjectPtr);
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* thisPtr = emitter.Stack().Pop();
    void* thisObjectPtr = emitter.GetObjectFromClassDelegate(thisPtr);
    emitter.CreateStore(objectValue, thisObjectPtr);
    void* thatDelegatePtr = emitter.GetDelegateFromClassDelegate(thatPtr);
    void* delegateValue = emitter.CreateLoad(thatDelegatePtr);
    void* thisDelegatePtr = emitter.GetDelegateFromClassDelegate(thisPtr);
    emitter.CreateStore(delegateValue, thisDelegatePtr);
}

ClassDelegateTypeEquality::ClassDelegateTypeEquality(const Span& span_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::classDelegateTypeEquality, span_, name_)
{
}

ClassDelegateTypeEquality::ClassDelegateTypeEquality(ClassDelegateTypeSymbol* classDelegateType, TypeSymbol* boolType) :
    FunctionSymbol(SymbolType::classDelegateTypeEquality, Span(), U"operator==")
{
    SetGroupName(U"operator==");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(Span(), U"left");
    leftParam->SetType(classDelegateType->AddConst(Span())->AddLvalueReference(Span()));
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(Span(), U"right");
    rightParam->SetType(classDelegateType->AddConst(Span())->AddLvalueReference(Span()));
    AddMember(rightParam);
    SetReturnType(boolType);
    ComputeName();
}

void ClassDelegateTypeEquality::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* leftPtr = emitter.Stack().Pop();
    void* leftObjectPtr = emitter.GetObjectFromClassDelegate(leftPtr);
    void* leftObjectValue = emitter.CreateLoad(leftObjectPtr);
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rightPtr = emitter.Stack().Pop();
    void* rightObjectPtr = emitter.GetObjectFromClassDelegate(rightPtr);
    void* rightObjectValue = emitter.CreateLoad(rightObjectPtr);
    void* objectsEqual = emitter.CreateICmpEQ(leftObjectValue, rightObjectValue);
    void* leftDelegatePtr = emitter.GetDelegateFromClassDelegate(leftPtr);
    void* leftDelegateValue = emitter.CreateLoad(leftDelegatePtr);
    void* rightDelegatePtr = emitter.GetDelegateFromClassDelegate(rightPtr);
    void* rightDelegateValue = emitter.CreateLoad(rightDelegatePtr);
    void* delegatesEqual = emitter.CreateICmpEQ(leftDelegateValue, rightDelegateValue);
    void* equal = emitter.CreateAnd(objectsEqual, delegatesEqual);
    emitter.Stack().Push(equal);
}

MemberFunctionToClassDelegateConversion::MemberFunctionToClassDelegateConversion(const Span& span_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::memberFunctionToClassDelegateSymbol, span_, name_)
{
}

MemberFunctionToClassDelegateConversion::MemberFunctionToClassDelegateConversion(const Span& span_, TypeSymbol* sourceType_, ClassDelegateTypeSymbol* targetType_, FunctionSymbol* function_) :
    FunctionSymbol(SymbolType::memberFunctionToClassDelegateSymbol, span_, U"@conversion"), sourceType(sourceType_), targetType(targetType_), function(function_)
{
    SetConversion();
}

std::vector<LocalVariableSymbol*> MemberFunctionToClassDelegateConversion::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    LocalVariableSymbol* objectDelegatePairVariable = currentFunction->CreateTemporary(targetType->ObjectDelegatePairType(), Span());
    temporaries.push_back(objectDelegatePairVariable);
    return temporaries;
}

void MemberFunctionToClassDelegateConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "MemberFunctionToClassDelegateConversion needs one temporary object");
    void* objectValue = emitter.Stack().Pop();
    if (!objectValue)
    {
        throw Exception(GetRootModuleForCurrentThread(), "cannot construct class delegate because expression has no this pointer", GetSpan());
    }
    void* objectValueAsVoidPtr = emitter.CreateBitCast(objectValue, emitter.GetIrTypeForVoidPtrType());
    void* memFunPtrValue = emitter.GetOrInsertFunction(ToUtf8(function->MangledName()), function->IrType(emitter));
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* ptr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectFromClassDelegate(ptr);
    emitter.CreateStore(objectValueAsVoidPtr, objectPtr);
    void* delegatePtr = emitter.GetDelegateFromClassDelegate(ptr);
    void* delegateValue = emitter.CreateBitCast(memFunPtrValue, targetType->DelegateType()->IrType(emitter));
    emitter.CreateStore(delegateValue, delegatePtr);
    emitter.Stack().Push(ptr);
}

void MemberFunctionToClassDelegateConversion::Check()
{
    FunctionSymbol::Check();
    if (!sourceType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "member function to class delegate conversion has no source type", GetSpan());
    }
    if (!targetType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "member function to class delegate conversion has no target type", GetSpan());
    }
    if (!function)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "member function to class delegate conversion has no function", GetSpan());
    }
}

} } // namespace cmajor::symbols
