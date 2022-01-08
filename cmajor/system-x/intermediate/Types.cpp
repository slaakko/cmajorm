// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Types.hpp>
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/Visitor.hpp>

namespace cmsx::intermediate {

Type::Type(const SourcePos& sourcePos_, TypeKind kind_, int32_t id_) : sourcePos(sourcePos_), kind(kind_), id(id_)
{
}

Type::~Type()
{
}

bool Type::IsIntegerType() const
{
    switch (id)
    {
        case sbyteTypeId:
        case byteTypeId:
        case shortTypeId:
        case ushortTypeId:
        case intTypeId:
        case uintTypeId:
        case longTypeId:
        case ulongTypeId:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

bool Type::IsUnsignedType() const
{
    switch (id)
    {
        case byteTypeId:
        case ushortTypeId:
        case uintTypeId:
        case ulongTypeId:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

bool Type::IsFloatingPointType() const
{
    switch (id)
    {
        case floatTypeId:
        case doubleTypeId:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

Type* Type::AddPointer(Context* context) const
{
    if (IsPointerType())
    {
        const PointerType* pointerType = static_cast<const PointerType*>(this);
        return context->GetTypes().MakePointerType(GetSourcePos(), GetBaseTypeId(pointerType->Id()), GetPointerCount(pointerType->Id()) + 1, context);
    }
    else
    {
        return context->GetTypes().MakePointerType(GetSourcePos(), Id(), 1, context);
    }
}

Type* Type::RemovePointer(const SourcePos& sourcePos, Context* context) const
{
    if (IsPointerType())
    {
        const PointerType* pointerType = static_cast<const PointerType*>(this);
        return pointerType->BaseType();
    }
    else
    {
        Error("pointer type expected", sourcePos, context);
    }
    return nullptr;
}

StructureType* Type::GetStructurePointeeType(const SourcePos& sourcePos, Context* context) const
{
    Type* pointeeType = RemovePointer(sourcePos, context);
    if (pointeeType)
    {
        if (pointeeType->IsStructureType())
        {
            return static_cast<StructureType*>(pointeeType);
        }
        else
        {
            Error("structure type expected", sourcePos, context);
        }
    }
    else
    {
        Error("pointer type expected", sourcePos, context);
    }
    return nullptr;
}

ArrayType* Type::GetArrayPointeeType(const SourcePos& sourcePos, Context* context) const
{
    Type* pointeeType = RemovePointer(sourcePos, context);
    if (pointeeType)
    {
        if (pointeeType->IsArrayType())
        {
            return static_cast<ArrayType*>(pointeeType);
        }
        else
        {
            Error("array type expected", sourcePos, context);
        }
    }
    else
    {
        Error("pointer type expected", sourcePos, context);
    }
    return nullptr;
}

void Type::Add(Types* types, Context* context)
{
}

void Type::Resolve(Types* types, Context* context)
{
}

cmsx::assembler::Instruction* Type::MakeAssemblyInst(Context* context) const
{
    Error("no assembly instruction for this type", sourcePos, context);
    return nullptr;
}

VoidType::VoidType() : Type(SourcePos(), TypeKind::fundamentalType, voidTypeId)
{
}

BoolType::BoolType() : Type(SourcePos(), TypeKind::fundamentalType, boolTypeId)
{
}

cmsx::assembler::Instruction* BoolType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::BYTE);
}

SByteType::SByteType() : Type(SourcePos(), TypeKind::fundamentalType, sbyteTypeId)
{
}

cmsx::assembler::Instruction* SByteType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::BYTE);
}

ByteType::ByteType() : Type(SourcePos(), TypeKind::fundamentalType, byteTypeId)
{
}

cmsx::assembler::Instruction* ByteType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::BYTE);
}

ShortType::ShortType() : Type(SourcePos(), TypeKind::fundamentalType, shortTypeId)
{
}

cmsx::assembler::Instruction* ShortType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::WYDE);
}

UShortType::UShortType() : Type(SourcePos(), TypeKind::fundamentalType, ushortTypeId)
{
}

cmsx::assembler::Instruction* UShortType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::WYDE);
}

IntType::IntType() : Type(SourcePos(), TypeKind::fundamentalType, intTypeId)
{
}

cmsx::assembler::Instruction* IntType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::TETRA);
}

UIntType::UIntType() : Type(SourcePos(), TypeKind::fundamentalType, uintTypeId)
{
}

cmsx::assembler::Instruction* UIntType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::TETRA);
}

LongType::LongType() : Type(SourcePos(), TypeKind::fundamentalType, longTypeId)
{
}

cmsx::assembler::Instruction* LongType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
}

ULongType::ULongType() : Type(SourcePos(), TypeKind::fundamentalType, ulongTypeId)
{
}

cmsx::assembler::Instruction* ULongType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
}

