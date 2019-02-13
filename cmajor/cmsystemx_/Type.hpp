// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_TYPE_INCLUDED
#define CMAJOR_CMSX_TYPE_INCLUDED
#include <cmajor/cmsystemx/CmSystemxApi.hpp>
#include <cmajor/cmsystemx/Value.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace cmsystemx {

const int voidTypeId = 0;
const int sbyteTypeId = 1;
const int byteTypeId = 2;
const int shortTypeId = 3;
const int ushortTypeId = 4;
const int intTypeId = 5;
const int uintTypeId = 6;
const int longTypeId = 7;
const int ulongTypeId = 8;
const int boolTypeId = 9;
const int charTypeId = 10;
const int wcharTypeId = 11;
const int ucharTypeId = 12;
const int floatTypeId = 13;
const int doubleTypeId = 14;
const int firstUserTypeId = 15;

class Value;
class Instruction;
class Context;

enum class TypeKind
{
    voidType, sbyteType, byteType, shortType, ushortType, intType, uintType, longType, ulongType, boolType, charType, wcharType, ucharType, floatType, doubleType, ptrType, functionType, classType
};

class CMSYSTEMX_API Type
{
public:
    Type(TypeKind kind_);
    Type(TypeKind kind_, int id_);
    virtual ~Type();
    virtual Value* DefaultValue() = 0;
    virtual Value* MinusOne() = 0;
    virtual Value* GetValue(int64_t value, Context& context) = 0;
    virtual Instruction* CreateLoad(Context& context) = 0;
    virtual Instruction* CreateStore(Context& context) = 0;
    virtual uint64_t SizeInBits() const { return -1; }
    virtual uint64_t AlignmentInBits() const { return -1; }
    virtual Value* CreateReturnValue(Context& context);
    virtual bool IsUnsigned() const { return false; }
    virtual bool IsScalar() const { return false; }
    int Id() const { return id; }
    TypeKind GetKind() const { return kind; }
private:
    TypeKind kind;
    int id;
};

class CMSYSTEMX_API VoidType : public Type
{
public:
    VoidType();
    Value* DefaultValue() override;
    Value* MinusOne() override;
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    Value* CreateReturnValue(Context& context) override;
};

class CMSYSTEMX_API SByteType : public Type
{
public:
    SByteType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 8; }
    uint64_t AlignmentInBits() const override { return 8; }
private:
    SByteValue defaultValue;
    SByteValue minusOne;
};

class CMSYSTEMX_API ByteType : public Type
{
public:
    ByteType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 8; }
    uint64_t AlignmentInBits() const override { return 8; }
    bool IsUnsigned() const override { return true; }
private:
    ByteValue defaultValue;
    ByteValue minusOne;
};

class CMSYSTEMX_API ShortType : public Type
{
public:
    ShortType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 16; }
    uint64_t AlignmentInBits() const override { return 16; }
private:
    ShortValue defaultValue;
    ShortValue minusOne;
};

class CMSYSTEMX_API UShortType : public Type
{
public:
    UShortType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 16; }
    uint64_t AlignmentInBits() const override { return 16; }
    bool IsUnsigned() const override { return true; }
private:
    UShortValue defaultValue;
    UShortValue minusOne;
};

class CMSYSTEMX_API IntType : public Type
{
public:
    IntType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 32; }
    uint64_t AlignmentInBits() const override { return 32; }
private:
    IntValue defaultValue;
    IntValue minusOne;
};

class CMSYSTEMX_API UIntType : public Type
{
public:
    UIntType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 32; }
    uint64_t AlignmentInBits() const override { return 32; }
    bool IsUnsigned() const override { return true; }
private:
    UIntValue defaultValue;
    UIntValue minusOne;
};

class CMSYSTEMX_API LongType : public Type
{
public:
    LongType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 64; }
    uint64_t AlignmentInBits() const override { return 64; }
private:
    LongValue defaultValue;
    LongValue minusOne;
};

class CMSYSTEMX_API ULongType : public Type
{
public:
    ULongType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 64; }
    uint64_t AlignmentInBits() const override { return 64; }
    bool IsUnsigned() const override { return true; }
private:
    ULongValue defaultValue;
    ULongValue minusOne;
};

class CMSYSTEMX_API BoolType : public Type
{
public:
    BoolType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 8; }
    uint64_t AlignmentInBits() const override { return 8; }
    bool IsUnsigned() const override { return true; }
private:
    BoolValue defaultValue;
    BoolValue minusOne;
};

class CMSYSTEMX_API CharType : public Type
{
public:
    CharType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 8; }
    uint64_t AlignmentInBits() const override { return 8; }
    bool IsUnsigned() const override { return true; }
private:
    CharValue defaultValue;
    CharValue minusOne;
};

class CMSYSTEMX_API WCharType : public Type
{
public:
    WCharType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 16; }
    uint64_t AlignmentInBits() const override { return 16; }
    bool IsUnsigned() const override { return true; }
private:
    WCharValue defaultValue;
    WCharValue minusOne;
};

class CMSYSTEMX_API UCharType : public Type
{
public:
    UCharType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override { return &minusOne; }
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 32; }
    uint64_t AlignmentInBits() const override { return 32; }
    bool IsUnsigned() const override { return true; }
private:
    UCharValue defaultValue;
    UCharValue minusOne;
};

class CMSYSTEMX_API FloatType : public Type
{
public:
    FloatType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override;
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 32; }
    uint64_t AlignmentInBits() const override { return 32; }
