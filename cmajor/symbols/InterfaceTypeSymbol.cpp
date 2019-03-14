// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::unicode;

InterfaceTypeSymbol::InterfaceTypeSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(SymbolType::interfaceTypeSymbol, span_, name_), copyConstructor(nullptr)
{
}

void InterfaceTypeSymbol::AddMember(Symbol* member)
{
    TypeSymbol::AddMember(member);
    if (member->GetSymbolType() == SymbolType::memberFunctionSymbol)
    {
        MemberFunctionSymbol* memberFunction = static_cast<MemberFunctionSymbol*>(member);
        memberFunction->SetImtIndex(memberFunctions.size());
        memberFunctions.push_back(memberFunction);
    }
}

void InterfaceTypeSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_)
    {
        collector->AddInterface(this);
    }
}

void InterfaceTypeSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be nothrow", GetSpan());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be throw", GetSpan());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "interface cannot be unit_test", GetSpan());
    }
}

void* InterfaceTypeSymbol::IrType(Emitter& emitter)
{
    void* localIrType = emitter.GetIrTypeByTypeId(TypeId());
    if (!localIrType)
    {
        std::vector<void*> elemTypes;
        elemTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        elemTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        localIrType = emitter.GetIrTypeForStructType(elemTypes);
        emitter.SetIrTypeByTypeId(TypeId(), localIrType);
    }
    return localIrType;
}

void* InterfaceTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    void* irType = IrType(emitter);
    std::vector<void*> arrayOfDefaults;
    arrayOfDefaults.push_back(emitter.CreateDefaultIrValueForVoidPtrType());
    arrayOfDefaults.push_back(emitter.CreateDefaultIrValueForVoidPtrType());
    return emitter.CreateDefaultIrValueForStruct(irType, arrayOfDefaults);
}

void InterfaceTypeSymbol::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, MemberFunctionSymbol* interfaceMemberFunction, const Span& span)
{
    TypeSymbol* type = static_cast<TypeSymbol*>(genObjects[0]->GetType());
    if (type->GetSymbolType() == SymbolType::interfaceTypeSymbol)
    {
        genObjects[0]->Load(emitter, OperationFlags::addr);
    }
    else
    {
        genObjects[0]->Load(emitter, OperationFlags::none);
    }
    void* interfaceTypePtr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectFromInterface(interfaceTypePtr);
    void* imtPtr = emitter.GetImtPtrFromInterface(interfaceTypePtr);
    void* callee = emitter.GetInterfaceMethod(imtPtr, interfaceMemberFunction->ImtIndex(), interfaceMemberFunction->IrType(emitter));
    int na = genObjects.size();
    for (int i = 1; i < na; ++i)
    {
        GenObject* genObject = genObjects[i];
        genObject->Load(emitter, flags & OperationFlags::functionCallFlags);
    }
    emitter.SetCurrentDebugLocation(span);
    std::vector<void*> args;
    int n = interfaceMemberFunction->Parameters().size();
    if (interfaceMemberFunction->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ++n;
    }
    args.resize(n);
    args[0] = objectPtr;
    for (int i = 0; i < n - 1; ++i)
    {
        void* arg = emitter.Stack().Pop();
        args[n - i - 1] = arg;
    }
    void* handlerBlock = emitter.HandlerBlock();
    void* cleanupBlock = emitter.CleanupBlock();
    bool newCleanupNeeded = emitter.NewCleanupNeeded();
    Pad* currentPad = emitter.CurrentPad();
    std::vector<void*> bundles;
    if (currentPad != nullptr)
    {
        bundles.push_back(currentPad->value);
    }
    if (interfaceMemberFunction->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol && !interfaceMemberFunction->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        if (IsNothrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.CreateCall(callee, args));
            }
            else
            {
                emitter.Stack().Push(emitter.CreateCallInst(callee, args, bundles, span));
            }
        }
        else
        {
            void* nextBlock = emitter.CreateBasicBlock("next");
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
                emitter.Stack().Push(emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span));
            }
            emitter.SetCurrentBasicBlock(nextBlock);
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
            void* nextBlock = emitter.CreateBasicBlock("next");
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
            emitter.SetCurrentBasicBlock(nextBlock);
        }
    }
}

