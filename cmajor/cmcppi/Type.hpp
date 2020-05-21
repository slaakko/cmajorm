// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_TYPE_INCLUDED
#define CMAJOR_CMCPPI_TYPE_INCLUDED
#include <cmajor/cmcppi/Value.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <string>
#include <unordered_map>

namespace cmcppi {

using namespace soulng::util;

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
const int charTypeId = -13;
const int wcharTypeId = -14;
const int ucharTypeId = -15;
const int ptrTypeId = -16;

std::string TypeName(int typeId);

class CMCPPI_API Type
{
public:
    Type(int id_);
    virtual ~Type();
    virtual std::string Name() const;
    virtual ConstantValue* DefaultValue();
    virtual void WriteForwardDeclaration(CodeFormatter& formatter);
    virtual void WriteDeclaration(CodeFormatter& formatter);
    virtual bool IsPrimitiveType() const { return false; }
    virtual bool IsStructureType() const { return false; }
    virtual bool IsArrayType() const { return false; }
    virtual bool IsFunctionType() const { return false; }
    virtual int SizeInBytes() const = 0;
    bool IsPtrType() const { return id == ptrTypeId; }
    bool IsVoidType() const { return id == voidTypeId; }
    int Id() const { return id; }
    void Write(CodeFormatter& formatter);
private:
    int id;
};

class CMCPPI_API PrimitiveType : public Type
{
public:
    PrimitiveType(int id);
    bool IsPrimitiveType() const override { return true; }
};

class CMCPPI_API VoidType : public PrimitiveType
{
public:
    VoidType();
    int SizeInBytes() const override { return 0; }
};

class CMCPPI_API BoolType : public PrimitiveType
{
public:
    BoolType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 1; }
private:
    BoolValue defaultValue;
};

class CMCPPI_API SByteType : public PrimitiveType
{
public:
    SByteType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 1; }
private:
    SByteValue defaultValue;
};

class CMCPPI_API ByteType : public PrimitiveType
{
public:
    ByteType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 1; }
private:
    ByteValue defaultValue;
};

class CMCPPI_API ShortType : public PrimitiveType
{
public:
    ShortType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 2; }
private:
    ShortValue defaultValue;
};

class CMCPPI_API UShortType : public PrimitiveType
{
public:
    UShortType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 2; }
private:
    UShortValue defaultValue;
};

class CMCPPI_API IntType : public PrimitiveType
{
public:
    IntType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 4; }
private:
    IntValue defaultValue;
};

class CMCPPI_API UIntType : public PrimitiveType
{
public:
    UIntType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 4; }
private:
    UIntValue defaultValue;
};

class CMCPPI_API LongType : public PrimitiveType
{
public:
    LongType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 8; }
private:
    LongValue defaultValue;
};

class CMCPPI_API ULongType : public PrimitiveType
{
public:
    ULongType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 8; }
private:
    ULongValue defaultValue;
};

class CMCPPI_API FloatType : public PrimitiveType
{
public:
    FloatType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 4; }
private:
    FloatValue defaultValue;
};

class CMCPPI_API DoubleType : public PrimitiveType
{
public:
    DoubleType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 8; }
private:
    DoubleValue defaultValue;
};

class CMCPPI_API CharType : public PrimitiveType
{
public:
    CharType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 8; }
private:
    CharValue defaultValue;
};

class CMCPPI_API WCharType : public PrimitiveType
{
public:
    WCharType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 16; }
private:
    WCharValue defaultValue;
};

class CMCPPI_API UCharType : public PrimitiveType
{
public:
    UCharType();
    ConstantValue* DefaultValue() override { return &defaultValue; }
    int SizeInBytes() const override { return 32; }
private:
    UCharValue defaultValue;
};

class CMCPPI_API PtrType : public Type
{
public:
    PtrType(Type* baseType_);
    std::string Name() const override;
    ConstantValue* DefaultValue() override { return &defaultValue; }
    Type* BaseType() const { return baseType; }
    int SizeInBytes() const override { return 8; }
private:
    Type* baseType;
    NullValue defaultValue;
};

