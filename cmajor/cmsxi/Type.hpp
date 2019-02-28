// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_TYPE_INCLUDED
#define CMAJOR_CMSXI_TYPE_INCLUDED
#include <cmajor/cmsxi/Value.hpp>
#include <cmajor/util/CodeFormatter.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace cmsxi {

using namespace cmajor::util;

const int voidTypeId = -1;
const int boolTypeId = -2;
const int sbyteTypeId = -3;
const int byteTypeId = -4;
const int shortTypeId = -5;
const int ushortTypeId = -6;
const int intTypeId = -7;
const int uintTypeId = -8;
const int longTypeId = -9;
const int ulongTypeId = -10;
const int floatTypeId = -11;
const int doubleTypeId = -12;
const int ptrTypeId = -13;

std::string TypeName(int typeId);

class CMSXI_API Type
{
public:
    Type(int id_);
    virtual ~Type();
    virtual std::string Name() const;
    virtual ConstantValue* DefaultValue();
    int Id() const { return id; }
    void Write(CodeFormatter& formatter);
    virtual void WriteDeclaration(CodeFormatter& formatter);
    bool IsPtrType() const { return id == ptrTypeId; }
    bool IsVoidType() const { return id == voidTypeId; }
    virtual bool IsStructureType() const { return false; }
    virtual bool IsArrayType() const { return false; }
    virtual bool IsFunctionType() const { return false; }
private:
    int id;
};

class CMSXI_API PrimitiveType : public Type
{
public:
    PrimitiveType(int id_);
};

class CMSXI_API VoidType : public PrimitiveType
{
public:
    VoidType();
};

class CMSXI_API BoolType : public PrimitiveType
{
public:
    BoolType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    BoolValue defaultValue;
};

class CMSXI_API SByteType : public PrimitiveType
{
public:
    SByteType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    SByteValue defaultValue;
};

class CMSXI_API ByteType : public PrimitiveType
{
public:
    ByteType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    ByteValue defaultValue;
};

class CMSXI_API ShortType : public PrimitiveType
{
public:
    ShortType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    ShortValue defaultValue;
};

class CMSXI_API UShortType : public PrimitiveType
{
public:
    UShortType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    UShortValue defaultValue;
};

class CMSXI_API IntType : public PrimitiveType
{
public:
    IntType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    IntValue defaultValue;
};

class CMSXI_API UIntType : public PrimitiveType
{
public:
    UIntType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    UIntValue defaultValue;
};

class CMSXI_API LongType : public PrimitiveType
{
public:
    LongType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    LongValue defaultValue;
};

class CMSXI_API ULongType : public PrimitiveType
{
public:
    ULongType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    ULongValue defaultValue;
};

class CMSXI_API FloatType : public PrimitiveType
{
public:
    FloatType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    FloatValue defaultValue;
};

class CMSXI_API DoubleType : public PrimitiveType
{
public:
    DoubleType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
private:
    DoubleValue defaultValue;
};

class CMSXI_API PtrType : public Type
{
public:
    PtrType(Type* baseType_);
    std::string Name() const override;
    ConstantValue* DefaultValue() override { return &defaultValue; }
    Type* BaseType() const { return baseType; }
private:
    Type* baseType;
    NullValue defaultValue;
};

class CMSXI_API StructureType : public Type
{
public:
    StructureType(int id_);
    const std::vector<Type*>& MemberTypes() const { return memberTypes; }
    void SetMemberTypes(const std::vector<Type*>& memberTypes_);
    void WriteDeclaration(CodeFormatter& formatter) override;
    bool IsStructureType() const { return true; }
    Type* GetMemberType(uint64_t index) const;
private:
    std::vector<Type*> memberTypes;
};

struct CMSXI_API StructureTypeHash
{
    size_t operator()(const std::vector<Type*>& memberTypes) const;
};

struct CMSXI_API StructureTypeEqual
{
    size_t operator()(const std::vector<Type*>& leftMemberTypes, const std::vector<Type*>& rightMemberTypes) const;
};

