// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ArrayTypeSymbol.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

ArrayTypeSymbol::ArrayTypeSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(SymbolType::arrayTypeSymbol, span_, name_), elementType(nullptr), size(-1)
{
}

ArrayTypeSymbol::ArrayTypeSymbol(const Span& span_, const std::u32string& name_, TypeSymbol* elementType_, int64_t size_) : 
    TypeSymbol(SymbolType::arrayTypeSymbol, span_, name_), elementType(elementType_), size(size_)
{
}

void ArrayTypeSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    const boost::uuids::uuid& elementTypeId = elementType->TypeId();
    writer.GetBinaryWriter().Write(elementTypeId);
    writer.GetBinaryWriter().Write(size);
}

void ArrayTypeSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    boost::uuids::uuid elementTypeId;
    reader.GetBinaryReader().ReadUuid(elementTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, elementTypeId, 0);
    size = reader.GetBinaryReader().ReadLong();
}

void ArrayTypeSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 0)
    {
        elementType = typeSymbol;
    }
    else
    {
        throw Exception(GetRootModuleForCurrentThread(), "internal error: invalid array emplace type index " + std::to_string(index), GetSpan());
    }
}

void* ArrayTypeSymbol::IrType(Emitter& emitter)
{
    if (size == -1)
    {
        throw Exception(GetRootModuleForCurrentThread(), "array '" + ToUtf8(FullName()) + "' size not defined", GetSpan());
    }
    void* localIrType = emitter.GetIrTypeByTypeId(TypeId());
    if (!localIrType)
    {
        localIrType = emitter.GetIrTypeForArrayType(elementType->IrType(emitter), size);
        emitter.SetIrTypeByTypeId(TypeId(), localIrType);
    }
    return localIrType;
}

void* ArrayTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    if (size == -1)
    {
        throw Exception(GetRootModuleForCurrentThread(), "array '" + ToUtf8(FullName()) + "' size not defined", GetSpan());
    }
    void* irType = IrType(emitter);
    std::vector<void*> arrayOfDefaults;
    for (int64_t i = 0; i < size; ++i)
    {
        arrayOfDefaults.push_back(elementType->CreateDefaultIrValue(emitter));
    }
    return emitter.CreateIrValueForConstantArray(irType, arrayOfDefaults, std::string());
}

void* ArrayTypeSymbol::CreateDIType(Emitter& emitter)
{
    // todo...
    std::vector<void*> elements;
    return emitter.CreateDITypeForArray(elementType->GetDIType(emitter), elements);
}

ValueType ArrayTypeSymbol::GetValueType() const
{
    return ValueType::arrayValue;
}

Value* ArrayTypeSymbol::MakeValue() const
{
    std::vector<std::unique_ptr<Value>> elementValues;
    return new ArrayValue(GetSpan(), const_cast<TypeSymbol*>(static_cast<const TypeSymbol*>(this)), std::move(elementValues));
}

void ArrayTypeSymbol::Check()
{
    TypeSymbol::Check();
    if (!elementType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array type has no element type", GetSpan());
    }
}

ArrayLengthFunction::ArrayLengthFunction(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::arrayLengthFunctionSymbol, span_, name_), arrayType(nullptr)
{
}

ArrayLengthFunction::ArrayLengthFunction(ArrayTypeSymbol* arrayType_) : FunctionSymbol(SymbolType::arrayLengthFunctionSymbol, arrayType_->GetSpan(), U"Length"), arrayType(arrayType_)
{
    SetGroupName(U"Length");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* arrayParam = new ParameterSymbol(arrayType->GetSpan(), U"array");
    arrayParam->SetType(arrayType);
    AddMember(arrayParam);
    TypeSymbol* longType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"long"); 
    SetReturnType(longType);
    ComputeName();
}

void ArrayLengthFunction::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(arrayType->TypeId());
}

void ArrayLengthFunction::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void ArrayLengthFunction::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::arrayTypeSymbol, "array type expected");
        arrayType = static_cast<ArrayTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void ArrayLengthFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    emitter.SetCurrentDebugLocation(span);
    Assert(genObjects.size() == 1, "array length needs one object");
    void* size = emitter.CreateIrValueForULong(arrayType->Size());
    emitter.Stack().Push(size);
}

std::unique_ptr<Value> ArrayLengthFunction::ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span, Value* receiver) const
{
    return std::unique_ptr<Value>(new LongValue(span, arrayType->Size()));
}

void ArrayLengthFunction::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array length function has no array type", GetSpan());
    }
}

