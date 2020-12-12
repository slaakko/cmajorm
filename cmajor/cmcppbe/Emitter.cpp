// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppbe/Emitter.hpp>
#include <cmajor/cmcppi/Context.hpp>
#include <cmajor/cmcppi/Function.hpp>
#include <cmajor/cmcppi/Type.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>

namespace cmcppbe {

Emitter::Emitter(cmcppbe::EmittingContext* emittingContext_) :
    cmajor::ir::Emitter(&stack), emittingContext(emittingContext_), emittingDelegate(nullptr), context(nullptr), compileUnit(nullptr), currentFunction(nullptr),
    objectPointer(nullptr), substituteLineNumber(false), currentLineNumber(-1)
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
    return context->GetCharType();
}

void* Emitter::GetIrTypeForWChar()
{
    return context->GetWCharType();
}

void* Emitter::GetIrTypeForUChar()
{
    return context->GetUCharType();
}

void* Emitter::GetIrTypeForVoid()
{
    return context->GetVoidType();
}

void* Emitter::GetIrTypeForFunction(void* retType, const std::vector<void*>& paramTypes)
{
    std::vector<cmcppi::Type*> parameterTypes;
    for (void* paramType : paramTypes)
    {
        parameterTypes.push_back(static_cast<cmcppi::Type*>(paramType));
    }
    return context->GetFunctionType(static_cast<cmcppi::Type*>(retType), parameterTypes);
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
    irTypeTypeIdMap[typeId] = static_cast<cmcppi::Type*>(irType);
}

void* Emitter::GetIrTypeForArrayType(void* elementType, int64_t size)
{
    return context->GetArrayType(static_cast<cmcppi::Type*>(elementType), size);
}

void* Emitter::GetIrTypeForClassType(const std::vector<void*>& elementTypes)
{
    std::vector<cmcppi::Type*> memberTypes;
    for (void* elementType : elementTypes)
    {
        memberTypes.push_back(static_cast<cmcppi::Type*>(elementType));
    }
    return context->GetStructureType(memberTypes);
}

void* Emitter::CreateFwdIrTypeForClassType()
{
    return context->CreateStructureType();
}

void Emitter::SetFwdIrTypeBody(void* forwardDeclaredType, const std::vector<void*>& elementTypes)
{
    std::vector<cmcppi::Type*> memberTypes;
    for (void* elementType : elementTypes)
    {
        memberTypes.push_back(static_cast<cmcppi::Type*>(elementType));
    }
    cmcppi::StructureType* structureType = static_cast<cmcppi::StructureType*>(forwardDeclaredType);
    structureType->SetMemberTypes(memberTypes);
}

void* Emitter::GetIrTypeForDelegateType(void* retType, const std::vector<void*>& paramTypes)
{
    std::vector<cmcppi::Type*> parameterTypes;
    for (void* paramType : paramTypes)
    {
        parameterTypes.push_back(static_cast<cmcppi::Type*>(paramType));
    }
    return context->GetPtrType(context->GetFunctionType(static_cast<cmcppi::Type*>(retType), parameterTypes));
}

void* Emitter::GetIrTypeForVoidPtrType()
{
    return context->GetPtrType(context->GetVoidType());
}

void* Emitter::GetIrTypeForStructType(const std::vector<void*>& elementTypes)
{
    std::vector<cmcppi::Type*> memberTypes;
    for (void* elementType : elementTypes)
    {
        memberTypes.push_back(static_cast<cmcppi::Type*>(elementType));
    }
    return context->GetStructureType(memberTypes);
}

void* Emitter::GetIrTypeForPtrType(void* baseIrType)
{
    return context->GetPtrType(static_cast<cmcppi::Type*>(baseIrType));
}

std::string Emitter::GetIrTypeName(void* irType)
{
    return static_cast<cmcppi::Type*>(irType)->Name();
}

std::string Emitter::MakeVmtVariableName(const std::string& vmtObjectName)
{
    return "__global_" + vmtObjectName;
}

void* Emitter::CreateDefaultIrValueForArrayType(void* arrayIrType, const std::vector<void*>& arrayOfDefaults)
{
    std::vector<cmcppi::Value*> arrayOfConstants;
    for (void* constant : arrayOfDefaults)
    {
        arrayOfConstants.push_back(static_cast<cmcppi::Value*>(constant));
    }
    cmcppi::Type* arrayType = static_cast<cmcppi::Type*>(arrayIrType);
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
    return context->GetDefaultWCharValue();
}