class CMSXI_API ArrayType : public Type
{
public:
    ArrayType(int id_, Type* elementType_, uint64_t size_);
    void WriteDeclaration(CodeFormatter& formatter) override;
    bool IsArrayType() const { return true; }
    Type* ElementType() const { return elementType; }
private:
    Type* elementType;
    uint64_t size;
};

struct CMSXI_API ArrayTypeKey
{
    ArrayTypeKey(Type* elementType_, uint64_t size_) : elementType(elementType_), size(size_) {}
    Type* elementType;
    uint64_t size;
};

struct CMSXI_API ArrayTypeKeyHash
{
    size_t operator()(const ArrayTypeKey& key) const;
};

struct CMSXI_API ArrayTypeKeyEqual
{
    size_t operator()(const ArrayTypeKey& left, const ArrayTypeKey& right) const;
};

class CMSXI_API FunctionType : public Type
{
public:
    FunctionType(int id_, Type* returnType_, const std::vector<Type*>& paramTypes_);
    void WriteDeclaration(CodeFormatter& formatter) override;
    bool IsFunctionType() const override { return true; }
    Type* ReturnType() const { return returnType; }
    const std::vector<Type*>& ParamTypes() const { return paramTypes; }
private:
    Type* returnType;
    std::vector<Type*> paramTypes;
};

struct CMSXI_API FunctionTypeKey
{
    FunctionTypeKey(Type* returnType_, const std::vector<Type*>& paramTypes_);
    Type* returnType;
    std::vector<Type*> paramTypes;
};

struct CMSXI_API FunctionTypeKeyHash
{
    size_t operator()(const FunctionTypeKey& key) const;
};

struct CMSXI_API FunctionTypeKeyEqual
{
    size_t operator()(const FunctionTypeKey& left, const FunctionTypeKey& right) const;
};

class CMSXI_API TypeRepository
{
public:
    TypeRepository();
    TypeRepository(const TypeRepository&) = delete;
    TypeRepository& operator=(const TypeRepository&) = delete;
    void Write(CodeFormatter& formatter);
    Type* GetVoidType() { return &voidType; }
    Type* GetBoolType() { return &boolType; }
    Type* GetSByteType() { return &sbyteType; }
    Type* GetByteType() { return &byteType; }
    Type* GetShortType() { return &shortType; }
    Type* GetUShortType() { return &ushortType; }
    Type* GetIntType() { return &intType; }
    Type* GetUIntType() { return &uintType; }
    Type* GetLongType() { return &longType; }
    Type* GetULongType() { return &ulongType; }
    Type* GetFloatType() { return &floatType; }
    Type* GetDoubleType() { return &doubleType; }
    Type* GetPtrType(Type* baseType);
    Type* GetStructureType(const std::vector<Type*>& memberTypes);
    Type* CreateStructureType();
    Type* GetArrayType(Type* elementType, uint64_t size);
    Type* GetFunctionType(Type* returnType, const std::vector<Type*>& paramTypes);
private:
    VoidType voidType;
    BoolType boolType;
    SByteType sbyteType;
    ByteType byteType;
    ShortType shortType;
    UShortType ushortType;
    IntType intType;
    UIntType uintType;
    LongType longType;
    ULongType ulongType;
    FloatType floatType;
    DoubleType doubleType;
    std::vector<std::unique_ptr<PtrType>> ptrTypes;
    std::unordered_map<Type*, PtrType*> ptrTypeMap;
    std::unordered_map<std::vector<Type*>, StructureType*, StructureTypeHash, StructureTypeEqual> structureTypeMap;
    std::unordered_map<ArrayTypeKey, ArrayType*, ArrayTypeKeyHash, ArrayTypeKeyEqual> arrayTypeMap;
    std::unordered_map<FunctionTypeKey, FunctionType*, FunctionTypeKeyHash, FunctionTypeKeyEqual> functionTypeMap;
    std::vector<std::unique_ptr<Type>> types;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_TYPE_INCLUDED
