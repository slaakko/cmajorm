// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxbe/Emitter.hpp>
#include <cmajor/cmsxi/Context.hpp>
#include <cmajor/cmsxi/Type.hpp>

namespace cmsxbe {

Emitter::Emitter(cmsxbe::EmittingContext* emittingContext_) :
    cmajor::ir::Emitter(&stack), emittingContext(emittingContext_), emittingDelegate(nullptr), context(nullptr), compileUnit(nullptr), currentFunction(nullptr),
    objectPointer(nullptr)
{
}

void Emitter::SetEmittingDelegate(cmajor::ir::EmittingDelegate* emittingDelegate_)
{
    emittingDelegate = emittingDelegate_;
}

void* Emitter::GetIrTypeForBool()
{
    return context->GetBoolType();
}

void* Emitter::GetIrTypeForSByte()
{
    return context->GetSByteType();
}

void* Emitter::GetIrTypeForByte()
{
    return context->GetByteType();
}

void* Emitter::GetIrTypeForShort()
{
    return context->GetShortType();
}

void* Emitter::GetIrTypeForUShort()
{
    return context->GetUShortType();
}

void* Emitter::GetIrTypeForInt()
{
    return context->GetIntType();
}

void* Emitter::GetIrTypeForUInt()
{
    return context->GetUIntType();
}

void* Emitter::GetIrTypeForLong()
{
    return context->GetLongType();
}

void* Emitter::GetIrTypeForULong()
{
    return context->GetULongType();
}

void* Emitter::GetIrTypeForFloat()
{
    return context->GetFloatType();
}

void* Emitter::GetIrTypeForDouble()
{
    return context->GetDoubleType();
}

void* Emitter::GetIrTypeForChar()
{
    return context->GetByteType();
}

void* Emitter::GetIrTypeForWChar()
{
    return context->GetUShortType();
}

void* Emitter::GetIrTypeForUChar()
{
    return context->GetUIntType();
}

void* Emitter::GetIrTypeForVoid()
{
    return context->GetVoidType();
}

void* Emitter::GetIrTypeForFunction(void* retType, const std::vector<void*>& paramTypes)
{
    std::vector<cmsxi::Type*> parameterTypes;
    for (void* paramType : paramTypes)
    {
        parameterTypes.push_back(static_cast<cmsxi::Type*>(paramType));
    }
    return context->GetFunctionType(static_cast<cmsxi::Type*>(retType), parameterTypes);
}

void* Emitter::GetIrTypeForVariableParamFunction(void* retType)
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeByTypeId(const boost::uuids::uuid& typeId)
{
    auto it = irTypeTypeIdMap.find(typeId);
    if (it != irTypeTypeIdMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetIrTypeByTypeId(const boost::uuids::uuid& typeId, void* irType)
{
    irTypeTypeIdMap[typeId] = static_cast<cmsxi::Type*>(irType);
}

void* Emitter::GetIrTypeForArrayType(void* elementType, int64_t size)
{
    return context->GetArrayType(static_cast<cmsxi::Type*>(elementType), size);
}

void* Emitter::GetIrTypeForClassType(const std::vector<void*>& elementTypes)
{
    std::vector<cmsxi::Type*> memberTypes;
    for (void* elementType : elementTypes)
    {
        memberTypes.push_back(static_cast<cmsxi::Type*>(elementType));
    }
    return context->GetStructureType(memberTypes);
}

void* Emitter::CreateFwdIrTypeForClassType()
{
    return context->CreateStructureType();
}

void Emitter::SetFwdIrTypeBody(void* forwardDeclaredType, const std::vector<void*>& elementTypes)
{
    std::vector<cmsxi::Type*> memberTypes;
    for (void* elementType : elementTypes)
    {
        memberTypes.push_back(static_cast<cmsxi::Type*>(elementType));
    }
    cmsxi::StructureType* structureType = static_cast<cmsxi::StructureType*>(forwardDeclaredType);
    structureType->SetMemberTypes(memberTypes);
}

void* Emitter::GetIrTypeForDelegateType(void* retType, const std::vector<void*>& paramTypes)
{
    std::vector<cmsxi::Type*> parameterTypes;
    for (void* paramType : paramTypes)
    {
        parameterTypes.push_back(static_cast<cmsxi::Type*>(paramType));
    }
    return context->GetPtrType(context->GetFunctionType(static_cast<cmsxi::Type*>(retType), parameterTypes));
}

void* Emitter::GetIrTypeForVoidPtrType()
{
    return context->GetPtrType(context->GetVoidType());
}

void* Emitter::GetIrTypeForStructType(const std::vector<void*>& elementTypes)
{
    std::vector<cmsxi::Type*> memberTypes;
    for (void* elementType : elementTypes)
    {
        memberTypes.push_back(static_cast<cmsxi::Type*>(elementType));
    }
    return context->GetStructureType(memberTypes);
}

void* Emitter::GetIrTypeForPtrType(void* baseIrType)
{
    return context->GetPtrType(static_cast<cmsxi::Type*>(baseIrType));
}

void* Emitter::CreateDefaultIrValueForArrayType(void* arrayIrType, const std::vector<void*>& arrayOfDefaults)
{
    std::vector<cmsxi::ConstantValue*> arrayOfConstants;
    for (void* constant : arrayOfDefaults)
    {
        arrayOfConstants.push_back(static_cast<cmsxi::ConstantValue*>(constant));
    }
    cmsxi::Type* arrayType = static_cast<cmsxi::Type*>(arrayIrType);
    return context->GetArrayValue(arrayType, arrayOfConstants, std::string());
}

void* Emitter::CreateDefaultIrValueForBool()
{
    return context->GetDefaultBoolValue();
}

void* Emitter::CreateDefaultIrValueForSByte()
{
    return context->GetDefaultSByteValue();
}

void* Emitter::CreateDefaultIrValueForByte()
{
    return context->GetDefaultByteValue();
}

void* Emitter::CreateDefaultIrValueForShort()
{
    return context->GetDefaultShortValue();
}

void* Emitter::CreateDefaultIrValueForUShort()
{
    return context->GetDefaultUShortValue();
}

void* Emitter::CreateDefaultIrValueForInt()
{
    return context->GetDefaultIntValue();
}

void* Emitter::CreateDefaultIrValueForUInt()
{
    return context->GetDefaultUIntValue();
}

void* Emitter::CreateDefaultIrValueForLong()
{
    return context->GetDefaultLongValue();
}

void* Emitter::CreateDefaultIrValueForULong()
{
    return context->GetDefaultULongValue();
}

void* Emitter::CreateDefaultIrValueForFloat()
{
    return context->GetDefaultFloatValue();
}

void* Emitter::CreateDefaultIrValueForDouble()
{
    return context->GetDefaultDoubleValue();
}

void* Emitter::CreateDefaultIrValueForChar()
{
    return context->GetDefaultByteValue();
}

void* Emitter::CreateDefaultIrValueForWChar()
{
    return context->GetDefaultUShortValue();
}

void* Emitter::CreateDefaultIrValueForUChar()
{
    return context->GetDefaultUIntValue();
}

void* Emitter::CreateDefaultIrValueForStruct(void* irType, const std::vector<void*>& defaultMembers)
{
    std::vector<cmsxi::ConstantValue*> arrayOfDefaults;
    for (void* constant : defaultMembers)
    {
        arrayOfDefaults.push_back(static_cast<cmsxi::ConstantValue*>(constant));
    }
    return context->GetStructureValue(static_cast<cmsxi::StructureType*>(irType), arrayOfDefaults);
}

void* Emitter::CreateDefaultIrValueForDelegateType(void* irType)
{
    return context->GetNullValue(static_cast<cmsxi::PtrType*>(irType));
}

void* Emitter::CreateDefaultIrValueForVoidPtrType()
{
    return context->GetNullValue(static_cast<cmsxi::PtrType*>(context->GetPtrType(context->GetVoidType())));
}

void* Emitter::CreateDefaultIrValueForDerivedType(void* irType)
{
    return static_cast<cmsxi::Type*>(irType)->DefaultValue();
}

void* Emitter::CreateDefaultIrValueForPtrType(void* irType)
{
    return context->GetNullValue(static_cast<cmsxi::PtrType*>(irType));
}

void* Emitter::CreateIrValueForBool(bool value)
{
    return context->GetBoolValue(value);
}

void* Emitter::CreateIrValueForSByte(int8_t value)
{
    return context->GetSByteValue(value);
}

void* Emitter::CreateIrValueForByte(uint8_t value)
{
    return context->GetByteValue(value);
}

void* Emitter::CreateIrValueForShort(int16_t value)
{
    return context->GetShortValue(value);
}

void* Emitter::CreateIrValueForUShort(uint16_t value)
{
    return context->GetUShortValue(value);
}

void* Emitter::CreateIrValueForInt(int32_t value)
{
    return context->GetIntValue(value);
}

void* Emitter::CreateIrValueForUInt(uint32_t value)
{
    return context->GetUIntValue(value);
}

void* Emitter::CreateIrValueForLong(int64_t value)
{
    return context->GetLongValue(value);
}

void* Emitter::CreateIrValueForULong(uint64_t value)
{
    return context->GetULongValue(value);
}

void* Emitter::CreateIrValueForFloat(float value)
{
    return context->GetFloatValue(value);
}

void* Emitter::CreateIrValueForDouble(double value)
{
    return context->GetDoubleValue(value);
}

void* Emitter::CreateIrValueForChar(uint8_t value)
{
    return context->GetByteValue(value);
}

void* Emitter::CreateIrValueForWChar(uint16_t value)
{
    return context->GetUShortValue(value);
}

void* Emitter::CreateIrValueForUChar(uint32_t value)
{
    return context->GetUIntValue(value);
}

void* Emitter::CreateIrValueForWString(void* wstringConstant)
{
    return static_cast<cmsxi::Value*>(wstringConstant);
}

void* Emitter::CreateIrValueForUString(void* ustringConstant)
{
    return static_cast<cmsxi::Value*>(ustringConstant);
}

void* Emitter::CreateIrValueForConstantArray(void* arrayIrType, const std::vector<void*>& elementConstants, const std::string& prefix)
{
    std::vector<cmsxi::ConstantValue*> elements;
    for (void* elementConstant : elementConstants)
    {
        elements.push_back(static_cast<cmsxi::ConstantValue*>(elementConstant));
    }
    return context->GetArrayValue(static_cast<cmsxi::ArrayType*>(arrayIrType), elements, prefix);
}

void* Emitter::CreateIrValueForConstantStruct(void* structIrType, const std::vector<void*>& elementConstants)
{
    std::vector<cmsxi::ConstantValue*> memberConstants;
    for (void* elementConstant : elementConstants)
    {
        memberConstants.push_back(static_cast<cmsxi::ConstantValue*>(elementConstant));
    }
    return context->GetStructureValue(static_cast<cmsxi::StructureType*>(structIrType), memberConstants);
}

void* Emitter::CreateIrValueForUuid(void* uuidConstant)
{
    cmsxi::Value* arg = context->CreatePtrOffset(static_cast<cmsxi::Value*>(uuidConstant), context->GetLongValue(0));
    return context->CreateBitCast(arg, context->GetPtrType(context->GetVoidType()));
}

void* Emitter::GetConversionValue(void* type, void* from)
{
    return context->GetConversionValue(static_cast<cmsxi::Type*>(type), static_cast<cmsxi::ConstantValue*>(from));
}

void* Emitter::CreateGlobalStringPtr(const std::string& stringValue)
{
    return context->CreateGlobalStringPtr(stringValue);
}

void* Emitter::GetGlobalStringPtr(int stringId)
{
    return emittingDelegate->GetGlobalStringPtr(stringId);
}

void* Emitter::GetGlobalWStringConstant(int stringId)
{
    return emittingDelegate->GetGlobalWStringConstant(stringId);
}

void* Emitter::GetGlobalUStringConstant(int stringId)
{
    return emittingDelegate->GetGlobalUStringConstant(stringId);
}

void* Emitter::GetGlobalUuidConstant(int uuidId)
{
    return emittingDelegate->GetGlobalUuidConstant(uuidId);
}

void* Emitter::CreateDITypeForBool()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForSByte()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForByte()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForShort()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForUShort()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForInt()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForUInt()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForLong()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForULong()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForFloat()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForDouble()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForChar()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForWChar()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForUChar()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForVoid()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForArray(void* elementDIType, const std::vector<void*>& elements)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForEnumConstant(const std::string& name, int64_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDITypeForEnumType(const std::string& name, const std::string& mangledName, const Span& span, const std::vector<void*>& enumConstantElements,
    uint64_t sizeInBits, uint32_t alignInBits, void* underlyingDIType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrDIForwardDeclaration(void* irType, const std::string& name, const std::string& mangledName, const Span& span)
{
    // todo
    return nullptr;
}

uint64_t Emitter::GetOffsetInBits(void* classIrType, int layoutIndex)
{
    // todo
    return uint64_t();
}

void* Emitter::CreateDITypeForClassType(void* irType, const std::vector<void*>& memberVariableElements, const Span& classSpan, const std::string& name, void* vtableHolderClass,
    const std::string& mangledName, void* baseClassDIType)
{
    // todo
    return nullptr;
}

void Emitter::MapFwdDeclaration(void* fwdDeclaration, const boost::uuids::uuid& typeId)
{
    // todo
}

void* Emitter::GetDITypeByTypeId(const boost::uuids::uuid& typeId) const
{
    // todo
    return nullptr;
}

void Emitter::SetDITypeByTypeId(const boost::uuids::uuid& typeId, void* diType)
{
    // todo
}

void* Emitter::GetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId)
{
    // todo
    return nullptr;
}

void Emitter::SetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId, void* diType)
{
    // todo
}

void* Emitter::CreateDIMemberType(void* scope, const std::string& name, const Span& span, uint64_t sizeInBits, uint64_t alignInBits, uint64_t offsetInBits, void* diType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateConstDIType(void* diType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateLValueRefDIType(void* diType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateRValueRefDIType(void* diType)
{
    // todo
    return nullptr;
}

void* Emitter::CreatePointerDIType(void* diType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateUnspecifiedDIType(const std::string& name)
{
    // todo
    return nullptr;
}

void Emitter::MapClassPtr(const boost::uuids::uuid& typeId, void* classPtr)
{
    // todo
}

uint64_t Emitter::GetSizeInBits(void* irType)
{
    // todo
    return 0;
    //return static_cast<cmsxi::Type*>(irType)->SizeInBits();
}

uint64_t Emitter::GetAlignmentInBits(void* irType)
{
    // todo
    return 0;
    //return static_cast<cmsxi::Type*>(irType)->AlignmentInBits();
}

void Emitter::SetCurrentDebugLocation(const Span& span)
{
    // todo
}

void* Emitter::GetArrayBeginAddress(void* arrayPtr)
{
    return context->CreateElemAddr(static_cast<cmsxi::Value*>(arrayPtr), context->GetLongValue(0));
}

void* Emitter::GetArrayEndAddress(void* arrayPtr, uint64_t size)
{
    return context->CreateElemAddr(static_cast<cmsxi::Value*>(arrayPtr), context->GetLongValue(size));
}

void* Emitter::CreateBasicBlock(const std::string& name)
{
    return currentFunction->CreateBasicBlock();
}

void Emitter::CreateBr(void* targetBasicBlock)
{
    context->CreateJump(static_cast<cmsxi::BasicBlock*>(targetBasicBlock));
}

void* Emitter::CurrentBasicBlock() const
{
    return context->GetCurrentBasicBlock();
}

void Emitter::SetCurrentBasicBlock(void* basicBlock)
{
    context->SetCurrentBasicBlock(static_cast<cmsxi::BasicBlock*>(basicBlock));
}

void Emitter::CreateCondBr(void* cond, void* trueBasicBlock, void* falseBasicBlock)
{
    context->CreateBranch(static_cast<cmsxi::Value*>(cond), static_cast<cmsxi::BasicBlock*>(trueBasicBlock), static_cast<cmsxi::BasicBlock*>(falseBasicBlock));
}

void* Emitter::CreateArrayIndexAddress(void* arrayPtr, void* index)
{
    return context->CreateElemAddr(static_cast<cmsxi::Value*>(arrayPtr), static_cast<cmsxi::Value*>(index));
}

void Emitter::CreateStore(void* value, void* ptr)
{
    context->CreateStore(static_cast<cmsxi::Value*>(value), static_cast<cmsxi::Value*>(ptr));
}

void* Emitter::CreateLoad(void* ptr)
{
    return context->CreateLoad(static_cast<cmsxi::Value*>(ptr));
}

void* Emitter::CreateAdd(void* left, void* right)
{
    return context->CreateAdd(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateFAdd(void* left, void* right)
{
    return context->CreateAdd(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateSub(void* left, void* right)
{
    return context->CreateSub(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateFSub(void* left, void* right)
{
    return context->CreateSub(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateMul(void* left, void* right)
{
    return context->CreateMul(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateFMul(void* left, void* right)
{
    return context->CreateMul(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateUDiv(void* left, void* right)
{
    return context->CreateDiv(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateSDiv(void* left, void* right)
{
    return context->CreateDiv(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateFDiv(void* left, void* right)
{
    return context->CreateDiv(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateURem(void* left, void* right)
{
    return context->CreateMod(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateSRem(void* left, void* right)
{
    return context->CreateMod(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateAnd(void* left, void* right)
{
    return context->CreateAnd(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateOr(void* left, void* right)
{
    return context->CreateOr(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateXor(void* left, void* right)
{
    return context->CreateXor(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateShl(void* left, void* right)
{
    return context->CreateShl(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateAShr(void* left, void* right)
{
    return context->CreateShr(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateLShr(void* left, void* right)
{
    return context->CreateShr(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateICmpEQ(void* left, void* right)
{
    return context->CreateEqual(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateFCmpOEQ(void* left, void* right)
{
    return context->CreateEqual(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateICmpULT(void* left, void* right)
{
    return context->CreateLess(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateICmpSLT(void* left, void* right)
{
    return context->CreateLess(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateFCmpOLT(void* left, void* right)
{
    return context->CreateLess(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

void* Emitter::CreateSExt(void* operand, void* destinationType)
{
    return context->CreateSignExtend(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateZExt(void* operand, void* destinationType)
{
    return context->CreateZeroExtend(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateFPExt(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateTrunc(void* operand, void* destinationType)
{
    return context->CreateTruncate(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateFPTrunc(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateBitCast(void* operand, void* destinationType)
{
    return context->CreateBitCast(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateUIToFP(void* operand, void* destinationType)
{
    return context->CreateIntToFloat(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateSIToFP(void* operand, void* destinationType)
{
    return context->CreateIntToFloat(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateFPToUI(void* operand, void* destinationType)
{
    return context->CreateFloatToInt(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateFPToSI(void* operand, void* destinationType)
{
    return context->CreateFloatToInt(static_cast<cmsxi::Value*>(operand), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateIntToPtr(void* intValue, void* destinationType)
{
    return context->CreateIntToPtr(static_cast<cmsxi::Value*>(intValue), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreatePtrToInt(void* ptrValue, void* destinationType)
{
    return context->CreatePtrToInt(static_cast<cmsxi::Value*>(ptrValue), static_cast<cmsxi::Type*>(destinationType));
}

void* Emitter::CreateNot(void* value)
{
    return context->CreateNot(static_cast<cmsxi::Value*>(value));
}

void* Emitter::CreateNeg(void* value)
{
    return context->CreateNeg(static_cast<cmsxi::Value*>(value));
}

void* Emitter::CreateFNeg(void* value)
{
    return context->CreateNeg(static_cast<cmsxi::Value*>(value));
}

std::string Emitter::GetVmtObjectName(void* symbol) const
{
    // todo
    return std::string();
}

void Emitter::SetVmtObjectName(void* symbol, const std::string& vmtObjectName)
{
    // todo
}

std::string Emitter::GetImtArrayObjectName(void* symbol) const
{
    // todo
    return std::string();
}

void Emitter::SetImtArrayObjectName(void* symbol, const std::string& imtArrayObjectName)
{
    // todo
}

void* Emitter::GetVmtObjectType(void* symbol) const
{
    // todo
    return nullptr;
}

void Emitter::SetVmtObjectType(void* symbol, void* vmtObjectType)
{
    // todo
}

void* Emitter::GetStaticObjectType(void* symbol) const
{
    auto it = staticTypeMap.find(symbol);
    if (it != staticTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetStaticObjectType(void* symbol, void* type)
{
    staticTypeMap[symbol] = static_cast<cmsxi::StructureType*>(type);
}

std::string Emitter::GetStaticObjectName(void* symbol) const
{
    auto it = staticObjectNameMap.find(symbol);
    if (it != staticObjectNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Emitter::SetStaticObjectName(void* symbol, const std::string& staticObjectName)
{
    staticObjectNameMap[symbol] = staticObjectName;
}

void* Emitter::GetOrInsertGlobal(const std::string& name, void* type)
{
    return context->GetOrInsertGlobal(name, static_cast<cmsxi::Type*>(type));
}

void* Emitter::GetOrInsertAnyComdat(const std::string& name, void* global)
{
    static_cast<cmsxi::GlobalVariable*>(global)->SetLinkOnce();
    return nullptr;
}

void* Emitter::GetOrInsertAnyFunctionComdat(const std::string& name, void* function)
{
    static_cast<cmsxi::Function*>(function)->SetLinkOnce();
    return nullptr;
}

void* Emitter::GetOrInsertFunction(const std::string& name, void* type)
{
    return compileUnit->GetOrInsertFunction(name, static_cast<cmsxi::FunctionType*>(type));
}

void Emitter::SetInitializer(void* global, void* initializer)
{
    cmsxi::GlobalVariable* globalVar = static_cast<cmsxi::GlobalVariable*>(global);
    globalVar->SetInitializer(static_cast<cmsxi::ConstantValue*>(initializer));
}

void Emitter::SetPrivateLinkage(void* global)
{
    // todo
}

bool Emitter::IsVmtObjectCreated(void* symbol) const
{
    return vmtObjectCreatedSet.find(symbol) != vmtObjectCreatedSet.cend();
}

void Emitter::SetVmtObjectCreated(void* symbol)
{
    vmtObjectCreatedSet.insert(symbol);
}

bool Emitter::IsStaticObjectCreated(void* symbol) const
{
    return staticObjectCreatedSet.find(symbol) != staticObjectCreatedSet.cend();
}

void Emitter::SetStaticObjectCreated(void* symbol)
{
    staticObjectCreatedSet.insert(symbol);
}

void* Emitter::HandlerBlock()
{
    // todo
    return nullptr;
}

void* Emitter::CleanupBlock()
{
    // todo
    return nullptr;
}

bool Emitter::NewCleanupNeeded()
{
    // todo
    return false;
}

void Emitter::CreateCleanup()
{
    // todo
}

cmajor::ir::Pad* Emitter::CurrentPad()
{
    // todo
    return nullptr;
}

void* Emitter::CreateCleanupPadWithParent(void* parentPad, const std::vector<void*>& args)
{
    // todo
    return nullptr;
}

void* Emitter::CreateCleanupPad(const std::vector<void*>& args)
{
    // todo
    return nullptr;
}

void* Emitter::CreateCleanupRet(void* cleanupPad, void* unwindTarget)
{
    // todo
    return nullptr;
}

void* Emitter::CreateCatchRet(void* catchPad, void* returnTarget)
{
    // todo
    return nullptr;
}

void* Emitter::CreateCatchSwitch(void* unwindBlock)
{
    // todo
    return nullptr;
}

void* Emitter::CreateCatchSwitchWithParent(void* parentPad, void* unwindBlock)
{
    // todo
    return nullptr;
}

void Emitter::AddHandlerToCatchSwitch(void* catchSwitch, void* target)
{
    // todo
}

void* Emitter::CreateCatchPad(void* parentPad, const std::vector<void*>& args)
{
    // todo
    return nullptr;
}

void* Emitter::CreateClassDIType(void* classPtr)
{
    // todo
    return nullptr;
}

void* Emitter::CreateCall(void* callee, const std::vector<void*>& args)
{
    for (void* arg : args)
    {
        cmsxi::Value* argument = static_cast<cmsxi::Value*>(arg);
        context->CreateArg(argument);
    }
    cmsxi::Value* calleeValue = static_cast<cmsxi::Value*>(callee);
    return context->CreateCall(calleeValue);
}

void* Emitter::CreateCallInst(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span)
{
    for (void* arg : args)
    {
        cmsxi::Value* argument = static_cast<cmsxi::Value*>(arg);
        context->CreateArg(argument);
    }
    cmsxi::Value* calleeValue = static_cast<cmsxi::Value*>(callee);
    return context->CreateCall(calleeValue);
}

void* Emitter::CreateCallInstToBasicBlock(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, void* basicBlock, const Span& span)
{
    void* prevBasicBlock = context->GetCurrentBasicBlock();
    SetCurrentBasicBlock(basicBlock);
    for (void* arg : args)
    {
        cmsxi::Value* argument = static_cast<cmsxi::Value*>(arg);
        context->CreateArg(argument);
    }
    cmsxi::Value* calleeValue = static_cast<cmsxi::Value*>(callee);
    cmsxi::Instruction* callInst = context->CreateCall(calleeValue);
    SetCurrentBasicBlock(prevBasicBlock);
    return callInst;
}

void* Emitter::CreateInvoke(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args)
{
    // todo
    return nullptr;
}

void* Emitter::CreateInvokeInst(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span)
{
    // todo
    return nullptr;
}

void* Emitter::DIBuilder()
{
    // todo
    return nullptr;
}

void Emitter::SetCurrentDIBuilder(void* diBuilder_)
{
    // todo
}

void* Emitter::GetObjectFromClassDelegate(void* classDelegatePtr)
{
    return context->CreateElemAddr(static_cast<cmsxi::Value*>(classDelegatePtr), context->GetLongValue(0));
}

void* Emitter::GetDelegateFromClassDelegate(void* classDelegatePtr)
{
    return context->CreateElemAddr(static_cast<cmsxi::Value*>(classDelegatePtr), context->GetLongValue(1));
}

void* Emitter::GetObjectFromInterface(void* interfaceTypePtr)
{
    cmsxi::Value* addr = context->CreateElemAddr(static_cast<cmsxi::Value*>(interfaceTypePtr), context->GetLongValue(0));
    return context->CreateLoad(addr);
}

void* Emitter::GetObjectPtrFromInterface(void* interfaceTypePtr)
{
    return context->CreateElemAddr(static_cast<cmsxi::Value*>(interfaceTypePtr), context->GetLongValue(0));
}

void* Emitter::GetImtPtrFromInterface(void* interfaceTypePtr)
{
    cmsxi::Value* interfacePtrAddr = context->CreateElemAddr(static_cast<cmsxi::Value*>(interfaceTypePtr), context->GetLongValue(1));
    cmsxi::Value* interfacePtr = context->CreateLoad(interfacePtrAddr);
    return context->CreateBitCast(interfacePtr, context->GetPtrType(context->GetVoidType()));
}

void* Emitter::GetInterfaceMethod(void* imtPtr, int32_t methodIndex, void* interfaceMethodType)
{
    cmsxi::Value* methodPtrPtr = context->CreatePtrOffset(static_cast<cmsxi::Value*>(imtPtr), context->GetLongValue(methodIndex));
    cmsxi::Value* methodPtr = context->CreateLoad(methodPtrPtr);
    cmsxi::Value* callee = context->CreateBitCast(methodPtr, context->GetPtrType(static_cast<cmsxi::Type*>(interfaceMethodType)));
    return callee;
}

void* Emitter::GetFunctionIrType(void* functionSymbol) const
{
    auto it = functionIrTypeMap.find(functionSymbol);
    if (it != functionIrTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetFunctionIrType(void* symbol, void* irType)
{
    functionIrTypeMap[symbol] = static_cast<cmsxi::FunctionType*>(irType);
}

void* Emitter::GetVmtPtr(void* thisPtr, int32_t vmtPtrIndex, void* vmtPtrType)
{
    cmsxi::Value* vmtPtrPtr = context->CreateElemAddr(static_cast<cmsxi::Value*>(thisPtr), context->GetLongValue(vmtPtrIndex));
    cmsxi::Value* vmtPtr = context->CreateLoad(vmtPtrPtr);
    return context->CreateBitCast(vmtPtr, static_cast<cmsxi::Type*>(vmtPtrType));
}

void* Emitter::GetMethodPtr(void* vmtPtr, int32_t vmtIndex)
{
    cmsxi::Value* funPtrPtr = context->CreateElemAddr(static_cast<cmsxi::Value*>(vmtPtr), context->GetLongValue(vmtIndex));
    return context->CreateLoad(funPtrPtr);
}

void* Emitter::GetImtArray(void* vmtObjectPtr, int32_t imtsVmtIndexOffset)
{
    cmsxi::Value* imtsArrayPtrPtr = context->CreateElemAddr(static_cast<cmsxi::Value*>(vmtObjectPtr), context->GetLongValue(imtsVmtIndexOffset));
    cmsxi::Value* imtsArrayPtr = context->CreateBitCast(imtsArrayPtrPtr, context->GetPtrType(context->GetPtrType(context->GetVoidType())));
    return context->CreateLoad(imtsArrayPtr);
}

void* Emitter::GetImt(void* imtArray, int32_t interfaceIndex)
{
    cmsxi::Value* imtArrayPtr = context->CreatePtrOffset(static_cast<cmsxi::Value*>(imtArray), context->GetLongValue(interfaceIndex));
    return context->CreateLoad(imtArrayPtr);
}

void* Emitter::GetIrObject(void* symbol) const
{
    auto it = irObjectMap.find(symbol);
    if (it != irObjectMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("emitter: IR object not found");
    }
}

void Emitter::SetIrObject(void* symbol, void* irObject)
{
    irObjectMap[symbol] = static_cast<cmsxi::Value*>(irObject);
}

void* Emitter::GetMemberVariablePtr(void* classPtr, int32_t memberVariableLayoutIndex)
{
    cmsxi::Value* clsPtr = static_cast<cmsxi::Value*>(classPtr);
    return context->CreateElemAddr(clsPtr, context->GetLongValue(memberVariableLayoutIndex));
}

void* Emitter::SizeOf(void* ptrType)
{
    cmsxi::Value* nullPtr = context->GetNullValue(static_cast<cmsxi::PtrType*>(ptrType));
    cmsxi::Value* one = context->CreatePtrOffset(nullPtr, context->GetLongValue(1));
    cmsxi::Value* size = context->CreatePtrToInt(one, context->GetLongType());
    return size;
}

void Emitter::SetLineNumber(int32_t lineNumber)
{
    // todo
}

void Emitter::SaveObjectPointer(void* objectPointer_)
{
    if (objectPointer == nullptr)
    {
        objectPointer = static_cast<cmsxi::Value*>(objectPointer_);
    }
}

void Emitter::SetObjectPointer(void* objectPointer_)
{
    objectPointer = static_cast<cmsxi::Value*>(objectPointer_);
}

void* Emitter::GetObjectPointer()
{
    return objectPointer;
}

void* Emitter::GetClassIdPtr(void* vmtPtr)
{
    cmsxi::Value* classIdPtr = context->CreateElemAddr(static_cast<cmsxi::Value*>(vmtPtr), context->GetLongValue(0));
    return classIdPtr;
}

void* Emitter::GetClassName(void* vmtPtr, int32_t classNameVmtIndexOffset)
{
    cmsxi::Value* classNamePtr = context->CreateElemAddr(static_cast<cmsxi::Value*>(vmtPtr), context->GetLongValue(classNameVmtIndexOffset));
    cmsxi::Value* className = context->CreateLoad(classNamePtr);
    return className;
}

void* Emitter::ComputeAddress(void* ptr, void* index)
{
    return context->CreatePtrOffset(static_cast<cmsxi::Value*>(ptr), static_cast<cmsxi::Value*>(index));
}

void* Emitter::CreatePtrDiff(void* left, void* right)
{
    return context->CreatePtrDiff(static_cast<cmsxi::Value*>(left), static_cast<cmsxi::Value*>(right));
}

uint32_t Emitter::GetPrivateFlag()
{
    // todo
    return uint32_t();
}

uint32_t Emitter::GetProtectedFlag()
{
    // todo
    return uint32_t();
}

uint32_t Emitter::GetPublicFlag()
{
    // todo
    return uint32_t();
}

uint32_t Emitter::GetNoFlags()
{
    // todo
    return uint32_t();
}

void* Emitter::CreateModule(const std::string& moduleName)
{
    return new cmsxi::CompileUnit(moduleName);
}

void Emitter::DestroyModule(void* module)
{
    delete static_cast<cmsxi::CompileUnit*>(module);
}

void Emitter::SetModule(void* module_)
{
    compileUnit = static_cast<cmsxi::CompileUnit*>(module_);
    context = compileUnit->GetContext();
}

void Emitter::SetTargetTriple(const std::string& targetTriple)
{
    // todo
}

void Emitter::SetDataLayout(void* dataLayout_)
{
    // todo
}

void Emitter::SetSourceFileName(const std::string& sourceFileName)
{
    // todo
}

void Emitter::SetDICompileUnit(void* diCompileUnit_)
{
    // todo
}

void Emitter::SetDIFile(void* diFile_)
{
    // todo
}

void Emitter::SetColumnSpanProvider(cmajor::common::ColumnSpanProvider* columnSpanProvider_)
{
    // todo
}

void Emitter::ResetCurrentDebugLocation()
{
    // todo
}

void Emitter::StartDebugInfo(const std::string& sourceFilePath, const std::string& compilerVersion, bool optimized, cmajor::common::ColumnSpanProvider* columnSpanProvider_)
{
    // todo
}

void Emitter::FinalizeDebugInfo()
{
    // todo
}

void Emitter::EndDebugInfo()
{
    // todo
}

void Emitter::EmitIrText(const std::string& filePath)
{
    // todo
}

void Emitter::EmitOptIrText(const std::string& irFilePath, const std::string& optIrFilePath, int opmitizationLevel)
{
    // todo
}

void Emitter::VerifyModule()
{
    // todo
}

void Emitter::EmitObjectCodeFile(const std::string& objectFilePath)
{
    // todo
}

void* Emitter::CreateDebugInfoForNamespace(void* scope, const std::string& name)
{
    // todo
    return nullptr;
}

void* Emitter::GetDebugInfoForFile(int32_t fileIndex)
{
    // todo
    return nullptr;
}

void Emitter::PushScope(void* scope)
{
    // todo
}

void Emitter::PopScope()
{
    // todo
}

void* Emitter::CurrentScope()
{
    // todo
    return nullptr;
}

uint64_t Emitter::GetClassTypeSizeInBits(void* classIrType)
{
    // todo
    return uint64_t();
}

uint64_t Emitter::GetClassTypeAlignmentInBits(void* classIrType)
{
    // todo
    return uint64_t();
}

void Emitter::AddInlineFunctionAttribute(void* function)
{
    // todo
}

void Emitter::SetFunctionLinkageToLinkOnceODRLinkage(void* function)
{
    // todo
    //static_cast<cmsxi::Function*>(function)->SetLinkOnce();
}

void Emitter::SetFunction(void* function_)
{
    currentFunction = static_cast<cmsxi::Function*>(function_);
}

void Emitter::SetInPrologue(bool inPrologue_)
{
    // todo
}

void* Emitter::CreateSubroutineType(const std::vector<void*>& elementTypes)
{
    // todo
    return nullptr;
}

unsigned Emitter::GetPureVirtualVirtuality()
{
    // todo
    return unsigned();
}

unsigned Emitter::GetVirtualVirtuality()
{
    // todo
    return unsigned();
}

unsigned Emitter::GetFunctionFlags(bool isStatic, unsigned accessFlags, bool isExplicit)
{
    // todo
    return unsigned();
}

void* Emitter::CreateDIMethod(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned virtuality, unsigned vtableIndex, void* vtableHolder,
    unsigned flags)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDIFunction(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned flags)
{
    // todo
    return nullptr;
}

void Emitter::SetDISubprogram(void* function, void* subprogram)
{
    // todo
}

void* Emitter::CreateAlloca(void* irType)
{
    return context->CreateLocal(static_cast<cmsxi::Type*>(irType));
}

void* Emitter::CreateDIParameterVariable(const std::string& name, int index, const Span& span, void* irType, void* allocaInst)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDIAutoVariable(const std::string& name, const Span& span, void* irType, void* allocaInst)
{
    // todo
    return nullptr;
}

void* Emitter::GetFunctionArgument(void* function, int argumentIndex)
{
    return static_cast<cmsxi::Function*>(function)->GetParam(argumentIndex);
}

void Emitter::SetDebugLoc(void* callInst)
{
    // todo
}

void* Emitter::CreateRet(void* value)
{
    return context->CreateRet(static_cast<cmsxi::Value*>(value));
}

void* Emitter::CreateRetVoid()
{
    return context->CreateRet(nullptr);
}

void Emitter::SetPersonalityFunction(void* function, void* personalityFunction)
{
    // todo
}

void Emitter::AddNoUnwindAttribute(void* function)
{
    // todo
}

void Emitter::AddUWTableAttribute(void* function)
{
    // todo
}

void* Emitter::CreateLexicalBlock(const Span& span)
{
    // todo
    return nullptr;
}

void* Emitter::CreateSwitch(void* condition, void* defaultDest, unsigned numCases)
{
    return context->CreateSwitch(static_cast<cmsxi::Value*>(condition), static_cast<cmsxi::BasicBlock*>(defaultDest));
}

void Emitter::AddCase(void* switchInst, void* caseValue, void* caseDest)
{
    cmsxi::SwitchInstruction* inst = static_cast<cmsxi::SwitchInstruction*>(switchInst);
    inst->AddCase(static_cast<cmsxi::Value*>(caseValue), static_cast<cmsxi::BasicBlock*>(caseDest));
}

void* Emitter::GenerateTrap(const std::vector<void*>& args)
{
    std::vector<cmsxi::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<cmsxi::Value*>(arg));
    }
    return context->CreateTrap(arguments);
}

void Emitter::SetCompileUnitId(const std::string& compileUnitId)
{
    context->SetCompileUnitId(compileUnitId);
}

void* Emitter::GetClsIdValue(const std::string& typeId)
{
    return context->GetClsIdValue(typeId);
}

void* Emitter::CreateMDBool(bool value)
{
    return context->CreateMDBool(value);
}

void* Emitter::CreateMDLong(int64_t value)
{
    return context->CreateMDLong(value);
}

void* Emitter::CreateMDString(const std::string& value)
{
    return context->CreateMDString(value);
}

void* Emitter::CreateMDStructRef(int id)
{
    return context->CreateMDStructRef(id);
}

int Emitter::GetMDStructId(void* mdStruct)
{
    return static_cast<cmsxi::MDStruct*>(mdStruct)->Id();
}

void* Emitter::CreateMDStruct()
{
    return context->CreateMDStruct();
}

void Emitter::AddMDItem(void* mdStruct, const std::string& fieldName, void* mdItem)
{
    context->AddMDStructItem(static_cast<cmsxi::MDStruct*>(mdStruct), fieldName, static_cast<cmsxi::MDItem*>(mdItem));
}

void Emitter::SetFunctionMdId(void* function, int mdId)
{
    static_cast<cmsxi::Function*>(function)->SetMdId(mdId);
}

} // namespace cmsxbe