class CMCPPI_API StructureType : public Type
{
public:
    StructureType(int id_);
    const std::vector<Type*>& MemberTypes() const { return memberTypes; }
    void SetMemberTypes(const std::vector<Type*>& memberTypes_);
    void WriteForwardDeclaration(CodeFormatter& formatter);
    void WriteDeclaration(CodeFormatter& formatter) override;
    bool IsStructureType() const { return true; }
    Type* GetMemberType(uint64_t index) const;
    int SizeInBytes() const override;
private:
    std::vector<Type*> memberTypes;
};

struct CMCPPI_API StructureTypeHash
{
    size_t operator()(const std::vector<Type*>& memberTypes) const;
};

struct CMCPPI_API StructureTypeEqual
{
    size_t operator()(const std::vector<Type*>& leftMemberTypes, const std::vector<Type*>& rightMemberTypes) const;
};

class CMCPPI_API ArrayType : public Type
{
public:
    ArrayType(int id_, Type* elementType_, uint64_t size_);
    void WriteDeclaration(CodeFormatter& formatter) override;
    bool IsArrayType() const { return true; }
    Type* ElementType() const { return elementType; }
    int SizeInBytes() const override;
    uint64_t Size() const { return size; }
private:
    Type* elementType;
    uint64_t size;
};

struct CMCPPI_API ArrayTypeKey
{
    ArrayTypeKey(Type* elementType_, uint64_t size_) : elementType(elementType_), size(size_) {}
    Type* elementType;
    uint64_t size;
};

struct CMCPPI_API ArrayTypeKeyHash
{
    size_t operator()(const ArrayTypeKey& key) const;
};

struct CMCPPI_API ArrayTypeKeyEqual
{
    size_t operator()(const ArrayTypeKey& left, const ArrayTypeKey& right) const;
};

class CMCPPI_API FunctionType : public Type
{
public:
    FunctionType(int id_, Type* returnType_, const std::vector<Type*>& paramTypes_);
    void WriteDeclaration(CodeFormatter& formatter) override;
    bool IsFunctionType() const override { return true; }
    Type* ReturnType() const { return returnType; }
    const std::vector<Type*>& ParamTypes() const { return paramTypes; }
    int SizeInBytes() const override { return 0; }
private:
    Type* returnType;
    std::vector<Type*> paramTypes;
};

struct CMCPPI_API FunctionTypeKey
{
    FunctionTypeKey(Type* returnType_, const std::vector<Type*>& paramTypes_);
    Type* returnType;
    std::vector<Type*> paramTypes;
};

struct CMCPPI_API FunctionTypeKeyHash
{
    size_t operator()(const FunctionTypeKey& key) const;
};

struct CMCPPI_API FunctionTypeKeyEqual
{
    size_t operator()(const FunctionTypeKey& left, const FunctionTypeKey& right) const;
};

class CMCPPI_API TypeRepository
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
    Type* GetCharType() { return &charType; }
    Type* GetWCharType() { return &wcharType; }
    Type* GetUCharType() { return &ucharType; }
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
    CharType charType;
    WCharType wcharType;
    UCharType ucharType;
    std::vector<std::unique_ptr<PtrType>> ptrTypes;
    std::unordered_map<Type*, PtrType*> ptrTypeMap;
    std::unordered_map<std::vector<Type*>, StructureType*, StructureTypeHash, StructureTypeEqual> structureTypeMap;
    std::unordered_map<ArrayTypeKey, ArrayType*, ArrayTypeKeyHash, ArrayTypeKeyEqual> arrayTypeMap;
    std::unordered_map<FunctionTypeKey, FunctionType*, FunctionTypeKeyHash, FunctionTypeKeyEqual> functionTypeMap;
    std::vector<std::unique_ptr<Type>> types;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_TYPE_INCLUDED
