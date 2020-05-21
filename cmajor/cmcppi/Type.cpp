// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Type.hpp>
#include <soulng/util/Error.hpp>
#include <algorithm>
#include <set>
#include <unordered_set>

namespace cmcppi {

const char* primitiveTypeName[] =
{
    "", "void", "bool", "int8_t", "uint8_t", "int16_t", "uint16_t", "int32_t", "uint32_t", "int64_t", "uint64_t", "float", "double", "char8_t", "char16_t", "char32_t"
};

std::string TypeName(int typeId)
{
    if (typeId < 0)
    {
        return primitiveTypeName[-typeId];
    }
    else
    {
        return "__T" + std::to_string(typeId);
    }
}

Type::Type(int id_) : id(id_)
{
}

Type::~Type()
{
}

std::string Type::Name() const
{
    return TypeName(id);
}

ConstantValue* Type::DefaultValue()
{
    Assert(false, "default value not implemented");
    return nullptr;
}

void Type::WriteForwardDeclaration(CodeFormatter& formatter)
{
}

void Type::WriteDeclaration(CodeFormatter& formatter)
{
    formatter.Write("using " + Name() + " = ");
}

void Type::Write(CodeFormatter& formatter)
{
    formatter.Write(Name());
}

PrimitiveType::PrimitiveType(int id) : Type(id)
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

CharType::CharType() : PrimitiveType(charTypeId)
{
}

WCharType::WCharType() : PrimitiveType(wcharTypeId)
{
}

UCharType::UCharType() : PrimitiveType(ucharTypeId)
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

void StructureType::WriteForwardDeclaration(CodeFormatter& formatter)
{
    formatter.WriteLine("struct __struct" + std::to_string(Id()) + ";");
    formatter.WriteLine("using " + Name() + " = struct __struct" + std::to_string(Id()) + ";");
}

void StructureType::WriteDeclaration(CodeFormatter& formatter)
{
    int memberIndex = 0;
    Type::WriteDeclaration(formatter);
    formatter.Write("struct __struct" + std::to_string(Id()) + " { ");
    bool first = true;
    for (Type* memberType : memberTypes)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(" ");
        }
        memberType->Write(formatter);
        formatter.Write(" m" + std::to_string(memberIndex) + ";");
        ++memberIndex;
    }
    formatter.Write(" };");
}

Type* StructureType::GetMemberType(uint64_t index) const
{
    Assert(index < memberTypes.size(), "invalid member type index");
    return memberTypes[index];
}

int StructureType::SizeInBytes() const
{
    int size = 0;
    for (Type* memberType : memberTypes)
    {
        size = size + memberType->SizeInBytes();
    }
    return size;
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
    elementType->Write(formatter);
    formatter.Write("[");
    formatter.Write(std::to_string(size));
    formatter.Write("];");
}

int ArrayType::SizeInBytes() const
{
    return size * elementType->SizeInBytes();
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
    returnType->Write(formatter);
    formatter.Write(" (*)(");
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
    formatter.Write(");");
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

void Visit(std::vector<Type*>& order, Type* type, std::unordered_set<Type*>& visited, std::unordered_set<Type*>& tempVisit, const std::unordered_map<Type*, std::set<Type*>>& dependencies)
{
    if (tempVisit.find(type) == tempVisit.end())
    {
        if (visited.find(type) == visited.end())
        {
            tempVisit.insert(type);
            auto i = dependencies.find(type);
            if (i != dependencies.end())
            {
                const std::set<Type*>& dependsOn = i->second;
                for (Type* type : dependsOn)
                {
                    Visit(order, type, visited, tempVisit, dependencies);
                }
                tempVisit.erase(type);
                visited.insert(type);
                order.push_back(type);
            }
            else
            {
                tempVisit.erase(type);
                visited.insert(type);
                order.push_back(type);
            }
        }
    }
    else
    {
        throw std::runtime_error("circular type dependency '" + type->Name() + "' detected");
    }
}

std::vector<Type*> CreateTypeOrder(const std::vector<std::unique_ptr<Type>>& types)
{
    std::unordered_map<Type*, std::set<Type*>> dependencies;
    for (const std::unique_ptr<Type>& type : types)
    {
        if (type->IsStructureType())
        {
            StructureType* structureType = static_cast<StructureType*>(type.get());
            int n = structureType->MemberTypes().size();
            for (int i = 0; i < n; ++i)
            {
                Type* memberType = structureType->GetMemberType(i);
                if (memberType->IsStructureType())
                {
                    dependencies[type.get()].insert(memberType);
                }
            }
        }
    }
    std::vector<Type*> order;
    std::unordered_set<Type*> visited;
    std::unordered_set<Type*> tempVisit;
    for (const std::unique_ptr<Type>& type : types)
    {
        if (visited.find(type.get()) == visited.end())
        {
            Visit(order, type.get(), visited, tempVisit, dependencies);
        }
    }
    return order;
}

TypeRepository::TypeRepository()
{
}

void TypeRepository::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("#include <stdint.h>");
    formatter.WriteLine("#ifndef __cpp_char8_t");
    formatter.WriteLine("using char8_t = unsigned char;");
    formatter.WriteLine("#endif");
    formatter.WriteLine();
    formatter.WriteLine("namespace");
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (const std::unique_ptr<Type>& type : types)
    {
        type->WriteForwardDeclaration(formatter);
    }
    for (const std::unique_ptr<Type>& type : types)
    {
        if (type->IsStructureType())
        {
            continue;
        }
        type->WriteDeclaration(formatter);
        formatter.WriteLine();
    }
    std::vector<Type*> typeOrder = CreateTypeOrder(types);
    for (Type* type : typeOrder)
    {
        if (!(type->IsStructureType()))
        {
            continue;
        }
        type->WriteDeclaration(formatter);
        formatter.WriteLine();
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
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
        structureType->SetMemberTypes(memberTypes);
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

} // namespace cmcppi