ArrayBeginFunction::ArrayBeginFunction(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::arrayBeginFunctionSymbol, span_, name_), arrayType(nullptr)
{
}

ArrayBeginFunction::ArrayBeginFunction(ArrayTypeSymbol* arrayType_) : FunctionSymbol(SymbolType::arrayBeginFunctionSymbol, arrayType_->GetSpan(), U"@arrayBegin"), arrayType(arrayType_)
{
    SetGroupName(U"Begin");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* arrayParam = new ParameterSymbol(arrayType->GetSpan(), U"array");
    arrayParam->SetType(arrayType);
    AddMember(arrayParam);
    TypeSymbol* returnType = arrayType->ElementType()->AddPointer(arrayType->GetSpan());
    SetReturnType(returnType);
    ComputeName();
}

void ArrayBeginFunction::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(arrayType->TypeId());
}

void ArrayBeginFunction::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void ArrayBeginFunction::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::arrayTypeSymbol, "array type expected");
        arrayType = static_cast<ArrayTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void ArrayBeginFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "array begin needs one object");
    genObjects[0]->Load(emitter, OperationFlags::addr);
    emitter.SetCurrentDebugLocation(span);
    void* arrayPtr = emitter.Stack().Pop();
    void* beginPtr = emitter.GetArrayBeginAddress(arrayPtr);
    emitter.Stack().Push(beginPtr);
}

void ArrayBeginFunction::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array begin function has no array type", GetSpan());
    }
}

ArrayEndFunction::ArrayEndFunction(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::arrayEndFunctionSymbol, span_, name_), arrayType(nullptr)
{
}

ArrayEndFunction::ArrayEndFunction(ArrayTypeSymbol* arrayType_) : FunctionSymbol(SymbolType::arrayEndFunctionSymbol, arrayType_->GetSpan(), U"@arrayEnd"), arrayType(arrayType_)
{
    SetGroupName(U"End");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* arrayParam = new ParameterSymbol(arrayType->GetSpan(), U"array");
    arrayParam->SetType(arrayType);
    AddMember(arrayParam);
    TypeSymbol* returnType = arrayType->ElementType()->AddPointer(arrayType->GetSpan());
    SetReturnType(returnType);
    ComputeName();
}

void ArrayEndFunction::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(arrayType->TypeId());
}

void ArrayEndFunction::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void ArrayEndFunction::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::arrayTypeSymbol, "array type expected");
        arrayType = static_cast<ArrayTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void ArrayEndFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "array end needs one object");
    genObjects[0]->Load(emitter, OperationFlags::addr);
    emitter.SetCurrentDebugLocation(span);
    void* arrayPtr = emitter.Stack().Pop();
    void* endPtr = emitter.GetArrayEndAddress(arrayPtr, arrayType->Size());
    emitter.Stack().Push(endPtr);
}

void ArrayEndFunction::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array end function has no array type", GetSpan());
    }
}

ArrayCBeginFunction::ArrayCBeginFunction(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::arrayCBeginFunctionSymbol, span_, name_), arrayType(nullptr)
{
}

ArrayCBeginFunction::ArrayCBeginFunction(ArrayTypeSymbol* arrayType_) : FunctionSymbol(SymbolType::arrayCBeginFunctionSymbol, arrayType_->GetSpan(), U"@arrayCBegin"), arrayType(arrayType_)
{
    SetGroupName(U"CBegin");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* arrayParam = new ParameterSymbol(arrayType->GetSpan(), U"array");
    arrayParam->SetType(arrayType);
    AddMember(arrayParam);
    TypeSymbol* returnType = arrayType->ElementType()->AddConst(arrayType->GetSpan())->AddPointer(arrayType->GetSpan());
    SetReturnType(returnType);
    ComputeName();
}

void ArrayCBeginFunction::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(arrayType->TypeId());
}

void ArrayCBeginFunction::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void ArrayCBeginFunction::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::arrayTypeSymbol, "array type expected");
        arrayType = static_cast<ArrayTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void ArrayCBeginFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "array cbegin needs one object");
    genObjects[0]->Load(emitter, OperationFlags::addr);
    emitter.SetCurrentDebugLocation(span);
    void* arrayPtr = emitter.Stack().Pop();
    void* beginPtr = emitter.GetArrayBeginAddress(arrayPtr);
    emitter.Stack().Push(beginPtr);
}

void ArrayCBeginFunction::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array cbegin function has no array type", GetSpan());
    }
}