void* Emitter::CreateDefaultIrValueForUChar()
{
    return context->GetDefaultUCharValue();
}

void* Emitter::CreateDefaultIrValueForStruct(void* irType, const std::vector<void*>& defaultMembers)
{
    std::vector<cmcppi::Value*> arrayOfDefaults;
    for (void* constant : defaultMembers)
    {
        arrayOfDefaults.push_back(static_cast<cmcppi::Value*>(constant));
    }
    return context->GetStructureValue(static_cast<cmcppi::StructureType*>(irType), arrayOfDefaults);
}

void* Emitter::CreateDefaultIrValueForDelegateType(void* irType)
{
    return context->GetNullValue(static_cast<cmcppi::PtrType*>(irType));
}

void* Emitter::CreateDefaultIrValueForVoidPtrType()
{
    return context->GetNullValue(static_cast<cmcppi::PtrType*>(context->GetPtrType(context->GetVoidType())));
}

void* Emitter::CreateDefaultIrValueForDerivedType(void* irType)
{
    return static_cast<cmcppi::Type*>(irType)->DefaultValue();
}

void* Emitter::CreateDefaultIrValueForPtrType(void* irType)
{
    return context->GetNullValue(static_cast<cmcppi::PtrType*>(irType));
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
    if (substituteLineNumber)
    {
        return context->GetIntValue(currentLineNumber);
    }
    else
    {
        return context->GetIntValue(value);
    }
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
    return context->GetWCharValue(value);
}

void* Emitter::CreateIrValueForUChar(uint32_t value)
{
    return context->GetUCharValue(value);
}

void* Emitter::CreateIrValueForWString(void* wstringConstant)
{
    return static_cast<cmcppi::Value*>(wstringConstant);
}

void* Emitter::CreateIrValueForUString(void* ustringConstant)
{
    return static_cast<cmcppi::Value*>(ustringConstant);
}

void* Emitter::CreateIrValueForConstantArray(void* arrayIrType, const std::vector<void*>& elementConstants, const std::string& prefix)
{
    std::vector<cmcppi::Value*> elements;
    for (void* elementConstant : elementConstants)
    {
        elements.push_back(static_cast<cmcppi::ConstantValue*>(elementConstant));
    }
    return context->GetArrayValue(static_cast<cmcppi::ArrayType*>(arrayIrType), elements, prefix);
}

void* Emitter::CreateIrValueForConstantStruct(void* structIrType, const std::vector<void*>& elementConstants)
{
    std::vector<cmcppi::Value*> memberConstants;
    for (void* elementConstant : elementConstants)
    {
        memberConstants.push_back(static_cast<cmcppi::Value*>(elementConstant));
    }
    return context->GetStructureValue(static_cast<cmcppi::StructureType*>(structIrType), memberConstants);
}

void* Emitter::CreateIrValueForUuid(void* uuidConstant)
{
    cmcppi::Value* arg = context->CreatePtrOffset(static_cast<cmcppi::Value*>(uuidConstant), context->GetLongValue(0));
    return context->CreateBitCast(arg, context->GetPtrType(context->GetVoidType()));
}

void* Emitter::GetConversionValue(void* type, void* from)
{
    return context->GetConversionValue(static_cast<cmcppi::Type*>(type), static_cast<cmcppi::ConstantValue*>(from));
}

void* Emitter::CreateGlobalStringPtr(const std::string& stringValue)
{
    return context->CreateGlobalStringPtr(stringValue);
}

void* Emitter::CreateGlobalWStringPtr(const std::u16string& stringValue)
{
    return context->CreateGlobalWStringPtr(stringValue);
}

