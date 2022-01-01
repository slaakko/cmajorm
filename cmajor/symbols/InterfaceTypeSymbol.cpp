// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

InterfaceTypeSymbol::InterfaceTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    TypeSymbol(SymbolType::interfaceTypeSymbol, span_, sourceModuleId_, name_), copyConstructor(nullptr)
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
        throw Exception("interface cannot be static", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception("interface cannot be virtual", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception("interface cannot be override", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("interface cannot be abstract", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception("interface cannot be inline", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("interface cannot be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception("interface cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception("interface cannot be suppressed", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception("interface cannot be default", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception("interface cannot be constexpr", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("interface cannot be cdecl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception("interface cannot be nothrow", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception("interface cannot be throw", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("interface cannot be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception("interface cannot be const", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("interface cannot be unit_test", GetSpan(), SourceModuleId());
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

void InterfaceTypeSymbol::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, MemberFunctionSymbol* interfaceMemberFunction, const Span& span, const boost::uuids::uuid& moduleId)
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
            void* nextBlock = nullptr;
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
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
                emitter.Stack().Push(emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span));
            }
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
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
            throw SymbolCheckException("interface type symbol contains null member function", GetSpan(), SourceModuleId());
        }
    }
}

InterfaceTypeDefaultConstructor::InterfaceTypeDefaultConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::interfaceTypeDefaultCtor, span_, sourceModuleId_, U"@interfaceDefaultCtor"), interfaceType(nullptr)
{
}

InterfaceTypeDefaultConstructor::InterfaceTypeDefaultConstructor(InterfaceTypeSymbol* interfaceType_) :
    FunctionSymbol(SymbolType::interfaceTypeDefaultCtor, interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"@constructor"), interfaceType(interfaceType_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(interfaceType->GetSpan(), interfaceType->SourceModuleId(), U"this");
    thisParam->SetType(interfaceType_->AddPointer(interfaceType->GetSpan(), interfaceType->SourceModuleId()));
    AddMember(thisParam);
    ComputeName();
}

void InterfaceTypeDefaultConstructor::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(interfaceType->TypeId());
}

void InterfaceTypeDefaultConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void InterfaceTypeDefaultConstructor::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type expected");
        interfaceType = static_cast<InterfaceTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void InterfaceTypeDefaultConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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

    void* interfacePtrPtr = emitter.GetImtPtrPtrFromInterface(interfaceTypePtr);
    emitter.CreateStore(emitter.CreateDefaultIrValueForVoidPtrType(), interfacePtrPtr);
}

InterfaceTypeCopyConstructor::InterfaceTypeCopyConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::interfaceTypeCopyCtor, span_, sourceModuleId_, U"@interfaceCopyCtor"), interfaceType(nullptr)
{
}

InterfaceTypeCopyConstructor::InterfaceTypeCopyConstructor(InterfaceTypeSymbol* interfaceType_) :
    FunctionSymbol(SymbolType::interfaceTypeCopyCtor, interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"@interfaceCopyCtor"), interfaceType(interfaceType_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"this");
    thisParam->SetType(interfaceType_->AddPointer(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"that");
    thatParam->SetType(interfaceType_->AddConst(interfaceType_->GetSpan(), interfaceType_->SourceModuleId())->AddLvalueReference(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thatParam);
    ComputeName();
}

void InterfaceTypeCopyConstructor::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(interfaceType->TypeId());
}

void InterfaceTypeCopyConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void InterfaceTypeCopyConstructor::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type expected");
        interfaceType = static_cast<InterfaceTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void InterfaceTypeCopyConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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

    void* interfacePtrPtr = emitter.GetImtPtrPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtrPtr = emitter.GetImtPtrPtrFromInterface(thatPtr);
    void* thatInterfacePtr = emitter.CreateLoad(thatInterfacePtrPtr);
    emitter.CreateStore(thatInterfacePtr, interfacePtrPtr);
}

InterfaceTypeMoveConstructor::InterfaceTypeMoveConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::interfaceTypeMoveCtor, span_, sourceModuleId_, U"@interfaceMoveCtor"), interfaceType(nullptr)
{
}