ArrayCEndFunction::ArrayCEndFunction(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::arrayCEndFunctionSymbol, span_, name_), arrayType(nullptr)
{
}

ArrayCEndFunction::ArrayCEndFunction(ArrayTypeSymbol* arrayType_) : FunctionSymbol(SymbolType::arrayCEndFunctionSymbol, arrayType_->GetSpan(), U"@arrayCEnd"), arrayType(arrayType_)
{
    SetGroupName(U"CEnd");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* arrayParam = new ParameterSymbol(arrayType->GetSpan(), U"array");
    arrayParam->SetType(arrayType);
    AddMember(arrayParam);
    TypeSymbol* returnType = arrayType->ElementType()->AddConst(arrayType->GetSpan())->AddPointer(arrayType->GetSpan());
    SetReturnType(returnType);
    ComputeName();
}

void ArrayCEndFunction::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(arrayType->TypeId());
}

void ArrayCEndFunction::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 1);
}

void ArrayCEndFunction::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::arrayTypeSymbol, "array type expected");
        arrayType = static_cast<ArrayTypeSymbol*>(typeSymbol);
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void ArrayCEndFunction::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "array cend needs one object");
    genObjects[0]->Load(emitter, OperationFlags::addr);
    emitter.SetCurrentDebugLocation(span);
    void* arrayPtr = emitter.Stack().Pop();
    void* endPtr = emitter.GetArrayEndAddress(arrayPtr, arrayType->Size());
    emitter.Stack().Push(endPtr);
}

void ArrayCEndFunction::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array cend function has no array type", GetSpan());
    }
}

ArrayTypeDefaultConstructor::ArrayTypeDefaultConstructor(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeDefaultConstructor_, const Span& span_) :
    FunctionSymbol(arrayType_->GetSpan(), U"@arrayDefaultCtor"), arrayType(arrayType_), elementTypeDefaultConstructor(elementTypeDefaultConstructor_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_); 
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(arrayType->AddPointer(span_));
    AddMember(thisParam);
    ComputeName();
}

std::vector<LocalVariableSymbol*> ArrayTypeDefaultConstructor::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    temporaries.push_back(currentFunction->CreateTemporary(GetModule()->GetSymbolTable().GetTypeByName(U"long"), Span()));
    return temporaries;
}

void ArrayTypeDefaultConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "array type default constructor needs two objects: one array type object and one loop variable temporary");
    emitter.Stack().Push(emitter.CreateIrValueForLong(0));
    GenObject* loopVar = genObjects[1];
    loopVar->Store(emitter, OperationFlags::none);
    void* size = emitter.CreateIrValueForLong(arrayType->Size());
    void* loop = emitter.CreateBasicBlock("loop");
    void* init = emitter.CreateBasicBlock("init");
    void* next = emitter.CreateBasicBlock("next");
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(loop);
    loopVar->Load(emitter, OperationFlags::none);
    void* index = emitter.Stack().Pop();
    void* less = emitter.CreateICmpULT(index, size);
    emitter.CreateCondBr(less, init, next);
    emitter.SetCurrentBasicBlock(init);
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* ptr = emitter.Stack().Pop();
    loopVar->Load(emitter, OperationFlags::none);
    void* index2 = emitter.Stack().Pop();
    void* elementPtr = emitter.CreateArrayIndexAddress(ptr, index2);
    NativeValue elementPtrValue(elementPtr);
    std::vector<GenObject*> elementGenObjects;
    elementGenObjects.push_back(&elementPtrValue);
    elementTypeDefaultConstructor->GenerateCall(emitter, elementGenObjects, OperationFlags::none, span);
    void* nextI = emitter.CreateAdd(index2, emitter.CreateIrValueForLong(1));
    emitter.Stack().Push(nextI);
    loopVar->Store(emitter, OperationFlags::none);
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(next);
}

void ArrayTypeDefaultConstructor::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array default constructor has no array type", GetSpan());
    }
    if (!elementTypeDefaultConstructor)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array default constructor has no element type default constructor", GetSpan());
    }
}

ArrayTypeCopyConstructor::ArrayTypeCopyConstructor(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeCopyConstructor_, const Span& span_) :
    FunctionSymbol(arrayType_->GetSpan(), U"@arrayCopyCtor"), arrayType(arrayType_), elementTypeCopyConstructor(elementTypeCopyConstructor_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(arrayType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(arrayType->AddConst(span_)->AddLvalueReference(span_));
    AddMember(thatParam);
    ComputeName();
}

std::vector<LocalVariableSymbol*> ArrayTypeCopyConstructor::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    temporaries.push_back(currentFunction->CreateTemporary(GetModule()->GetSymbolTable().GetTypeByName(U"long"), Span()));
    return temporaries;
}

void ArrayTypeCopyConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 3, "copy constructor needs three objects: two array type objects and one loop variable temporary");
    emitter.Stack().Push(emitter.CreateIrValueForLong(0));
    GenObject* loopVar = genObjects[2];
    loopVar->Store(emitter, OperationFlags::none);
    void* size = emitter.CreateIrValueForLong(arrayType->Size());
    void* loop = emitter.CreateBasicBlock("loop");
    void* init = emitter.CreateBasicBlock("init");
    void* next = emitter.CreateBasicBlock("next");
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(loop);
    loopVar->Load(emitter, OperationFlags::none);
    void* index = emitter.Stack().Pop();
    void* less = emitter.CreateICmpULT(index, size);
    emitter.CreateCondBr(less, init, next);
    emitter.SetCurrentBasicBlock(init);
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* ptr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* sourcePtr = emitter.Stack().Pop();
    loopVar->Load(emitter, OperationFlags::none);
    void* index2 = emitter.Stack().Pop();
    void* elementPtr = emitter.CreateArrayIndexAddress(ptr, index2);
    NativeValue elementPtrValue(elementPtr);
    std::vector<GenObject*> elementGenObjects;
    elementGenObjects.push_back(&elementPtrValue);
    void* sourceElementPtr = emitter.CreateArrayIndexAddress(sourcePtr, index2);
    void* sourceElementValue = sourceElementPtr;
    TypeSymbol* elementType = arrayType->ElementType();
    if (elementType->IsBasicTypeSymbol() || elementType->IsPointerType() || elementType->GetSymbolType() == SymbolType::delegateTypeSymbol)
    {
        sourceElementValue = emitter.CreateLoad(sourceElementPtr);
    }
    NativeValue sourceValue(sourceElementValue);
    elementGenObjects.push_back(&sourceValue);
    elementTypeCopyConstructor->GenerateCall(emitter, elementGenObjects, OperationFlags::none, span);
    void* nextI = emitter.CreateAdd(index2, emitter.CreateIrValueForLong(1));
    emitter.Stack().Push(nextI);
    loopVar->Store(emitter, OperationFlags::none);
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(next);
}

void ArrayTypeCopyConstructor::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array copy constructor has no array type", GetSpan());
    }
    if (!elementTypeCopyConstructor)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array copy constructor has no element type copy constructor", GetSpan());
    }
}

ArrayTypeMoveConstructor::ArrayTypeMoveConstructor(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeMoveConstructor_, const Span& span_) :
    FunctionSymbol(arrayType_->GetSpan(), U"@arrayMoveCtor"), arrayType(arrayType_), elementTypeMoveConstructor(elementTypeMoveConstructor_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(arrayType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(arrayType->AddRvalueReference(span_));
    AddMember(thatParam);
    ComputeName();
}

std::vector<LocalVariableSymbol*> ArrayTypeMoveConstructor::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    temporaries.push_back(currentFunction->CreateTemporary(GetModule()->GetSymbolTable().GetTypeByName(U"long"), Span()));
    return temporaries;
}

void ArrayTypeMoveConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 3, "move constructor needs three objects: two array type objects and one loop variable temporary");
    emitter.Stack().Push(emitter.CreateIrValueForLong(0));
    GenObject* loopVar = genObjects[2];
    loopVar->Store(emitter, OperationFlags::none);
    void* size = emitter.CreateIrValueForLong(arrayType->Size());
    void* loop = emitter.CreateBasicBlock("loop");
    void* init = emitter.CreateBasicBlock("init");
    void* next = emitter.CreateBasicBlock("next");
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(loop);
    loopVar->Load(emitter, OperationFlags::none);
    void* index = emitter.Stack().Pop();
    void* less = emitter.CreateICmpULT(index, size);
    emitter.CreateCondBr(less, init, next);
    emitter.SetCurrentBasicBlock(init);
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* ptr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* sourcePtr = emitter.Stack().Pop();
    loopVar->Load(emitter, OperationFlags::none);
    void* index2 = emitter.Stack().Pop();
    void* elementPtr = emitter.CreateArrayIndexAddress(ptr, index2);
    NativeValue elementPtrValue(elementPtr);
    std::vector<GenObject*> elementGenObjects;
    elementGenObjects.push_back(&elementPtrValue);
    void* sourceElementPtr = emitter.CreateArrayIndexAddress(sourcePtr, index2);
    NativeValue sourcePtrValue(sourceElementPtr);
    elementGenObjects.push_back(&sourcePtrValue);
    elementTypeMoveConstructor->GenerateCall(emitter, elementGenObjects, OperationFlags::none, span);
    void* nextI = emitter.CreateAdd(index2, emitter.CreateIrValueForLong(1));
    emitter.Stack().Push(nextI);
    loopVar->Store(emitter, OperationFlags::none);
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(next);
}