void* Emitter::CreateGlobalUStringPtr(const std::u32string& stringValue)
{
    return context->CreateGlobalUStringPtr(stringValue);
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

void* Emitter::CreateDITypeForEnumType(const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId, const std::vector<void*>& enumConstantElements,
    uint64_t sizeInBits, uint32_t alignInBits, void* underlyingDIType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrDIForwardDeclaration(void* irType, const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId)
{
    // todo
    return nullptr;
}

uint64_t Emitter::GetOffsetInBits(void* classIrType, int layoutIndex)
{
    // todo
    return uint64_t();
}

void* Emitter::CreateDITypeForClassType(void* irType, const std::vector<void*>& memberVariableElements, const Span& classSpan, const boost::uuids::uuid& moduleId, const std::string& name, void* vtableHolderClass,
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

void Emitter::SetDITypeByTypeId(const boost::uuids::uuid& typeId, void* diType, const std::string& typeName)
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

void* Emitter::CreateDIMemberType(void* scope, const std::string& name, const Span& span, const boost::uuids::uuid& moduleId, uint64_t sizeInBits, uint64_t alignInBits, uint64_t offsetInBits, void* diType)
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

void Emitter::MapClassPtr(const boost::uuids::uuid& typeId, void* classPtr, const std::string& className)
{
    // todo
}

uint64_t Emitter::GetSizeInBits(void* irType)
{
    // todo
    return 0;
    //return static_cast<cmcppi::Type*>(irType)->SizeInBits();
}

uint64_t Emitter::GetAlignmentInBits(void* irType)
{
    // todo
    return 0;
    //return static_cast<cmcppi::Type*>(irType)->AlignmentInBits();
}

void Emitter::SetCurrentDebugLocation(const Span& span)
{
    // todo
}

void* Emitter::GetArrayBeginAddress(void* arrayPtr)
{
    return context->CreateElemAddr(static_cast<cmcppi::Value*>(arrayPtr), context->GetLongValue(0));
}

void* Emitter::GetArrayEndAddress(void* arrayPtr, uint64_t size)
{
    return context->CreateElemAddr(static_cast<cmcppi::Value*>(arrayPtr), context->GetLongValue(size));
}

void* Emitter::CreateBasicBlock(const std::string& name)
{
    if (name == "cleanup")
    {
        return currentFunction->CreateCleanupBasicBlock(*context);
    }
    else
    {
        return currentFunction->CreateBasicBlock(name, *context);
    }
}

void* Emitter::CreateIncludeBasicBlockInstruction(void* basicBlock)
{
    return context->CreateIncludeBasicBlockInstruction(static_cast<cmcppi::BasicBlock*>(basicBlock));
}

void Emitter::PushParentBlock()
{
    context->PushParent();
}

void Emitter::PopParentBlock()
{
    context->PopParent();
}

void Emitter::SetHandlerBlock(void* tryBlock, void* catchBlock)
{
    context->SetHandlerBlock(static_cast<cmcppi::BasicBlock*>(tryBlock), static_cast<cmcppi::BasicBlock*>(catchBlock));
}

void Emitter::SetCleanupBlock(void* cleanupBlock)
{
    context->SetCleanupBlock(static_cast<cmcppi::BasicBlock*>(cleanupBlock));
}

int Emitter::GetBasicBlockId(void* basicBlock)
{
    return static_cast<cmcppi::BasicBlock*>(basicBlock)->Id();
}

void Emitter::CreateBr(void* targetBasicBlock)
{
    context->CreateJump(static_cast<cmcppi::BasicBlock*>(targetBasicBlock));
}

void* Emitter::CurrentBasicBlock() const
{
    return context->GetCurrentBasicBlock();
}

void Emitter::SetCurrentBasicBlock(void* basicBlock)
{
    context->SetCurrentBasicBlock(static_cast<cmcppi::BasicBlock*>(basicBlock));
}

void Emitter::CreateCondBr(void* cond, void* trueBasicBlock, void* falseBasicBlock)
{
    context->CreateBranch(static_cast<cmcppi::Value*>(cond), static_cast<cmcppi::BasicBlock*>(trueBasicBlock), static_cast<cmcppi::BasicBlock*>(falseBasicBlock));
}

void* Emitter::CreateArrayIndexAddress(void* arrayPtr, void* index)
{
    return context->CreateElemAddr(static_cast<cmcppi::Value*>(arrayPtr), static_cast<cmcppi::Value*>(index));
}

void Emitter::CreateStore(void* value, void* ptr)
{
    context->CreateStore(static_cast<cmcppi::Value*>(value), static_cast<cmcppi::Value*>(ptr));
}

void* Emitter::CreateLoad(void* ptr)
{
    return context->CreateLoad(static_cast<cmcppi::Value*>(ptr));
}

void* Emitter::CreateAdd(void* left, void* right)
{
    return context->CreateAdd(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateFAdd(void* left, void* right)
{
    return context->CreateAdd(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateSub(void* left, void* right)
{
    return context->CreateSub(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateFSub(void* left, void* right)
{
    return context->CreateSub(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateMul(void* left, void* right)
{
    return context->CreateMul(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateFMul(void* left, void* right)
{
    return context->CreateMul(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateUDiv(void* left, void* right)
{
    return context->CreateDiv(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateSDiv(void* left, void* right)
{
    return context->CreateDiv(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateFDiv(void* left, void* right)
{
    return context->CreateDiv(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateURem(void* left, void* right)
{
    return context->CreateMod(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateSRem(void* left, void* right)
{
    return context->CreateMod(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateAnd(void* left, void* right)
{
    return context->CreateAnd(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateOr(void* left, void* right)
{
    return context->CreateOr(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateXor(void* left, void* right)
{
    return context->CreateXor(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateShl(void* left, void* right)
{
    return context->CreateShl(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateAShr(void* left, void* right)
{
    return context->CreateShr(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateLShr(void* left, void* right)
{
    return context->CreateShr(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateICmpEQ(void* left, void* right)
{
    return context->CreateEqual(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateFCmpOEQ(void* left, void* right)
{
    return context->CreateEqual(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateICmpULT(void* left, void* right)
{
    return context->CreateLess(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateICmpSLT(void* left, void* right)
{
    return context->CreateLess(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateFCmpOLT(void* left, void* right)
{
    return context->CreateLess(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
}

void* Emitter::CreateSExt(void* operand, void* destinationType)
{
    return context->CreateSignExtend(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateZExt(void* operand, void* destinationType)
{
    return context->CreateZeroExtend(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateFPExt(void* operand, void* destinationType)
{
    return operand;
}

void* Emitter::CreateTrunc(void* operand, void* destinationType)
{
    return context->CreateTruncate(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateFPTrunc(void* operand, void* destinationType)
{
    return context->CreateBitCast(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateBitCast(void* operand, void* destinationType)
{
    return context->CreateBitCast(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateUIToFP(void* operand, void* destinationType)
{
    return context->CreateIntToFloat(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateSIToFP(void* operand, void* destinationType)
{
    return context->CreateIntToFloat(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateFPToUI(void* operand, void* destinationType)
{
    return context->CreateFloatToInt(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateFPToSI(void* operand, void* destinationType)
{
    return context->CreateFloatToInt(static_cast<cmcppi::Value*>(operand), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateIntToPtr(void* intValue, void* destinationType)
{
    return context->CreateIntToPtr(static_cast<cmcppi::Value*>(intValue), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreatePtrToInt(void* ptrValue, void* destinationType)
{
    return context->CreatePtrToInt(static_cast<cmcppi::Value*>(ptrValue), static_cast<cmcppi::Type*>(destinationType));
}

void* Emitter::CreateNot(void* value)
{
    return context->CreateNot(static_cast<cmcppi::Value*>(value));
}

void* Emitter::CreateNeg(void* value)
{
    return context->CreateNeg(static_cast<cmcppi::Value*>(value));
}

void* Emitter::CreateFNeg(void* value)
{
    cmcppi::Value* val = static_cast<cmcppi::Value*>(value);
    if (val->GetType(*context)->Id() == cmcppi::doubleTypeId)
    {
        cmcppi::ConstantValue* minusOne = context->GetDoubleValue(-1.0);
        return context->CreateMul(minusOne, val);
    }
    else if (val->GetType(*context)->Id() == cmcppi::floatTypeId)
    {
        cmcppi::ConstantValue* minusOne = context->GetFloatValue(-1.0f);
        return context->CreateMul(minusOne, val);
    }
    else
    {
        throw std::runtime_error("invalid FNeg operand type");
    }
}

void* Emitter::CreateNop()
{
    return context->CreateNop();
}

void* Emitter::CreateSave()
{
    return nullptr;
}

void* Emitter::CreateBeginTry()
{
    return context->CreateBeginTry();
}

void* Emitter::CreateEndTry(void* nextDest)
{
    return context->CreateEndTry(static_cast<cmcppi::BasicBlock*>(nextDest));
}

void* Emitter::CreateBeginCatch()
{
    return context->CreateBeginCatch();
}

void* Emitter::CreateEndCatch(void* nextDest)
{
    return context->CreateEndCatch(static_cast<cmcppi::BasicBlock*>(nextDest));
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
    staticTypeMap[symbol] = static_cast<cmcppi::StructureType*>(type);
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
    return context->GetOrInsertGlobal(name, static_cast<cmcppi::Type*>(type));
}

void* Emitter::GetOrInsertAnyComdat(const std::string& name, void* global)
{
    static_cast<cmcppi::GlobalVariable*>(global)->SetLinkOnce();
    return nullptr;
}

void* Emitter::GetOrInsertAnyFunctionComdat(const std::string& name, void* function)
{
    return nullptr;
}

void* Emitter::GetOrInsertFunction(const std::string& name, void* type, bool nothrow)
{
    cmcppi::Function* function = compileUnit->GetOrInsertFunction(name, static_cast<cmcppi::FunctionType*>(type));
    if (nothrow)
    {
        function->SetNothrow();
    }
    return function;
}


void Emitter::SetInitializer(void* global, void* initializer)
{
    cmcppi::GlobalVariable* globalVar = static_cast<cmcppi::GlobalVariable*>(global);
    globalVar->SetInitializer(static_cast<cmcppi::ConstantValue*>(initializer));
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
    return emittingDelegate->HandlerBlock();
}

void* Emitter::CleanupBlock()
{
    return emittingDelegate->CleanupBlock();
}

bool Emitter::NewCleanupNeeded()
{
    return false;
}

void Emitter::CreateCleanup()
{
    return emittingDelegate->CreateCleanup();
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
    std::vector<cmcppi::Value*> argInsts;
    for (void* arg : args)
    {
        cmcppi::Value* argument = static_cast<cmcppi::Value*>(arg);
        argInsts.push_back(context->CreateArg(argument));
    }
    cmcppi::Value* calleeValue = static_cast<cmcppi::Value*>(callee);
    return context->CreateCall(calleeValue, argInsts);
}

void* Emitter::CreateCallInst(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span)
{
    std::vector<cmcppi::Value*> argInsts;
    for (void* arg : args)
    {
        cmcppi::Value* argument = static_cast<cmcppi::Value*>(arg);
        argInsts.push_back(context->CreateArg(argument));
    }
    cmcppi::Value* calleeValue = static_cast<cmcppi::Value*>(callee);
    return context->CreateCall(calleeValue, argInsts);
}

void* Emitter::CreateCallInstToBasicBlock(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, void* basicBlock, const Span& span)
{
    std::vector<cmcppi::Value*> argInsts;
    void* prevBasicBlock = context->GetCurrentBasicBlock();
    SetCurrentBasicBlock(basicBlock);
    for (void* arg : args)
    {
        cmcppi::Value* argument = static_cast<cmcppi::Value*>(arg);
        argInsts.push_back(context->CreateArg(argument));
    }
    cmcppi::Value* calleeValue = static_cast<cmcppi::Value*>(callee);
    cmcppi::Instruction* callInst = context->CreateCall(calleeValue, argInsts);
    SetCurrentBasicBlock(prevBasicBlock);
    return callInst;
}

void* Emitter::CreateInvoke(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args)
{
    std::vector<cmcppi::Value*> argInsts;
    for (void* arg : args)
    {
        cmcppi::Value* argument = static_cast<cmcppi::Value*>(arg);
        argInsts.push_back(context->CreateArg(argument));
    }
    cmcppi::Value* calleeValue = static_cast<cmcppi::Value*>(callee);
    cmcppi::BasicBlock* normalBlockNext = static_cast<cmcppi::BasicBlock*>(normalBlock);
    cmcppi::BasicBlock* unwindBlockNext = static_cast<cmcppi::BasicBlock*>(unwindBlock);
    return context->CreateInvoke(calleeValue, argInsts, normalBlockNext, unwindBlockNext);
}

void* Emitter::CreateInvokeInst(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span)
{
    return CreateInvoke(callee, normalBlock, unwindBlock, args);
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
    return context->CreateElemAddr(static_cast<cmcppi::Value*>(classDelegatePtr), context->GetLongValue(0));
}

void* Emitter::GetDelegateFromClassDelegate(void* classDelegatePtr)
{
    return context->CreateElemAddr(static_cast<cmcppi::Value*>(classDelegatePtr), context->GetLongValue(1));
}

void* Emitter::GetObjectFromInterface(void* interfaceTypePtr)
{
    cmcppi::Value* addr = context->CreateElemAddr(static_cast<cmcppi::Value*>(interfaceTypePtr), context->GetLongValue(0));
    return context->CreateLoad(addr);
}

void* Emitter::GetObjectPtrFromInterface(void* interfaceTypePtr)
{
    return context->CreateElemAddr(static_cast<cmcppi::Value*>(interfaceTypePtr), context->GetLongValue(0));
}

void* Emitter::GetImtPtrFromInterface(void* interfaceTypePtr)
{
    cmcppi::Value* interfacePtrAddr = context->CreateElemAddr(static_cast<cmcppi::Value*>(interfaceTypePtr), context->GetLongValue(1));
    cmcppi::Value* interfacePtr = context->CreateLoad(interfacePtrAddr);
    return context->CreateBitCast(interfacePtr, context->GetPtrType(context->GetVoidType()));
}

void* Emitter::GetInterfaceMethod(void* imtPtr, int32_t methodIndex, void* interfaceMethodType)
{
    cmcppi::Value* methodPtrPtr = context->CreatePtrOffset(static_cast<cmcppi::Value*>(imtPtr), context->GetLongValue(methodIndex));
    cmcppi::Value* methodPtr = context->CreateLoad(methodPtrPtr);
    cmcppi::Value* callee = context->CreateBitCast(methodPtr, context->GetPtrType(static_cast<cmcppi::Type*>(interfaceMethodType)));
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
    functionIrTypeMap[symbol] = static_cast<cmcppi::FunctionType*>(irType);
}

void* Emitter::GetVmtPtr(void* thisPtr, int32_t vmtPtrIndex, void* vmtPtrType)
{
    cmcppi::Value* vmtPtrPtr = context->CreateElemAddr(static_cast<cmcppi::Value*>(thisPtr), context->GetLongValue(vmtPtrIndex));
    cmcppi::Value* vmtPtr = context->CreateLoad(vmtPtrPtr);
    return context->CreateBitCast(vmtPtr, static_cast<cmcppi::Type*>(vmtPtrType));
}

void* Emitter::GetMethodPtr(void* vmtPtr, int32_t vmtIndex)
{
    cmcppi::Value* funPtrPtr = context->CreateElemAddr(static_cast<cmcppi::Value*>(vmtPtr), context->GetLongValue(vmtIndex));
    return context->CreateLoad(funPtrPtr);
}

void* Emitter::GetImtArray(void* vmtObjectPtr, int32_t imtsVmtIndexOffset)
{
    cmcppi::Value* imtsArrayPtrPtr = context->CreateElemAddr(static_cast<cmcppi::Value*>(vmtObjectPtr), context->GetLongValue(imtsVmtIndexOffset));
    cmcppi::Value* imtsArrayPtr = context->CreateBitCast(imtsArrayPtrPtr, context->GetPtrType(context->GetPtrType(context->GetVoidType())));
    return context->CreateLoad(imtsArrayPtr);
}

void* Emitter::GetImt(void* imtArray, int32_t interfaceIndex)
{
    cmcppi::Value* imtArrayPtr = context->CreatePtrOffset(static_cast<cmcppi::Value*>(imtArray), context->GetLongValue(interfaceIndex));
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
    irObjectMap[symbol] = static_cast<cmcppi::Value*>(irObject);
}

void* Emitter::GetMemberVariablePtr(void* classPtr, int32_t memberVariableLayoutIndex)
{
    cmcppi::Value* clsPtr = static_cast<cmcppi::Value*>(classPtr);
    return context->CreateElemAddr(clsPtr, context->GetLongValue(memberVariableLayoutIndex));
}

void* Emitter::SizeOf(void* ptrType)
{
    cmcppi::Value* nullPtr = context->GetNullValue(static_cast<cmcppi::PtrType*>(ptrType));
    cmcppi::Value* one = context->CreatePtrOffset(nullPtr, context->GetLongValue(1));
    cmcppi::Value* size = context->CreatePtrToInt(one, context->GetLongType());
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
        objectPointer = static_cast<cmcppi::Value*>(objectPointer_);
    }
}

void Emitter::SetObjectPointer(void* objectPointer_)
{
    objectPointer = static_cast<cmcppi::Value*>(objectPointer_);
}

void* Emitter::GetObjectPointer()
{
    return objectPointer;
}

void* Emitter::GetClassIdPtr(void* vmtPtr, int32_t classIdVmtIndexOffset)
{
    cmcppi::Value* classIdPtr = context->CreateElemAddr(static_cast<cmcppi::Value*>(vmtPtr), context->GetLongValue(0));
    return classIdPtr;
}

void* Emitter::GetClassName(void* vmtPtr, int32_t classNameVmtIndexOffset)
{
    cmcppi::Value* classNamePtrPtr = context->CreateElemAddr(static_cast<cmcppi::Value*>(vmtPtr), context->GetLongValue(classNameVmtIndexOffset));
    cmcppi::Value* classNamePtr = context->CreateLoad(classNamePtrPtr);
    cmcppi::Value* className = context->CreateBitCast(classNamePtr, context->GetPtrType(context->GetCharType()));
    return className;
}

void* Emitter::ComputeAddress(void* ptr, void* index)
{
    return context->CreatePtrOffset(static_cast<cmcppi::Value*>(ptr), static_cast<cmcppi::Value*>(index));
}

void* Emitter::CreatePtrDiff(void* left, void* right)
{
    return context->CreatePtrDiff(static_cast<cmcppi::Value*>(left), static_cast<cmcppi::Value*>(right));
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
    return new cmcppi::CompileUnit(moduleName);
}

void Emitter::DestroyModule(void* module)
{
    delete static_cast<cmcppi::CompileUnit*>(module);
}

void Emitter::SetModule(void* module_)
{
    compileUnit = static_cast<cmcppi::CompileUnit*>(module_);
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

void Emitter::EmitIrFile(const std::string& filePath)
{
    // todo
}

void Emitter::Optimize(const std::string& bcFilePath, const std::string& optBCFilePath, const std::string& optimizationFlags)
{
    // todo
}

void Emitter::Disassemble(const std::string& bcFilePath, const std::string& filePath)
{
    // todo
}

void Emitter::Compile(const std::string& bcFilePath, const std::string& objectFilePath, int optimizationLevel)
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

void* Emitter::GetDebugInfoForFile(const Span& span, const boost::uuids::uuid& moduleId)
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

void Emitter::SetFunctionLinkage(void* function, bool setInline)
{
    // todo
}

void Emitter::SetFunctionLinkageToLinkOnceODRLinkage(void* function)
{
    static_cast<cmcppi::Function*>(function)->SetLinkOnce();
}

void Emitter::SetFunctionCallConventionToStdCall(void* function)
{
    // todo
}

void Emitter::SetFunction(void* function_, int32_t fileIndex, const boost::uuids::uuid& sourceModuleId, const boost::uuids::uuid& functionId)
{
    currentFunction = static_cast<cmcppi::Function*>(function_);
    currentFunction->SetFileIndex(fileIndex);
    currentFunction->SetSourceModuleId(sourceModuleId);
    currentFunction->SetFunctionId(functionId);
    context->SetCurrentFunction(currentFunction);
}

void Emitter::SetFunctionName(const std::string& functionName)
{
    currentFunction->SetFullName(functionName);
}

void Emitter::BeginScope()
{
    context->BeginScope();
}

void Emitter::EndScope()
{
    context->EndScope();
}

int16_t Emitter::GetCurrentScopeId() const
{
    return context->CurrentScopeId();
}

void Emitter::SetCurrentScopeId(int16_t scopeId)
{
    context->SetCurrentScopeId(scopeId);
}

int32_t Emitter::AddControlFlowGraphNode()
{
    return context->AddControlFlowGraphNode();
}

void Emitter::SetCurrentControlFlowGraphNodeId(int32_t controlFlowGraphNodeId)
{
    context->SetCurrentFlowGraphNodeId(controlFlowGraphNodeId);
}

void Emitter::AddControlFlowGraphEdge(int32_t startNodeId, int32_t endNodeId)
{
    context->AddControlFlowGraphEdge(startNodeId, endNodeId);
}

void Emitter::AddLocalVariable(const std::string& localVariableName, const boost::uuids::uuid& typeId, void* irObject)
{
    context->AddLocalVariable(localVariableName, typeId, static_cast<cmcppi::LocalInstruction*>(irObject));
}

void Emitter::BeginInstructionFlag(int16_t flag)
{
    context->BeginInstructionFlag(flag);
}

void Emitter::EndInstructionFlag(int16_t flag)
{
    context->EndInstructionFlag(flag);
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

void* Emitter::CreateDIMethod(const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId, void* subroutineType, unsigned virtuality, unsigned vtableIndex, void* vtableHolder,
    unsigned flags)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDIFunction(const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId, void* subroutineType, unsigned flags)
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
    return context->CreateLocal(static_cast<cmcppi::Type*>(irType));
}

void* Emitter::CreateDIParameterVariable(const std::string& name, int index, const Span& span, const boost::uuids::uuid& moduleId, void* irType, void* allocaInst)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDIAutoVariable(const std::string& name, const Span& span, const boost::uuids::uuid& moduleId, void* irType, void* allocaInst)
{
    return nullptr;
}

void* Emitter::GetFunctionArgument(void* function, int argumentIndex)
{
    return static_cast<cmcppi::Function*>(function)->GetParam(argumentIndex);
}

void Emitter::SetDebugLoc(void* callInst)
{
    // todo
}

void* Emitter::CreateRet(void* value)
{
    return context->CreateRet(static_cast<cmcppi::Value*>(value));
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

void* Emitter::CreateLexicalBlock(const Span& span, const boost::uuids::uuid& moduleId)
{
    // todo
    return nullptr;
}

void* Emitter::CreateSwitch(void* condition, void* defaultDest, unsigned numCases)
{
    return context->CreateSwitch(static_cast<cmcppi::Value*>(condition), static_cast<cmcppi::BasicBlock*>(defaultDest));
}

void Emitter::AddCase(void* switchInst, void* caseValue, void* caseDest)
{
    cmcppi::SwitchInstruction* inst = static_cast<cmcppi::SwitchInstruction*>(switchInst);
    inst->AddCase(static_cast<cmcppi::Value*>(caseValue), static_cast<cmcppi::BasicBlock*>(caseDest));
}

void* Emitter::GenerateTrap(const std::vector<void*>& args)
{
    return nullptr;
}

void Emitter::SetCompileUnitId(const std::string& compileUnitId)
{
    context->SetCompileUnitId(compileUnitId);
}

void* Emitter::GetClsIdValue(const std::string& typeId)
{
    return nullptr;
}

void* Emitter::CreateMDBool(bool value)
{
    return nullptr;
}

void* Emitter::CreateMDLong(int64_t value)
{
    return nullptr;
}

void* Emitter::CreateMDString(const std::string& value)
{
    return nullptr;
}

void* Emitter::CreateMDStructRef(int id)
{
    return nullptr;
}

int Emitter::GetMDStructId(void* mdStruct)
{
    return 0;
}

void* Emitter::CreateMDStruct()
{
    return nullptr;
}

void* Emitter::CreateMDBasicBlockRef(void* bb)
{
    return nullptr;
}

void Emitter::AddMDItem(void* mdStruct, const std::string& fieldName, void* mdItem)
{
}

void Emitter::SetFunctionMdId(void* function, int mdId)
{
}

void Emitter::SetCurrentSourceSpan(int32_t line, int16_t scol, int16_t ecol)
{
    context->SetCurrentSourceSpan(cmajor::debug::SourceSpan(line, scol, ecol));
}

void* Emitter::GetMDStructRefForSourceFile(const std::string& sourceFileName)
{
    return nullptr;
}

void Emitter::SetMetadataRef(void* inst, void* mdStructRef)
{
}

void Emitter::FinalizeFunction(void* function, bool hasCleanup)
{
    static_cast<cmcppi::Function*>(function)->Finalize(hasCleanup);
}

int Emitter::Install(const std::string& str)
{
    return emittingDelegate->Install(str);
}

int Emitter::Install(const std::u16string& str)
{
    return emittingDelegate->Install(str);
}

int Emitter::Install(const std::u32string& str)
{
    return emittingDelegate->Install(str);
}

void* Emitter::CreateLandingPad(void* lpType)
{
    return nullptr;
}

void Emitter::SetLandindPadAsCleanup(void* landingPad)
{
}

void Emitter::MoveAllocaIntoBasicBlock(void* allocaInst, void* lastAlloca, void* basicBlock)
{
}

void Emitter::AddClauseToLangdingPad(void* landingPad, void* exceptionTypeId)
{
}

void* Emitter::CreateExtractValue(void* aggregate, const std::vector<unsigned int>& indeces)
{
    return nullptr;
}

void* Emitter::CreateInsertValue(void* aggregate, void* value, const std::vector<unsigned int>& indeces)
{
    return nullptr;
}

void* Emitter::CreateUndefValue(void* type)
{
    return nullptr;
}

void Emitter::DebugPrintDebugInfo(const std::string& filePath)
{
}

void Emitter::CreateResume(void* exception)
{
    context->CreateResume();
}

void Emitter::BeginSubstituteLineNumber(int32_t lineNumber)
{
    substituteLineNumber = true;
    currentLineNumber = lineNumber;
}

void Emitter::EndSubstituteLineNumber()
{
    substituteLineNumber = false;
}

} // namespace cmsxbe
