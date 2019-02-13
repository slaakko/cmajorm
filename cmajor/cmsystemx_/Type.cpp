// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/Type.hpp>
#include <cmajor/cmsystemx/Context.hpp>
#include <cmajor/cmsystemx/Function.hpp>
#include <cmajor/cmsystemx/OpCode.hpp>

namespace cmsystemx {

Type::Type(TypeKind kind_) : kind(kind_), id(-1)
{
}

Type::Type(TypeKind kind_, int id_) : kind(kind_), id(id_)
{
}

Type::~Type()
{
}

Value* Type::CreateReturnValue(Context& context)
{
    Instruction* setInst = context.CreateInstruction(SET);
    Value* reg = context.GetCurrentBasicBlock()->GetFunction()->GetReg(this);
    setInst->AddOperand(reg);
    setInst->AddOperand(context.Ax());
    return reg;
}

VoidType::VoidType() : Type(TypeKind::voidType, voidTypeId)
{
}

Value* VoidType::DefaultValue()
{
    throw std::runtime_error("tried to create default value for void type");
}

Value* VoidType::MinusOne()
{
    throw std::runtime_error("tried to create minus one value for void type");
}

Instruction* VoidType::CreateLoad(Context& context)
{
    throw std::runtime_error("tried to create load from void type");
}

Instruction* VoidType::CreateStore(Context& context)
{
    throw std::runtime_error("tried to create store to void type");
}

Value* VoidType::CreateReturnValue(Context& context)
{
    return nullptr;
}

Value* VoidType::GetValue(int64_t value, Context& context)
{
    throw std::runtime_error("tried to get value from void type");
}

SByteType::SByteType() : Type(TypeKind::sbyteType, sbyteTypeId), minusOne(-1)
{
}

Value* SByteType::GetValue(int64_t value, Context& context)
{
    return context.GetSByteValue(static_cast<int8_t>(value));
}

Instruction* SByteType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDB);
}

Instruction* SByteType::CreateStore(Context& context)
{
    return context.CreateInstruction(STB);
}

ByteType::ByteType() : Type(TypeKind::byteType, byteTypeId), minusOne(static_cast<uint8_t>(-1))
{
}

Value* ByteType::GetValue(int64_t value, Context& context)
{
    return context.GetByteValue(static_cast<uint8_t>(value));
}

Instruction* ByteType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDBU);
}

Instruction* ByteType::CreateStore(Context& context)
{
    return context.CreateInstruction(STBU);
}

ShortType::ShortType() : Type(TypeKind::shortType, shortTypeId), minusOne(-1)
{
}

Value* ShortType::GetValue(int64_t value, Context& context)
{
    return context.GetShortValue(static_cast<int16_t>(value));
}

Instruction* ShortType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDW);
}

Instruction* ShortType::CreateStore(Context& context)
{
    return context.CreateInstruction(STW);
}

UShortType::UShortType() : Type(TypeKind::ushortType, ushortTypeId), minusOne(static_cast<uint16_t>(-1))
{
}

Value* UShortType::GetValue(int64_t value, Context& context)
{
    return context.GetUShortValue(static_cast<uint16_t>(value));
}

Instruction* UShortType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDWU);
}

Instruction* UShortType::CreateStore(Context& context)
{
    return context.CreateInstruction(STWU);
}

IntType::IntType() : Type(TypeKind::intType, intTypeId), minusOne(-1)
{
}

Value* IntType::GetValue(int64_t value, Context& context)
{
    return context.GetIntValue(static_cast<int32_t>(value));
}

Instruction* IntType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDT);
}

Instruction* IntType::CreateStore(Context& context)
{
    return context.CreateInstruction(STT);
}

UIntType::UIntType() : Type(TypeKind::uintType, uintTypeId), minusOne(static_cast<uint32_t>(-1))
{
}

Value* UIntType::GetValue(int64_t value, Context& context)
{
    return context.GetUIntValue(static_cast<uint32_t>(value));
}