void ArrayTypeMoveConstructor::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array move constructor has no array type", GetSpan());
    }
    if (!elementTypeMoveConstructor)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array move constructor has no element type move constructor", GetSpan());
    }
}

ArrayTypeCopyAssignment::ArrayTypeCopyAssignment(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeCopyAssignment_, const Span& span_) :
    FunctionSymbol(arrayType_->GetSpan(), U"@arrayCopyAssignment"), arrayType(arrayType_), elementTypeCopyAssignment(elementTypeCopyAssignment_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(arrayType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(arrayType->AddConst(span_)->AddLvalueReference(span_));
    AddMember(thatParam);
    TypeSymbol* voidType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void");
    SetReturnType(voidType);
    ComputeName();
}

std::vector<LocalVariableSymbol*> ArrayTypeCopyAssignment::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    temporaries.push_back(currentFunction->CreateTemporary(GetModule()->GetSymbolTable().GetTypeByName(U"long"), Span()));
    return temporaries;
}

void ArrayTypeCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 3, "copy assignment needs three objects: two array type objects and one loop variable temporary");
    emitter.Stack().Push(emitter.CreateIrValueForLong(0));
    GenObject* loopVar = genObjects[2];
    loopVar->Store(emitter, OperationFlags::none);
    void* size = emitter.CreateIrValueForLong(arrayType->Size());
    void* loop = emitter.CreateBasicBlock("loop");
    void* init = emitter.CreateBasicBlock("init");
    void* next = emitter.CreateBasicBlock("next");
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(loop);
    loopVar->Load(emitter, OperationFlags::none);
    void* index = emitter.Stack().Pop();
    void* less = emitter.CreateICmpULT(index, size);
    emitter.CreateCondBr(less, init, next);
    emitter.SetCurrentBasicBlock(init);
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* ptr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* sourcePtr = emitter.Stack().Pop();
    loopVar->Load(emitter, OperationFlags::none);
    void* index2 = emitter.Stack().Pop();
    void* elementPtr = emitter.CreateArrayIndexAddress(ptr, index2);
    NativeValue elementPtrValue(elementPtr);
    std::vector<GenObject*> elementGenObjects;
    elementGenObjects.push_back(&elementPtrValue);
    void* sourceElementPtr = emitter.CreateArrayIndexAddress(sourcePtr, index2);
    void* sourceElementValue = sourceElementPtr;
    TypeSymbol* elementType = arrayType->ElementType();
    if (elementType->IsBasicTypeSymbol() || elementType->IsPointerType() || elementType->GetSymbolType() == SymbolType::delegateTypeSymbol)
    {
        sourceElementValue = emitter.CreateLoad(sourceElementPtr);
    }
    NativeValue sourceValue(sourceElementValue);
    elementGenObjects.push_back(&sourceValue);
    elementTypeCopyAssignment->GenerateCall(emitter, elementGenObjects, OperationFlags::none, span);
    void* nextI = emitter.CreateAdd(index2, emitter.CreateIrValueForLong(1));
    emitter.Stack().Push(nextI);
    loopVar->Store(emitter, OperationFlags::none);
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(next);
}

void ArrayTypeCopyAssignment::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array copy assignment has no array type", GetSpan());
    }
    if (!elementTypeCopyAssignment)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array copy assignmet has no element type copy assignment", GetSpan());
    }
}

ArrayTypeMoveAssignment::ArrayTypeMoveAssignment(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeMoveAssignment_, const Span& span_) :
    FunctionSymbol(arrayType_->GetSpan(), U"@arrayMoveAssignment"), arrayType(arrayType_), elementTypeMoveAssignment(elementTypeMoveAssignment_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(arrayType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(arrayType->AddRvalueReference(span_));
    AddMember(thatParam);
    TypeSymbol* voidType = GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void");
    SetReturnType(voidType);
    ComputeName();
}

std::vector<LocalVariableSymbol*> ArrayTypeMoveAssignment::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    std::vector<LocalVariableSymbol*> temporaries;
    temporaries.push_back(currentFunction->CreateTemporary(GetModule()->GetSymbolTable().GetTypeByName(U"long"), Span()));
    return temporaries;
}

void ArrayTypeMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 3, "move assignment needs three objects: two array type objects and one loop variable temporary");
    emitter.Stack().Push(emitter.CreateIrValueForLong(0));
    GenObject* loopVar = genObjects[2];
    loopVar->Store(emitter, OperationFlags::none);
    void* size = emitter.CreateIrValueForLong(arrayType->Size());
    void* loop = emitter.CreateBasicBlock("loop");
    void* init = emitter.CreateBasicBlock("init");
    void* next = emitter.CreateBasicBlock("next");
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(loop);
    loopVar->Load(emitter, OperationFlags::none);
    void* index = emitter.Stack().Pop();
    void* less = emitter.CreateICmpULT(index, size);
    emitter.CreateCondBr(less, init, next);
    emitter.SetCurrentBasicBlock(init);
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* ptrPtr = emitter.Stack().Pop();
    void* ptr = emitter.CreateLoad(ptrPtr);
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* sourcePtr = emitter.Stack().Pop();
    loopVar->Load(emitter, OperationFlags::none);
    void* index2 = emitter.Stack().Pop();
    void* elementPtr = emitter.CreateArrayIndexAddress(ptr, index2);
    NativeValue elementPtrValue(elementPtr);
    std::vector<GenObject*> elementGenObjects;
    elementGenObjects.push_back(&elementPtrValue);
    void* sourceElementPtr = emitter.CreateArrayIndexAddress(sourcePtr, index2);
    TypeSymbol* elementType = arrayType->ElementType();
    NativeValue sourcePtrValue(sourceElementPtr);
    elementGenObjects.push_back(&sourcePtrValue);
    elementTypeMoveAssignment->GenerateCall(emitter, elementGenObjects, OperationFlags::none, span);
    void* nextI = emitter.CreateAdd(index2, emitter.CreateIrValueForLong(1));
    emitter.Stack().Push(nextI);
    loopVar->Store(emitter, OperationFlags::none);
    emitter.CreateBr(loop);
    emitter.SetCurrentBasicBlock(next);
}

void ArrayTypeMoveAssignment::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array move assignment has no array type", GetSpan());
    }
    if (!elementTypeMoveAssignment)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array move assignmet has no element type move assignment", GetSpan());
    }
}

ArrayTypeElementAccess::ArrayTypeElementAccess(ArrayTypeSymbol* arrayType_, const Span& span_) : FunctionSymbol(arrayType_->GetSpan(), U"@arrayElementAccess"), arrayType(arrayType_)
{
    SetGroupName(U"operator[]");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* arrayParam = new ParameterSymbol(span_, U"array");
    arrayParam->SetType(arrayType);
    AddMember(arrayParam);
    ParameterSymbol* indexParam = new ParameterSymbol(span_, U"index");
    indexParam->SetType(GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"long"));
    AddMember(indexParam);
    TypeSymbol* returnType = arrayType->ElementType();
    if (!returnType->IsBasicTypeSymbol() && !returnType->IsPointerType() && returnType->GetSymbolType() != SymbolType::delegateTypeSymbol)
    {
        returnType = returnType->AddLvalueReference(span_);
    }
    SetReturnType(returnType);
    ComputeName();
}

void ArrayTypeElementAccess::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "element access needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::addr);
    void* ptr = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    emitter.SetCurrentDebugLocation(span);
    void* indexValue = emitter.Stack().Pop();
    void* elementPtr = emitter.CreateArrayIndexAddress(ptr, indexValue);
    TypeSymbol* elementType = arrayType->ElementType();
    if ((flags & OperationFlags::addr) == OperationFlags::none && (elementType->IsBasicTypeSymbol() || elementType->IsPointerType() || elementType->GetSymbolType() == SymbolType::delegateTypeSymbol))
    {
        void* elementValue = emitter.CreateLoad(elementPtr);
        emitter.Stack().Push(elementValue);
    }
    else
    {
        emitter.Stack().Push(elementPtr);
    }
}

void ArrayTypeElementAccess::Check()
{
    FunctionSymbol::Check();
    if (!arrayType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "array type element access has no array type", GetSpan());
    }
}

} } // namespace cmajor::symbols