private:
    FloatValue defaultValue;
};

class CMSYSTEMX_API DoubleType : public Type
{
public:
    DoubleType();
    bool IsScalar() const override { return true; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override;
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 64; }
    uint64_t AlignmentInBits() const override { return 64; }
private:
    DoubleValue defaultValue;
};

class CMSYSTEMX_API PtrType : public Type
{
public:
    PtrType(int id_, Type* baseType_);
    bool IsScalar() const override { return true; }
    Type* BaseType() const { return baseType; }
    Value* DefaultValue() override { return &defaultValue; }
    Value* MinusOne() override;
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    uint64_t SizeInBits() const override { return 64; }
    uint64_t AlignmentInBits() const override { return 64; }
private:
    Type* baseType;
    NullValue defaultValue;
};

class CMSYSTEMX_API FunctionType : public Type
{
public:
    FunctionType();
    FunctionType(Type* returnType_, const std::vector<Type*>& paramTypes_);
    Value* DefaultValue() override;
    Value* MinusOne() override;
    Value* GetValue(int64_t value, Context& context) override;
    Type* ReturnType() const { return returnType; }
    const std::vector<Type*>& ParamTypes() const { return paramTypes; }
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
private:
    Type* returnType;
    std::vector<Type*> paramTypes;
};

struct CMSYSTEMX_API FunctionTypeEqual
{
    bool operator()(FunctionType* left, FunctionType* right) const;
};

struct CMSYSTEMX_API FunctionTypeHash
{
    size_t operator()(FunctionType* x) const;
};

struct CMSYSTEMX_API PtrTypeEqual
{
    bool operator()(PtrType* left, PtrType* right) const;
};

struct CMSYSTEMX_API PtrTypeHash
{
    size_t operator()(PtrType* x) const;
};

struct CMSYSTEMX_API MemberType
{
    MemberType(Type* type_, uint64_t offset_) : type(type_), offset(offset_) { }
    Type* type;
    uint64_t offset;
};

bool operator==(const MemberType& left, const MemberType& right);
inline bool operator!=(const MemberType& left, const MemberType& right)
{
    return !(left == right);
}

struct CMSYSTEMX_API MemberTypeHash
{
    size_t operator()(const MemberType& x) const;
};

class CMSYSTEMX_API ClassType : public Type
{
public:
    ClassType(int id_, const std::vector<MemberType>& memberTypes_, uint64_t sizeInBytes_);
    Value* DefaultValue() override;
    Value* MinusOne() override;
    Value* GetValue(int64_t value, Context& context) override;
    Instruction* CreateLoad(Context& context) override;
    Instruction* CreateStore(Context& context) override;
    const std::vector<MemberType>& MemberTypes() const { return memberTypes; }
    uint64_t SizeInBits() const { return 8 * sizeInBytes; }
    uint64_t AlignmentInBits() const { return 64; }
private:
    std::vector<MemberType> memberTypes;
    uint64_t sizeInBytes;
};

struct CMSYSTEMX_API ClassTypeHash
{
    size_t operator()(ClassType* x) const;
};

struct CMSYSTEMX_API ClassTypeEqual
{
    bool operator()(ClassType* left, ClassType* right) const;
};

class CMSYSTEMX_API TypeMap
{
public:
    TypeMap();
    TypeMap(const TypeMap&) = delete;
    TypeMap& operator=(const TypeMap&) = delete;
    FunctionType* GetFunctionType(Type* returnType, const std::vector<Type*>& parameterTypes);
    Type* GetVoidType() { return &voidType; }
    Type* GetSByteType() { return &sbyteType; }
    Type* GetByteType() { return &byteType; }
    Type* GetShortType() { return &shortType; }
    Type* GetUShortType() { return &ushortType; }
    Type* GetIntType() { return &intType; }
    Type* GetUIntType() { return &uintType; }
    Type* GetLongType() { return &longType; }
    Type* GetULongType() { return &ulongType; }
    Type* GetBoolType() { return &boolType; }
    Type* GetCharType() { return &charType; }
    Type* GetWCharType() { return &wcharType; }
    Type* GetUCharType() { return &ucharType; }
    Type* GetFloatType() { return &floatType; }
    Type* GetDoubleType() { return &doubleType; }
    PtrType* GetVoidPtrType() { return &voidPtrType; }
    Type* GetPtrType(Type* baseType);
    Type* GetClassType(const std::vector<Type*>& memberTypes);
private:
    std::vector<std::unique_ptr<Type>> types;
    std::unordered_set<FunctionType*, FunctionTypeHash, FunctionTypeEqual> functionTypeSet;
    std::unordered_set<PtrType*, PtrTypeHash, PtrTypeEqual> ptrTypeSet;
    std::unordered_set<ClassType*, ClassTypeHash, ClassTypeEqual> classTypeSet;
    std::unordered_map<int, Type*> typeMap;
    VoidType voidType;
    SByteType sbyteType;
    ByteType byteType;
    ShortType shortType;
    UShortType ushortType;
    IntType intType;
    UIntType uintType;
    LongType longType;
    ULongType ulongType;
    BoolType boolType;
    CharType charType;
    WCharType wcharType;
    UCharType ucharType;
    FloatType floatType;
    DoubleType doubleType;
    PtrType voidPtrType;
    int nextTypeId;
};

} // namespace cmsystemx

#endif // CMAJOR_CMSX_TYPE_INCLUDED