InterfaceTypeMoveConstructor::InterfaceTypeMoveConstructor(InterfaceTypeSymbol* interfaceType_) :
    FunctionSymbol(SymbolType::interfaceTypeMoveCtor, interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"@interfaceMoveCtor"), interfaceType(interfaceType_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"this");
    thisParam->SetType(interfaceType_->AddPointer(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"that");
    thatParam->SetType(interfaceType_->AddConst(interfaceType_->GetSpan(), interfaceType_->SourceModuleId())->AddRvalueReference(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thatParam);
    ComputeName();
}

void InterfaceTypeMoveConstructor::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(interfaceType->TypeId());
}

void InterfaceTypeMoveConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void InterfaceTypeMoveConstructor::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type expected");
        interfaceType = static_cast<InterfaceTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void InterfaceTypeMoveConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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

    void* interfacePtrPtr = emitter.GetImtPtrPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtrPtr = emitter.GetImtPtrPtrFromInterface(thatPtr);
    void* thatInterfacePtr = emitter.CreateLoad(thatInterfacePtrPtr);
    emitter.CreateStore(thatInterfacePtr, interfacePtrPtr);
}

InterfaceTypeCopyAssignment::InterfaceTypeCopyAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::interfaceTypeCopyAssignment, span_, sourceModuleId_, U"@interfaceCopyAssignment"), interfaceType(nullptr)
{
}

InterfaceTypeCopyAssignment::InterfaceTypeCopyAssignment(InterfaceTypeSymbol* interfaceType_) : 
    FunctionSymbol(SymbolType::interfaceTypeCopyAssignment, interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"@interfaceCopyAssignment"), interfaceType(interfaceType_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"this");
    thisParam->SetType(interfaceType_->AddPointer(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"that");
    thatParam->SetType(interfaceType_->AddConst(interfaceType_->GetSpan(), interfaceType_->SourceModuleId())->AddLvalueReference(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thatParam);
    TypeSymbol* voidType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void");
    SetReturnType(voidType);
    ComputeName();
}

void InterfaceTypeCopyAssignment::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(interfaceType->TypeId());
}

void InterfaceTypeCopyAssignment::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void InterfaceTypeCopyAssignment::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type expected");
        interfaceType = static_cast<InterfaceTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void InterfaceTypeCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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

    void* interfacePtrPtr = emitter.GetImtPtrPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtrPtr = emitter.GetImtPtrPtrFromInterface(thatPtr);
    void* thatInterfacePtr = emitter.CreateLoad(thatInterfacePtrPtr);
    emitter.CreateStore(thatInterfacePtr, interfacePtrPtr);
}

InterfaceTypeMoveAssignment::InterfaceTypeMoveAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::interfaceTypeMoveAssignment, span_, sourceModuleId_, U"@interfaceMoveAssignment"), interfaceType(nullptr)
{
}

InterfaceTypeMoveAssignment::InterfaceTypeMoveAssignment(InterfaceTypeSymbol* interfaceType_) :
    FunctionSymbol(SymbolType::interfaceTypeMoveAssignment, interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"@interfaceMoveAssignment"), interfaceType(interfaceType_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"this");
    thisParam->SetType(interfaceType_->AddPointer(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"that");
    thatParam->SetType(interfaceType_->AddRvalueReference(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thatParam);
    TypeSymbol* voidType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void");
    SetReturnType(voidType);
    ComputeName();
}

void InterfaceTypeMoveAssignment::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(interfaceType->TypeId());
}

void InterfaceTypeMoveAssignment::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void InterfaceTypeMoveAssignment::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type expected");
        interfaceType = static_cast<InterfaceTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void InterfaceTypeMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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

    void* interfacePtrPtr = emitter.GetImtPtrPtrFromInterface(interfaceTypePtr);
    void* thatInterfacePtrPtr = emitter.GetImtPtrPtrFromInterface(thatPtr);
    void* thatInterfacePtr = emitter.CreateLoad(thatInterfacePtrPtr);
    emitter.CreateStore(thatInterfacePtr, interfacePtrPtr);
}