FloatType::FloatType() : Type(SourcePos(), TypeKind::fundamentalType, floatTypeId)
{
}

DoubleType::DoubleType() : Type(SourcePos(), TypeKind::fundamentalType, doubleTypeId)
{
}

TypeRef::TypeRef() : sourcePos(), id(-1), type(nullptr)
{
}

TypeRef::TypeRef(const SourcePos& sourcePos_, int32_t id_) : sourcePos(sourcePos_), id(id_), type(nullptr)
{
}

StructureType::StructureType(const SourcePos& sourcePos_, int32_t typeId_, const std::vector<TypeRef>& fieldTypeRefs_) :
    Type(sourcePos_, TypeKind::structureType, typeId_), fieldTypeRefs(fieldTypeRefs_), sizeAndOffsetsComputed(false)
{
}

void StructureType::Accept(Visitor& visitor)
{
    visitor.Visit(*this); 
}

void StructureType::Add(Types* types, Context* context)
{
    types->Add(this, context);
}

void StructureType::Resolve(Types* types, Context* context)
{
    for (auto& typeRef : fieldTypeRefs)
    {
        types->ResolveType(typeRef, context);
    }
}

bool StructureType::IsWeakType() const
{
    int n = FieldCount();
    for (int i = 0; i < n; ++i)
    {
        Type* type = FieldType(i);
        if (type->IsStructureType() || type->IsArrayType() || type->IsFunctionType())
        {
            return false;
        }
    }
    return true;
}

int64_t StructureType::Size() const 
{
    if (!sizeAndOffsetsComputed)
    {
        ComputeSizeAndOffsets();
    }
    return size;
}

void StructureType::ComputeSizeAndOffsets() const
{
    int64_t offset = 0;
    int n = FieldCount();
    for (int i = 0; i < n; ++i)
    {
        Type* fieldType = FieldType(i);
        int64_t memberOffset = offset;
        if (i > 0)
        {
            int64_t alignment = fieldType->Alignment();
            assert(offset > 0);
            memberOffset = alignment * ((offset - 1) / alignment + 1);
        }
        fieldOffsets.push_back(memberOffset);
        offset = memberOffset + fieldType->Size();
    }
    if (n == 0)
    {
        size = 8;
    }
    else
    {
        size = 8 * ((offset - 1) / 8 + 1);
    }
    sizeAndOffsetsComputed = true;
}

int64_t StructureType::GetFieldOffset(int64_t index) const
{
    if (!sizeAndOffsetsComputed)
    {
        ComputeSizeAndOffsets();
    }
    return fieldOffsets[index];
}

ArrayType::ArrayType(const SourcePos& sourcePos_, int32_t typeId_, int64_t elementCount_, const TypeRef& elementTypeRef_) :
    Type(sourcePos_, TypeKind::arrayType, typeId_), elementCount(elementCount_), elementTypeRef(elementTypeRef_)
{
}

void ArrayType::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ArrayType::Add(Types* types, Context* context)
{
    types->Add(this, context);
}

void ArrayType::Resolve(Types* types, Context* context)
{
    types->ResolveType(elementTypeRef, context);
}

bool ArrayType::IsWeakType() const
{
    Type* elementType = ElementType();
    if (elementType->IsStructureType() || elementType->IsArrayType() || elementType->IsFunctionType())
    {
        return false;
    }
    return true;
}

FunctionType::FunctionType(const SourcePos& sourcePos_, int32_t typeId_, const TypeRef& returnTypeRef_, const std::vector<TypeRef>& paramTypeRefs_) :
    Type(sourcePos_, TypeKind::functionType, typeId_), returnTypeRef(returnTypeRef_), paramTypeRefs(paramTypeRefs_)
{
}

void FunctionType::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void FunctionType::Add(Types* types, Context* context)
{
    types->Add(this, context);
}

void FunctionType::Resolve(Types* types, Context* context)
{
    types->ResolveType(returnTypeRef, context);
    for (auto& paramTypeRef : paramTypeRefs)
    {
        types->ResolveType(paramTypeRef, context);
    }
}

bool FunctionType::IsWeakType() const
{
    Type* returnType = ReturnType();
    if (returnType->IsStructureType() || returnType->IsArrayType() || returnType->IsFunctionType())
    {
        return false;
    }
    int n = Arity();
    for (int i = 0; i < n; ++i)
    {
        Type* paramType = ParamType(i);
        if (paramType->IsStructureType() || paramType->IsArrayType() || paramType->IsFunctionType())
        {
            return false;
        }
    }
    return true;
}

PointerType::PointerType(const SourcePos& sourcePos_, int32_t typeId_, int8_t pointerCount_, int32_t baseTypeId_) :
    Type(sourcePos_, TypeKind::pointerType, typeId_), pointerCount(pointerCount_), baseTypeRef(sourcePos_, baseTypeId_)
{
}

std::string PointerType::Name() const
{
    return baseTypeRef.GetType()->Name() + "*";
}