Instruction* UIntType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDTU);
}

Instruction* UIntType::CreateStore(Context& context)
{
    return context.CreateInstruction(STTU);
}

LongType::LongType() : Type(TypeKind::longType, longTypeId), minusOne(-1)
{
}

Value* LongType::GetValue(int64_t value, Context& context)
{
    return context.GetLongValue(static_cast<int64_t>(value));
}

Instruction* LongType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDO);
}

Instruction* LongType::CreateStore(Context& context)
{
    return context.CreateInstruction(STO);
}

ULongType::ULongType() : Type(TypeKind::ulongType, ulongTypeId), minusOne(static_cast<uint64_t>(-1))
{
}

Value* ULongType::GetValue(int64_t value, Context& context)
{
    return context.GetULongValue(static_cast<uint64_t>(value));
}

Instruction* ULongType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDOU);
}

Instruction* ULongType::CreateStore(Context& context)
{
    return context.CreateInstruction(STOU);
}

BoolType::BoolType() : Type(TypeKind::boolType, boolTypeId), minusOne(true)
{
}

Value* BoolType::GetValue(int64_t value, Context& context)
{
    return context.GetBoolValue(static_cast<bool>(value));
}

Instruction* BoolType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDBU);
}

Instruction* BoolType::CreateStore(Context& context)
{
    return context.CreateInstruction(STBU);
}

CharType::CharType() : Type(TypeKind::charType, charTypeId), minusOne(static_cast<unsigned char>(-1))
{
}

Value* CharType::GetValue(int64_t value, Context& context)
{
    return context.GetCharValue(static_cast<unsigned char>(value));
}

Instruction* CharType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDBU);
}

Instruction* CharType::CreateStore(Context& context)
{
    return context.CreateInstruction(STBU);
}

WCharType::WCharType() : Type(TypeKind::wcharType, wcharTypeId), minusOne(static_cast<char16_t>(-1))
{
}

Value* WCharType::GetValue(int64_t value, Context& context)
{
    return context.GetWCharValue(static_cast<char16_t>(value));
}

Instruction* WCharType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDWU);
}

Instruction* WCharType::CreateStore(Context& context)
{
    return context.CreateInstruction(STWU);
}

UCharType::UCharType() : Type(TypeKind::ucharType, ucharTypeId), minusOne(static_cast<char32_t>(-1))
{
}

Value* UCharType::GetValue(int64_t value, Context& context)
{
    return context.GetUCharValue(static_cast<char32_t>(value));
}

Instruction* UCharType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDTU);
}

Instruction* UCharType::CreateStore(Context& context)
{
    return context.CreateInstruction(STTU);
}

FloatType::FloatType() : Type(TypeKind::floatType, floatTypeId)
{
}

Value* FloatType::MinusOne()
{
    throw std::runtime_error("tried to create minus one value for float type");
}

Value* FloatType::GetValue(int64_t value, Context& context)
{
    return context.GetFloatValue(static_cast<float>(value));
}

Instruction* FloatType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDTU);
}

Instruction* FloatType::CreateStore(Context& context)
{
    return context.CreateInstruction(STTU);
}

DoubleType::DoubleType() : Type(TypeKind::doubleType, doubleTypeId)
{
}

Value* DoubleType::MinusOne()
{
    throw std::runtime_error("tried to create minus one value for double type");
}

Value* DoubleType::GetValue(int64_t value, Context& context)
{
    return context.GetDoubleValue(static_cast<double>(value));
}

Instruction* DoubleType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDOU);
}

Instruction* DoubleType::CreateStore(Context& context)
{
    return context.CreateInstruction(STOU);
}

PtrType::PtrType(int id_, Type* baseType_) : Type(TypeKind::ptrType, id_), baseType(baseType_), defaultValue(this)
{
}

