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

llvm::Type* InterfaceTypeSymbol::IrType(Emitter& emitter)
{
    llvm::Type* localIrType = emitter.GetIrTypeByTypeId(TypeId());
    if (!localIrType)
    {
        std::vector<llvm::Type*> elemTypes;
        elemTypes.push_back(emitter.Builder().getInt8PtrTy());
        elemTypes.push_back(emitter.Builder().getInt8PtrTy());
        localIrType = llvm::StructType::get(emitter.Context(), elemTypes);
        emitter.SetIrTypeByTypeId(TypeId(), localIrType);
    }
    return localIrType;
}

llvm::Constant* InterfaceTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    llvm::Type* irType = IrType(emitter);
    std::vector<llvm::Constant*> arrayOfDefaults;
    arrayOfDefaults.push_back(llvm::Constant::getNullValue(emitter.Builder().getInt8PtrTy()));
    arrayOfDefaults.push_back(llvm::Constant::getNullValue(emitter.Builder().getInt8PtrTy()));
    return llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(irType), arrayOfDefaults);
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
    llvm::Value* interfaceTypePtr = emitter.Stack().Pop();
    ArgVector objectIndeces;
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    llvm::Value* objectPtrPtr = emitter.Builder().CreateGEP(interfaceTypePtr, objectIndeces);
    llvm::Value* objectPtr = emitter.Builder().CreateLoad(objectPtrPtr);
    ArgVector interfaceIndeces;
    interfaceIndeces.push_back(emitter.Builder().getInt32(0));
    interfaceIndeces.push_back(emitter.Builder().getInt32(1));
    llvm::Value* interfacePtrPtr = emitter.Builder().CreateGEP(interfaceTypePtr, interfaceIndeces);
    llvm::Value* interfacePtr = emitter.Builder().CreateLoad(interfacePtrPtr);
    llvm::Value* imtPtr = emitter.Builder().CreateBitCast(interfacePtr, llvm::PointerType::get(emitter.Builder().getInt8PtrTy(), 0));
    ArgVector methodIndeces;
    methodIndeces.push_back(emitter.Builder().getInt32(interfaceMemberFunction->ImtIndex()));
    llvm::Value* methodPtrPtr = emitter.Builder().CreateGEP(imtPtr, methodIndeces);
    llvm::Value* methodPtr = emitter.Builder().CreateLoad(methodPtrPtr);
    llvm::Value* callee = emitter.Builder().CreateBitCast(methodPtr, llvm::PointerType::get(interfaceMemberFunction->IrType(emitter), 0));
    int na = genObjects.size();
    for (int i = 1; i < na; ++i)
    {
        GenObject* genObject = genObjects[i];
        genObject->Load(emitter, flags & OperationFlags::functionCallFlags);
    }
    emitter.SetCurrentDebugLocation(span);
    ArgVector args;
    int n = interfaceMemberFunction->Parameters().size();
    if (interfaceMemberFunction->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ++n;
    }
    args.resize(n);
    args[0] = objectPtr;
    for (int i = 0; i < n - 1; ++i)
    {
        llvm::Value* arg = emitter.Stack().Pop();
        args[n - i - 1] = arg;
    }
    llvm::BasicBlock* handlerBlock = emitter.HandlerBlock();
    llvm::BasicBlock* cleanupBlock = emitter.CleanupBlock();
    bool newCleanupNeeded = emitter.NewCleanupNeeded();
    Pad* currentPad = emitter.CurrentPad();
    std::vector<llvm::OperandBundleDef> bundles;
    if (currentPad != nullptr)
    {
        std::vector<llvm::Value*> inputs;
        inputs.push_back(currentPad->value);
        bundles.push_back(llvm::OperandBundleDef("funclet", inputs));
    }
    if (interfaceMemberFunction->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol && !interfaceMemberFunction->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        if (IsNothrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.Builder().CreateCall(callee, args));
            }
            else
            {
                llvm::CallInst* callInst = llvm::CallInst::Create(callee, args, bundles, "", emitter.CurrentBasicBlock());
                if (emitter.DIBuilder())
                {
                    callInst->setDebugLoc(emitter.GetDebugLocation(span));
                }
                emitter.Stack().Push(callInst);
            }
        }
        else
        {
            llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(emitter.Context(), "next", emitter.Function());
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            llvm::BasicBlock* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.Builder().CreateInvoke(callee, nextBlock, unwindBlock, args));
            }
            else
            {
                llvm::InvokeInst* invokeInst = llvm::InvokeInst::Create(callee, nextBlock, unwindBlock, args, bundles, "", emitter.CurrentBasicBlock());
                if (emitter.DIBuilder())
                {
                    invokeInst->setDebugLoc(emitter.GetDebugLocation(span));
                }
                emitter.Stack().Push(invokeInst);
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
                emitter.Builder().CreateCall(callee, args);
            }
            else
            {
                llvm::CallInst* callInst = llvm::CallInst::Create(callee, args, bundles, "", emitter.CurrentBasicBlock());
                if (emitter.DIBuilder())
                {
                    callInst->setDebugLoc(emitter.GetDebugLocation(span));
                }
            }
        }
        else
        {
            llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(emitter.Context(), "next", emitter.Function());
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            llvm::BasicBlock* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.Builder().CreateInvoke(callee, nextBlock, unwindBlock, args);
            }
            else
            {
                llvm::InvokeInst* invokeInst = llvm::InvokeInst::Create(callee, nextBlock, unwindBlock, args, bundles, "", emitter.CurrentBasicBlock());
                if (emitter.DIBuilder())
                {
                    invokeInst->setDebugLoc(emitter.GetDebugLocation(span));
                }
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
    llvm::Value* interfaceTypePtr = emitter.Stack().Pop();
    ArgVector objectIndeces;
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    llvm::Value* objectPtr = emitter.Builder().CreateGEP(interfaceTypePtr, objectIndeces);
    emitter.Builder().CreateStore(llvm::Constant::getNullValue(emitter.Builder().getInt8PtrTy()), objectPtr);
    ArgVector interfaceIndeces;
    interfaceIndeces.push_back(emitter.Builder().getInt32(0));
    interfaceIndeces.push_back(emitter.Builder().getInt32(1));
    llvm::Value* interfacePtr = emitter.Builder().CreateGEP(interfaceTypePtr, interfaceIndeces);
    emitter.Builder().CreateStore(llvm::Constant::getNullValue(emitter.Builder().getInt8PtrTy()), interfacePtr);
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
    llvm::Value* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    llvm::Value* thatPtr = emitter.Stack().Pop();
    ArgVector objectIndeces;
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    llvm::Value* objectPtr = emitter.Builder().CreateGEP(interfaceTypePtr, objectIndeces);
    llvm::Value* thatObjectPtr = emitter.Builder().CreateGEP(thatPtr, objectIndeces);
    llvm::Value* thatObject = emitter.Builder().CreateLoad(thatObjectPtr);
    emitter.Builder().CreateStore(thatObject, objectPtr);
    ArgVector interfaceIndeces;
    interfaceIndeces.push_back(emitter.Builder().getInt32(0));
    interfaceIndeces.push_back(emitter.Builder().getInt32(1));
    llvm::Value* interfacePtr = emitter.Builder().CreateGEP(interfaceTypePtr, interfaceIndeces);
    llvm::Value* thatInterfacePtr = emitter.Builder().CreateGEP(thatPtr, interfaceIndeces);
    llvm::Value* thatInterfaceObject = emitter.Builder().CreateLoad(thatInterfacePtr);
    emitter.Builder().CreateStore(thatInterfaceObject, interfacePtr);
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
    llvm::Value* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    llvm::Value* thatPtr = emitter.Stack().Pop();
    ArgVector objectIndeces;
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    llvm::Value* objectPtr = emitter.Builder().CreateGEP(interfaceTypePtr, objectIndeces);
    llvm::Value* thatObjectPtr = emitter.Builder().CreateGEP(thatPtr, objectIndeces);
    llvm::Value* thatObject = emitter.Builder().CreateLoad(thatObjectPtr);
    emitter.Builder().CreateStore(thatObject, objectPtr);
    ArgVector interfaceIndeces;
    interfaceIndeces.push_back(emitter.Builder().getInt32(0));
    interfaceIndeces.push_back(emitter.Builder().getInt32(1));
    llvm::Value* interfacePtr = emitter.Builder().CreateGEP(interfaceTypePtr, interfaceIndeces);
    llvm::Value* thatInterfacePtr = emitter.Builder().CreateGEP(thatPtr, interfaceIndeces);
    llvm::Value* thatInterfaceObject = emitter.Builder().CreateLoad(thatInterfacePtr);
    emitter.Builder().CreateStore(thatInterfaceObject, interfacePtr);
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
    llvm::Value* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    llvm::Value* thatPtr = emitter.Stack().Pop();
    ArgVector objectIndeces;
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    llvm::Value* objectPtr = emitter.Builder().CreateGEP(interfaceTypePtr, objectIndeces);
    llvm::Value* thatObjectPtr = emitter.Builder().CreateGEP(thatPtr, objectIndeces);
    llvm::Value* thatObject = emitter.Builder().CreateLoad(thatObjectPtr);
    emitter.Builder().CreateStore(thatObject, objectPtr);
    ArgVector interfaceIndeces;
    interfaceIndeces.push_back(emitter.Builder().getInt32(0));
    interfaceIndeces.push_back(emitter.Builder().getInt32(1));
    llvm::Value* interfacePtr = emitter.Builder().CreateGEP(interfaceTypePtr, interfaceIndeces);
    llvm::Value* thatInterfacePtr = emitter.Builder().CreateGEP(thatPtr, interfaceIndeces);
    llvm::Value* thatInterfaceObject = emitter.Builder().CreateLoad(thatInterfacePtr);
    emitter.Builder().CreateStore(thatInterfaceObject, interfacePtr);
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
    llvm::Value* interfaceTypePtr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    llvm::Value* thatPtr = emitter.Stack().Pop();
    ArgVector objectIndeces;
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    llvm::Value* objectPtr = emitter.Builder().CreateGEP(interfaceTypePtr, objectIndeces);
    llvm::Value* thatObjectPtr = emitter.Builder().CreateGEP(thatPtr, objectIndeces);
    llvm::Value* thatObject = emitter.Builder().CreateLoad(thatObjectPtr);
    emitter.Builder().CreateStore(thatObject, objectPtr);
    ArgVector interfaceIndeces;
    interfaceIndeces.push_back(emitter.Builder().getInt32(0));
    interfaceIndeces.push_back(emitter.Builder().getInt32(1));
    llvm::Value* interfacePtr = emitter.Builder().CreateGEP(interfaceTypePtr, interfaceIndeces);
    llvm::Value* thatInterfacePtr = emitter.Builder().CreateGEP(thatPtr, interfaceIndeces);
    llvm::Value* thatInterfaceObject = emitter.Builder().CreateLoad(thatInterfacePtr);
    emitter.Builder().CreateStore(thatInterfaceObject, interfacePtr);
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
    llvm::Value* classPtr = emitter.Stack().Pop();
    llvm::Value* classPtrAsVoidPtr = emitter.Builder().CreateBitCast(classPtr, emitter.Builder().getInt8PtrTy());
    ArgVector objectIndeces;
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    objectIndeces.push_back(emitter.Builder().getInt32(0));
    //llvm::Value* objectPtr = emitter.Builder().CreateGEP(temporaryInterfaceObjectVar->IrObject(emitter), objectIndeces);
    genObjects[0]->Load(emitter, OperationFlags::addr);
    llvm::Value* temporaryInterfaceObjectVar = emitter.Stack().Pop();
    llvm::Value* objectPtr = emitter.Builder().CreateGEP(temporaryInterfaceObjectVar, objectIndeces);
    emitter.Builder().CreateStore(classPtrAsVoidPtr, objectPtr);
    llvm::Value* vmtObjectPtr = sourceClassType->VmtObject(emitter, false);
    ArgVector imtsArrayIndeces;
    imtsArrayIndeces.push_back(emitter.Builder().getInt32(0));
    imtsArrayIndeces.push_back(emitter.Builder().getInt32(imtsVmtIndexOffset)); // new layout: 4
    llvm::Value* imtsArrayPtrPtr = emitter.Builder().CreateGEP(vmtObjectPtr, imtsArrayIndeces);
    llvm::Value* imtsArrayPtr = emitter.Builder().CreateBitCast(imtsArrayPtrPtr, llvm::PointerType::get(llvm::PointerType::get(emitter.Builder().getInt8PtrTy(), 0), 0));
    llvm::Value* imtArray = emitter.Builder().CreateLoad(imtsArrayPtr);
    ArgVector imtArrayIndeces;
    imtArrayIndeces.push_back(emitter.Builder().getInt32(interfaceIndex));
    llvm::Value* imtArrayPtr = emitter.Builder().CreateGEP(imtArray, imtArrayIndeces);
    llvm::Value* imt = emitter.Builder().CreateLoad(imtArrayPtr);
    ArgVector imtIndeces;
    imtIndeces.push_back(emitter.Builder().getInt32(0));
    imtIndeces.push_back(emitter.Builder().getInt32(1));
    //llvm::Value* imtPtr = emitter.Builder().CreateGEP(temporaryInterfaceObjectVar->IrObject(emitter), imtIndeces);
    llvm::Value* imtPtr = emitter.Builder().CreateGEP(temporaryInterfaceObjectVar, imtIndeces);
    emitter.Builder().CreateStore(imt, imtPtr);
    //emitter.Stack().Push(temporaryInterfaceObjectVar->IrObject(emitter));
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