cmsx::assembler::Instruction* PointerType::MakeAssemblyInst(Context* context) const
{
    return new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
}

Types::Types() : context(nullptr)
{
}

void Types::AddStructureType(const SourcePos& sourcePos, int32_t typeId, const std::vector<TypeRef>& fieldTypeRefs)
{
    types.push_back(std::unique_ptr<Type>(new StructureType(sourcePos, typeId, fieldTypeRefs)));
}

void Types::AddArrayType(const SourcePos& sourcePos, int32_t typeId, int64_t size, const TypeRef& elementTypeRef)
{
    types.push_back(std::unique_ptr<Type>(new ArrayType(sourcePos, typeId, size, elementTypeRef)));
}

void Types::AddFunctionType(const SourcePos& sourcePos, int32_t typeId, const TypeRef& returnTypeRef, const std::vector<TypeRef>& paramTypeRefs)
{
    types.push_back(std::unique_ptr<Type>(new FunctionType(sourcePos, typeId, returnTypeRef, paramTypeRefs)));
}

void Types::Add(Type* type, Context* context)
{
    Type* prev = Get(type->Id());
    if (prev)
    {
        Error("error adding type id " + std::to_string(type->Id()) + ": type id not unique", type->GetSourcePos(), context, prev->GetSourcePos());
    }
    Map(type);
    declaratedTypes.push_back(type);
}

Type* Types::Get(int32_t id) const
{
    if (IsFundamentalTypeId(id))
    {
        return GetFundamentalType(id);
    }
    auto it = typeMap.find(id);
    if (it != typeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Type* Types::GetFundamentalType(int32_t id) const
{
    switch (id)
    {
        case voidTypeId: return const_cast<VoidType*>(&voidType);
        case boolTypeId: return const_cast<BoolType*>(&boolType);
        case sbyteTypeId: return const_cast<SByteType*>(&sbyteType);
        case byteTypeId: return const_cast<ByteType*>(&byteType);
        case shortTypeId: return const_cast<ShortType*>(&shortType);
        case ushortTypeId: return const_cast<UShortType*>(&ushortType);
        case intTypeId: return const_cast<IntType*>(&intType);
        case uintTypeId: return const_cast<UIntType*>(&uintType);
        case longTypeId: return const_cast<LongType*>(&longType);
        case ulongTypeId: return const_cast<ULongType*>(&ulongType);
        case floatTypeId: return const_cast<FloatType*>(&floatType);
        case doubleTypeId: return const_cast<DoubleType*>(&doubleType);
    }
    return nullptr;
}

void Types::Map(Type* type) 
{
    typeMap[type->Id()] = type;
}

void Types::VisitTypeDeclarations(Visitor& visitor)
{
    for (Type* declaredType : declaratedTypes)
    {
        declaredType->Accept(visitor);
    }
}

PointerType* Types::MakePointerType(const SourcePos& sourcePos, int32_t baseTypeId, int8_t pointerCount, Context* context)
{
    auto it = pointerTypeMap.find(std::make_pair(baseTypeId, pointerCount));
    if (it != pointerTypeMap.cend())
    {
        return it->second;
    }
    PointerType* type = nullptr;
    if (pointerCount > 1)
    {
        type = new PointerType(sourcePos, MakePointerTypeId(baseTypeId, pointerCount), pointerCount, MakePointerTypeId(baseTypeId, pointerCount - 1));
    }
    else if (pointerCount == 1)
    {
        type = new PointerType(sourcePos, MakePointerTypeId(baseTypeId, pointerCount), pointerCount, baseTypeId);
    }
    else
    {
        Error("pointer count > 0 expected", sourcePos, context);
    }
    ResolveType(type->BaseTypeRef(), context);
    types.push_back(std::unique_ptr<Type>(type));
    Map(type);
    pointerTypeMap[std::make_pair(baseTypeId, pointerCount)] = type;
    return type;
}

void Types::Resolve(Context* context)
{
    for (const auto& type : types)
    {
        type->Add(this, context);
    }
    std::vector<Type*> toResolve;
    for (const auto& type : types)
    {
        toResolve.push_back(type.get());
    }
    for (Type* type : toResolve)
    {
        type->Resolve(this, context);
    }
}

void Types::ResolveType(TypeRef& typeRef, Context* context)
{
    Type* type = Get(typeRef.Id());
    if (!type)
    {
        if (IsPointerTypeId(typeRef.Id()))
        {
            type = MakePointerType(typeRef.GetSourcePos(), GetBaseTypeId(typeRef.Id()), GetPointerCount(typeRef.Id()), context);
        }
    }
    if (!type)
    {
        Error("error resolving type: type id " + std::to_string(typeRef.Id()) + " not found", typeRef.GetSourcePos(), context);
    }
    typeRef.SetType(type);
}

} // cmsx::intermediate