Value* PtrType::MinusOne()
{
    throw std::runtime_error("tried to create minus one value for pointer type");
}

Value* PtrType::GetValue(int64_t value, Context& context)
{
    throw std::runtime_error("tried to get value for a pointer type");
}

Instruction* PtrType::CreateLoad(Context& context)
{
    return context.CreateInstruction(LDOU);
}

Instruction* PtrType::CreateStore(Context& context)
{
    return context.CreateInstruction(STOU);
}

bool PtrTypeEqual::operator()(PtrType* left, PtrType* right) const
{
    if (left->BaseType() != right->BaseType()) return false;
    return true;
}

size_t PtrTypeHash::operator()(PtrType* x) const
{
    return std::hash<Type*>()(x->BaseType());
}

FunctionType::FunctionType() : Type(TypeKind::functionType), returnType(nullptr), paramTypes()
{
}

FunctionType::FunctionType(Type* returnType_, const std::vector<Type*>& paramTypes_) : Type(TypeKind::functionType), returnType(returnType_), paramTypes(paramTypes_)
{
}

Value* FunctionType::DefaultValue()
{
    throw std::runtime_error("tried to create default value for a function type");
}

Value* FunctionType::MinusOne()
{
    throw std::runtime_error("tried to create minus one value for a function type");
}

Value* FunctionType::GetValue(int64_t value, Context& context)
{
    throw std::runtime_error("tried to get value for a function type");
}

Instruction* FunctionType::CreateLoad(Context& context)
{
    throw std::runtime_error("tried to create load from function type");
}

Instruction* FunctionType::CreateStore(Context& context)
{
    throw std::runtime_error("tried to create store to function type");
}

bool FunctionTypeEqual::operator()(FunctionType* left, FunctionType* right) const
{
    if (left->ReturnType() != right->ReturnType()) return false;
    if (left->ParamTypes().size() != right->ParamTypes().size()) return false;
    int n = left->ParamTypes().size();
    for (int i = 0; i < n; ++i)
    {
        if (left->ParamTypes()[i] != right->ParamTypes()[i]) return false;
    }
    return true;
}

size_t FunctionTypeHash::operator()(FunctionType* x) const
{
    size_t v = std::hash<Type*>()(x->ReturnType());
    for (Type* paramType : x->ParamTypes())
    {
        v = v ^ std::hash<Type*>()(paramType);
    }
    return v;
}

bool operator==(const MemberType& left, const MemberType& right)
{
    return left.type == right.type && left.offset == right.offset;
}

size_t MemberTypeHash::operator()(const MemberType& x) const
{
    return std::hash<Type*>()(x.type) ^ std::hash<uint64_t>()(x.offset);
}

ClassType::ClassType(int id_, const std::vector<MemberType>& memberTypes_, uint64_t sizeInBytes_) : Type(TypeKind::classType, id_), memberTypes(memberTypes_), sizeInBytes(sizeInBytes_)
{
}

Value* ClassType::DefaultValue()
{
    // todo
    return nullptr;
}

Value* ClassType::MinusOne()
{
    // todo
    return nullptr;
}

Value* ClassType::GetValue(int64_t value, Context& context)
{
    // todo
    return nullptr;
}

Instruction* ClassType::CreateLoad(Context& context)
{
    // todo
    return nullptr;
}

Instruction* ClassType::CreateStore(Context& context)
{
    // todo
    return nullptr;
}

size_t ClassTypeHash::operator()(ClassType* x) const
{
    size_t h = std::hash<uint64_t>()(0);
    int n = x->MemberTypes().size();
    for (int i = 0; i < n; ++i)
    {
        h = h ^ MemberTypeHash()(x->MemberTypes()[i]);
    }
    return h;
}