void InterfaceTypeSymbol::Check()
{
    TypeSymbol::Check();
    for (MemberFunctionSymbol* memberFunction : memberFunctions)
    {
        if (!memberFunction)
        {
            throw SymbolCheckException(GetRootModuleForCurrentThread(), "interface type symbol contains null member function", GetSpan());
        }
    }
}

InterfaceTypeDefaultConstructor::InterfaceTypeDefaultConstructor(InterfaceTypeSymbol* interfaceType_, const Span& span_) : FunctionSymbol(span_, U"@interfaceDefaultCtor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(interfaceType_->AddPointer(span_));
    AddMember(thisParam);
    ComputeName();
}

void InterfaceTypeDefaultConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    TypeSymbol* type = static_cast<TypeSymbol*>(genObjects[0]->GetType());
    if (type->GetSymbolType() == SymbolType::interfaceTypeSymbol)
    {
        genObjects[0]->Load(emitter, OperationFlags::addr);
    }
    else
    {
        genObjects[0]->Load(emitter, OperationFlags::none);
    }
    void* interfaceTypePtr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectPtrFromInterface(interfaceTypePtr);
    emitter.CreateStore(emitter.CreateDefaultIrValueForVoidPtrType(), objectPtr);
    void* interfacePtr = emitter.GetImtPtrFromInterface(interfaceTypePtr);
    emitter.CreateStore(emitter.CreateDefaultIrValueForVoidPtrType(), interfacePtr);
}

InterfaceTypeCopyConstructor::InterfaceTypeCopyConstructor(InterfaceTypeSymbol* interfaceType_, const Span& span_) : FunctionSymbol(span_, U"@interfaceCopyCtor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(interfaceType_->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(interfaceType_->AddConst(span_)->AddLvalueReference(span_));
    AddMember(thatParam);
    ComputeName();
}

void InterfaceTypeCopyConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    TypeSymbol* type = static_cast<TypeSymbol*>(genObjects[0]->GetType());
    if (type->GetSymbolType() == SymbolType::interfaceTypeSymbol)
    {
        genObjects[0]->Load(emitter, OperationFlags::addr);
    }
    else
    {
        genObjects[0]->Load(emitter, OperationFlags::none);
    }
    void* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectPtrFromInterface(interfaceTypePtr);
    void* thatObjectPtr = emitter.GetObjectPtrFromInterface(thatPtr);
    void* thatObject = emitter.CreateLoad(thatObjectPtr);
    emitter.CreateStore(thatObject, objectPtr);
    void* interfacePtr = emitter.GetImtPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtr = emitter.GetImtPtrFromInterface(thatPtr);
    void* thatInterfaceObject = emitter.CreateLoad(thatInterfacePtr);
    emitter.CreateStore(thatInterfaceObject, interfacePtr);
}

InterfaceTypeMoveConstructor::InterfaceTypeMoveConstructor(InterfaceTypeSymbol* interfaceType_, const Span& span_) : FunctionSymbol(span_, U"@interfaceMoveCtor")
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(interfaceType_->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(interfaceType_->AddRvalueReference(span_));
    AddMember(thatParam);
    ComputeName();
}

void InterfaceTypeMoveConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    TypeSymbol* type = static_cast<TypeSymbol*>(genObjects[0]->GetType());
    if (type->GetSymbolType() == SymbolType::interfaceTypeSymbol)
    {
        genObjects[0]->Load(emitter, OperationFlags::addr);
    }
    else
    {
        genObjects[0]->Load(emitter, OperationFlags::none);
    }
    void* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectPtrFromInterface(interfaceTypePtr);
    void* thatObjectPtr = emitter.GetObjectPtrFromInterface(thatPtr);
    void* thatObject = emitter.CreateLoad(thatObjectPtr);
    emitter.CreateStore(thatObject, objectPtr);
    void* interfacePtr = emitter.GetImtPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtr = emitter.GetImtPtrFromInterface(thatPtr);
    void* thatInterfaceObject = emitter.CreateLoad(thatInterfacePtr);
    emitter.CreateStore(thatInterfaceObject, interfacePtr);
}

InterfaceTypeCopyAssignment::InterfaceTypeCopyAssignment(InterfaceTypeSymbol* interfaceType_, const Span& span_) : FunctionSymbol(span_, U"@interfaceCopyAssignment")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(interfaceType_->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(interfaceType_->AddConst(span_)->AddLvalueReference(span_));
    AddMember(thatParam);
    TypeSymbol* voidType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void");
    SetReturnType(voidType);
    ComputeName();
}

void InterfaceTypeCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    TypeSymbol* type = static_cast<TypeSymbol*>(genObjects[0]->GetType());
    if (type->GetSymbolType() == SymbolType::interfaceTypeSymbol)
    {
        genObjects[0]->Load(emitter, OperationFlags::addr);
    }
    else
    {
        genObjects[0]->Load(emitter, OperationFlags::none);
    }
    void* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectPtrFromInterface(interfaceTypePtr);
    void* thatObjectPtr = emitter.GetObjectPtrFromInterface(thatPtr);
    void* thatObject = emitter.CreateLoad(thatObjectPtr);
    emitter.CreateStore(thatObject, objectPtr);
    void* interfacePtr = emitter.GetImtPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtr = emitter.GetImtPtrFromInterface(thatPtr);
    void* thatInterfaceObject = emitter.CreateLoad(thatInterfacePtr);
    emitter.CreateStore(thatInterfaceObject, interfacePtr);
}

InterfaceTypeMoveAssignment::InterfaceTypeMoveAssignment(InterfaceTypeSymbol* interfaceType_, const Span& span_) : FunctionSymbol(span_, U"@interfaceMoveAssignment")
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(interfaceType_->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(interfaceType_->AddRvalueReference(span_));
    AddMember(thatParam);
    TypeSymbol* voidType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void");
    SetReturnType(voidType);
    ComputeName();
}

void InterfaceTypeMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    TypeSymbol* type = static_cast<TypeSymbol*>(genObjects[0]->GetType());
    if (type->GetSymbolType() == SymbolType::interfaceTypeSymbol)
    {
        genObjects[0]->Load(emitter, OperationFlags::addr);
    }
    else
    {
        genObjects[0]->Load(emitter, OperationFlags::none);
    }
    void* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* thatPtr = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectPtrFromInterface(interfaceTypePtr);
    void* thatObjectPtr = emitter.GetObjectPtrFromInterface(thatPtr);
    void* thatObject = emitter.CreateLoad(thatObjectPtr);
    emitter.CreateStore(thatObject, objectPtr);
    void* interfacePtr = emitter.GetImtPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtr = emitter.GetImtPtrFromInterface(thatPtr);
    void* thatInterfaceObject = emitter.CreateLoad(thatInterfacePtr);
    emitter.CreateStore(thatInterfaceObject, interfacePtr);
}

ClassToInterfaceConversion::ClassToInterfaceConversion(ClassTypeSymbol* sourceClassType_, InterfaceTypeSymbol* targetInterfaceType_, int32_t interfaceIndex_, const Span& span_) : 
    FunctionSymbol(span_, U"@classToInterfaceConversion"), sourceClassType(sourceClassType_), targetInterfaceType(targetInterfaceType_), interfaceIndex(interfaceIndex_)
{
    SetConversion();
}

std::vector<LocalVariableSymbol*> ClassToInterfaceConversion::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    temporaries.push_back(currentFunction->CreateTemporary(targetInterfaceType, Span()));
    return temporaries;
}

void ClassToInterfaceConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    void* classPtr = emitter.Stack().Pop();
    void* classPtrAsVoidPtr = emitter.CreateBitCast(classPtr, emitter.GetIrTypeForVoidPtrType());
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* temporaryInterfaceObjectVar = emitter.Stack().Pop();
    void* objectPtr = emitter.GetObjectPtrFromInterface(temporaryInterfaceObjectVar);
    emitter.CreateStore(classPtrAsVoidPtr, objectPtr);
    void* vmtObjectPtr = sourceClassType->VmtObject(emitter, false);
    void* imtArray = emitter.GetImtArray(vmtObjectPtr, GetImtsVmtIndexOffset());
    void* imt = emitter.GetImt(imtArray, interfaceIndex);
    void* imtPtr = emitter.GetImtPtrFromInterface(temporaryInterfaceObjectVar);
    emitter.CreateStore(imt, imtPtr);
    emitter.Stack().Push(temporaryInterfaceObjectVar);
}

void ClassToInterfaceConversion::Check()
{
    FunctionSymbol::Check();
    if (!sourceClassType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class to interface conversion has no source class type", GetSpan());
    }
    if (!targetInterfaceType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class to interface conversion has no target interface type", GetSpan());
    }
}

} } // namespace cmajor::symbols
