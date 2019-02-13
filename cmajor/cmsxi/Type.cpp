// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Type.hpp>
#include <cmajor/util/Error.hpp>
#include <string>

namespace cmsxi {

const char* primitiveTypeName[] =
{
    "", "void", "bool", "sbyte", "byte", "short", "ushort", "int", "uint", "long", "ulong", "float", "double", "char", "wchar", "uchar"
};

std::string TypeName(int typeId)
{
    if (typeId < 0)
    {
        return primitiveTypeName[-typeId];
    }
    else
    {
        return "$T" + std::to_string(typeId);
    }
}

Type::Type(int id_) : id(id_)
{
}

Type::~Type()
{
}

Value* Type::DefaultValue()
{
    Assert(false, "default value not implemented");
    return nullptr;
}

std::string Type::Name() const
{
    return TypeName(id);
}

void Type::Write(CodeFormatter& formatter)
{
    formatter.Write(Name());
}

void Type::WriteDeclaration(CodeFormatter& formatter)
{
    formatter.Write(Name());
    formatter.Write(" = type ");
}

PrimitiveType::PrimitiveType(int id_) : Type(id_)
{
}

VoidType::VoidType() : PrimitiveType(voidTypeId)
{
}

BoolType::BoolType() : PrimitiveType(boolTypeId)
{
}

SByteType::SByteType() : PrimitiveType(sbyteTypeId)
{
}

ByteType::ByteType() : PrimitiveType(byteTypeId)
{
}

ShortType::ShortType() : PrimitiveType(shortTypeId)
{
}

UShortType::UShortType() : PrimitiveType(ushortTypeId)
{
}

IntType::IntType() : PrimitiveType(intTypeId)
{
}

UIntType::UIntType() : PrimitiveType(uintTypeId)
{
}

LongType::LongType() : PrimitiveType(longTypeId)
{
}

ULongType::ULongType() : PrimitiveType(ulongTypeId)
{
}

FloatType::FloatType() : PrimitiveType(floatTypeId)
{
}

DoubleType::DoubleType() : PrimitiveType(doubleTypeId)
{
}

PtrType::PtrType(Type* baseType_) : Type(ptrTypeId), baseType(baseType_), defaultValue(this)
{
}

std::string PtrType::Name() const
{
    return baseType->Name() + "*";
}

StructureType::StructureType(int id_) : Type(id_)
{
}

void StructureType::SetMemberTypes(const std::vector<Type*>& memberTypes_)
{
    memberTypes = memberTypes_;
}

void StructureType::WriteDeclaration(CodeFormatter& formatter)
{
    Type::WriteDeclaration(formatter);
    formatter.Write("{ ");
    bool first = true;
    for (Type* memberType : memberTypes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        memberType->Write(formatter);
    }
    formatter.Write(" }");
}

Type* StructureType::GetMemberType(uint64_t index) const
{
    Assert(index < memberTypes.size(), "invalid member type index");
    return memberTypes[index];
}

size_t StructureTypeHash::operator()(const std::vector<Type*>& memberTypes) const
{
    size_t h = 0;
    for (Type* memberType : memberTypes)
    {
        h = h ^ std::hash<Type*>()(memberType);
    }
    return h;
}

size_t StructureTypeEqual::operator()(const std::vector<Type*>& leftMemberTypes, const std::vector<Type*>& rightMemberTypes) const
{
    return leftMemberTypes == rightMemberTypes;
}

ArrayType::ArrayType(int id_, Type* elementType_, uint64_t size_) : Type(id_), elementType(elementType_), size(size_)
{
}

void ArrayType::WriteDeclaration(CodeFormatter& formatter)
{
    Type::WriteDeclaration(formatter);
    formatter.Write("[");
    formatter.Write(std::to_string(size));
    formatter.Write(" x ");
    elementType->Write(formatter);
    formatter.Write("]");
}

size_t ArrayTypeKeyHash::operator()(const ArrayTypeKey& key) const
{
    return std::hash<Type*>()(key.elementType) ^ std::hash<uint64_t>()(key.size);
}

size_t ArrayTypeKeyEqual::operator()(const ArrayTypeKey& left, const ArrayTypeKey& right) const
{
    return left.elementType == right.elementType && left.size == right.size;
}

FunctionType::FunctionType(int id_, Type* returnType_, const std::vector<Type*>& paramTypes_) : Type(id_), returnType(returnType_), paramTypes(paramTypes_)
{
}

void FunctionType::WriteDeclaration(CodeFormatter& formatter)
{
    Type::WriteDeclaration(formatter);
    formatter.Write("function ");
    returnType->Write(formatter);
    formatter.Write("(");
    bool first = true;
    for (Type* paramType : paramTypes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        paramType->Write(formatter);
    }
    formatter.Write(")");
}

FunctionTypeKey::FunctionTypeKey(Type* returnType_, const std::vector<Type*>& paramTypes_) : returnType(returnType_), paramTypes(paramTypes_)
{
}

size_t FunctionTypeKeyHash::operator()(const FunctionTypeKey& key) const
{
    size_t h = std::hash<Type*>()(key.returnType);
    for (Type* paramType : key.paramTypes)
    {
        h = h ^ std::hash<Type*>()(paramType);
    }
    return h;
}

size_t FunctionTypeKeyEqual::operator()(const FunctionTypeKey& left, const FunctionTypeKey& right) const
{
    if (left.returnType != right.returnType) return false;
    return left.paramTypes == right.paramTypes;
}

TypeRepository::TypeRepository()
{
}

void TypeRepository::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("types");
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (const auto& t : types)
    {
        t->WriteDeclaration(formatter);
        formatter.WriteLine();
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();
}

Type* TypeRepository::GetPtrType(Type* baseType)
{
    auto it = ptrTypeMap.find(baseType);
    if (it != ptrTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        PtrType* ptrType = new PtrType(baseType);
        ptrTypeMap[baseType] = ptrType;
        ptrTypes.push_back(std::unique_ptr<PtrType>(ptrType));
        return ptrType;
    }
}

Type* TypeRepository::GetStructureType(const std::vector<Type*>& memberTypes)
{
    auto it = structureTypeMap.find(memberTypes);
    if (it != structureTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        StructureType* structureType = new StructureType(types.size());
        structureTypeMap[memberTypes] = structureType;
        types.push_back(std::unique_ptr<Type>(structureType));
        return structureType;
    }
}

Type* TypeRepository::CreateStructureType()
{
    StructureType* structureType = new StructureType(types.size());
    types.push_back(std::unique_ptr<Type>(structureType));
    return structureType;
}

Type* TypeRepository::GetArrayType(Type* elementType, uint64_t size)
{
    ArrayTypeKey key(elementType, size);
    auto it = arrayTypeMap.find(key);
    if (it != arrayTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        ArrayType* arrayType = new ArrayType(types.size(), elementType, size);
        arrayTypeMap[key] = arrayType;
        types.push_back(std::unique_ptr<Type>(arrayType));
        return arrayType;
    }
}

Type* TypeRepository::GetFunctionType(Type* returnType, const std::vector<Type*>& paramTypes)
{
    FunctionTypeKey key(returnType, paramTypes);
    auto it = functionTypeMap.find(key);
    if (it != functionTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        FunctionType* functionType = new FunctionType(types.size(), returnType, paramTypes);
        functionTypeMap[key] = functionType;
        types.push_back(std::unique_ptr<Type>(functionType));
        return functionType;
    }
}

} // namespace cmsxi