ClassToInterfaceConversion::ClassToInterfaceConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::classToInterfaceConversion, span_, sourceModuleId_, U"@classToInterfaceConversion"), sourceClassType(nullptr), targetInterfaceType(nullptr), interfaceIndex(0)
{
}

ClassToInterfaceConversion::ClassToInterfaceConversion(ClassTypeSymbol* sourceClassType_, InterfaceTypeSymbol* targetInterfaceType_, int32_t interfaceIndex_, 
    const Span& span_, const boost::uuids::uuid& sourceModuleId_) :
    FunctionSymbol(SymbolType::classToInterfaceConversion, span_, sourceModuleId_, U"@classToInterfaceConversion"), sourceClassType(sourceClassType_), 
    targetInterfaceType(targetInterfaceType_), interfaceIndex(interfaceIndex_)
{
    SetConversion();
    SetConversionSourceType(sourceClassType->PlainType(GetSpan(), SourceModuleId()));
    SetConversionTargetType(targetInterfaceType->PlainType(GetSpan(), SourceModuleId()));
}

void ClassToInterfaceConversion::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(sourceClassType->TypeId());
    writer.GetBinaryWriter().Write(targetInterfaceType->TypeId());
    writer.GetBinaryWriter().Write(interfaceIndex);
}

void ClassToInterfaceConversion::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 2);
}

void ClassToInterfaceConversion::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::classTypeSymbol, "class type expected");
        sourceClassType = static_cast<ClassTypeSymbol*>(typeSymbol);
    }
    else if (index == 2)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type expected");
        targetInterfaceType = static_cast<InterfaceTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

std::vector<LocalVariableSymbol*> ClassToInterfaceConversion::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    temporaries.push_back(currentFunction->CreateTemporary(targetInterfaceType, GetSpan(), SourceModuleId()));
    return temporaries;
}

void ClassToInterfaceConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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
    void* imtPtr = emitter.GetImtPtrPtrFromInterface(temporaryInterfaceObjectVar);
    emitter.CreateStore(imt, imtPtr);
    emitter.Stack().Push(temporaryInterfaceObjectVar);
}

void ClassToInterfaceConversion::Check()
{
    FunctionSymbol::Check();
    if (!sourceClassType)
    {
        throw SymbolCheckException("class to interface conversion has no source class type", GetSpan(), SourceModuleId());
    }
    if (!targetInterfaceType)
    {
        throw SymbolCheckException("class to interface conversion has no target interface type", GetSpan(), SourceModuleId());
    }
}

GetObjectPtrFromInterface::GetObjectPtrFromInterface(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::getObjectPtrFromInterfaceSymbol, span_, sourceModuleId_, name_), interfaceType(nullptr)
{
}

GetObjectPtrFromInterface::GetObjectPtrFromInterface(InterfaceTypeSymbol* interfaceType_) :
    FunctionSymbol(SymbolType::getObjectPtrFromInterfaceSymbol, interfaceType_->GetSpan(), interfaceType_->SourceModuleId(), U"GetObjectPtrFromInterface"), interfaceType(interfaceType_)
{
    SetGroupName(U"GetObjectPtrFromInterface");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(Span(), boost::uuids::nil_uuid(), U"this");
    thisParam->SetType(interfaceType_->AddPointer(interfaceType_->GetSpan(), interfaceType_->SourceModuleId()));
    AddMember(thisParam);
    TypeSymbol* voidPtrType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void")->AddPointer(interfaceType_->GetSpan(), interfaceType_->SourceModuleId());
    SetReturnType(voidPtrType);
    ComputeName();
}

void GetObjectPtrFromInterface::Write(SymbolWriter& writer) 
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(interfaceType->TypeId());
}

void GetObjectPtrFromInterface::Read(SymbolReader& reader) 
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void GetObjectPtrFromInterface::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type expected");
        interfaceType = static_cast<InterfaceTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}


void GetObjectPtrFromInterface::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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
    void* object = emitter.CreateLoad(objectPtr);
    emitter.Stack().Push(object);
}

} } // namespace cmajor::symbols