bool ClassTypeEqual::operator()(ClassType* left, ClassType* right) const
{
    if (left->MemberTypes().size() != right->MemberTypes().size()) return false;
    int n = left->MemberTypes().size();
    for (int i = 0; i < n; ++i)
    {
        const MemberType& leftMemberType = left->MemberTypes()[i];
        const MemberType& rightMemberType = right->MemberTypes()[i];
        if (leftMemberType != rightMemberType) return false;
    }
    return true;
}

TypeMap::TypeMap() : voidType(), sbyteType(), byteType(), shortType(), ushortType(), intType(), uintType(),
    longType(), ulongType(), boolType(), charType(), wcharType(), ucharType(), floatType(), doubleType(), voidPtrType(firstUserTypeId, &voidType), nextTypeId(firstUserTypeId + 1)
{
    typeMap[voidType.Id()] = &voidType;
    typeMap[sbyteType.Id()] = &sbyteType;
    typeMap[byteType.Id()] = &byteType;
    typeMap[shortType.Id()] = &shortType;
    typeMap[ushortType.Id()] = &ushortType;
    typeMap[intType.Id()] = &intType;
    typeMap[uintType.Id()] = &uintType;
    typeMap[longType.Id()] = &longType;
    typeMap[ulongType.Id()] = &ulongType;
    typeMap[boolType.Id()] = &boolType;
    typeMap[charType.Id()] = &charType;
    typeMap[wcharType.Id()] = &wcharType;
    typeMap[ucharType.Id()] = &ucharType;
    typeMap[floatType.Id()] = &floatType;
    typeMap[doubleType.Id()] = &doubleType;
    ptrTypeSet.insert(&voidPtrType);
    typeMap[voidPtrType.Id()] = &voidPtrType;
}

Type* TypeMap::GetPtrType(Type* baseType)
{
    if (baseType->Id() == voidTypeId) return &voidPtrType;
    std::unique_ptr<PtrType> ptrType(new PtrType(nextTypeId++, baseType));
    auto it = ptrTypeSet.find(ptrType.get());
    if (it != ptrTypeSet.cend())
    {
        return *it;
    }
    else
    {
        PtrType* pt = ptrType.get();
        ptrTypeSet.insert(pt);
        types.push_back(std::move(ptrType));
        typeMap[pt->Id()] = pt;
        return pt;
    }
}

FunctionType* TypeMap::GetFunctionType(cmsystemx::Type* returnType, const std::vector<cmsystemx::Type*>& parameterTypes)
{
    std::unique_ptr<FunctionType> ft(new FunctionType(returnType, parameterTypes));
    auto it = functionTypeSet.find(ft.get());
    if (it != functionTypeSet.cend())
    {
        return *it;
    }
    else
    {
        FunctionType* funT = ft.get();
        functionTypeSet.insert(funT);
        types.push_back(std::move(ft));
        typeMap[funT->Id()] = funT;
        return funT;
    }
}

Type* TypeMap::GetClassType(const std::vector<Type*>& memberTypes)
{
    std::vector<MemberType> memTypes;
    uint64_t offset = 0;
    bool first = true;
    for (Type* type : memberTypes)
    {
        uint64_t memberOffset = offset;
        if (first)
        {
            first = false;
        }
        else
        {
            uint64_t alignment = type->AlignmentInBits() / 8;
            memberOffset = alignment * ((offset - 1) / alignment + 1);
        }
        MemberType memberType(type, memberOffset);
        memTypes.push_back(memberType);
        offset = memberOffset + type->SizeInBits() / 8;
    }
    uint64_t sizeInBytes = 8 * ((offset - 1) / 8 + 1);
    std::unique_ptr<ClassType> classType(new ClassType(nextTypeId++, memTypes, sizeInBytes));
    auto it = classTypeSet.find(classType.get());
    if (it != classTypeSet.cend())
    {
        return *it;
    }
    else
    {
        ClassType* ct = classType.get();
        classTypeSet.insert(ct);
        types.push_back(std::move(classType));
        typeMap[ct->Id()] = ct;
        return ct;
    }
}

} // namespace cmsystemx
