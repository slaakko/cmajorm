// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxbe/Emitter.hpp>

namespace cmsxbe {

Emitter::Emitter(cmsxbe::EmittingContext* emittingContext_) : cmajor::ir::Emitter(&stack), emittingContext(emittingContext_)
{
}

void Emitter::SetEmittingDelegate(cmajor::ir::EmittingDelegate* emittingDelegate_)
{
    // todo
}

void* Emitter::GetIrTypeForBool()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForSByte()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForByte()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForShort()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForUShort()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForInt()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForUInt()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForLong()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForULong()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForFloat()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForDouble()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForChar()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForWChar()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForUChar()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForVoid()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForFunction(void* retType, const std::vector<void*>& paramTypes)
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForVariableParamFunction(void* retType)
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeByTypeId(const boost::uuids::uuid& typeId)
{
    // todo
    return nullptr;
}

void Emitter::SetIrTypeByTypeId(const boost::uuids::uuid& typeId, void* irType)
{
    // todo
}

void* Emitter::GetIrTypeForArrayType(void* elementType, int64_t size)
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForClassType(const std::vector<void*>& elementTypes)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFwdIrTypeForClassType()
{
    // todo
    return nullptr;
}

void Emitter::SetFwdIrTypeBody(void* forwardDeclaredType, const std::vector<void*>& elementTypes)
{
    // todo
}

void* Emitter::GetIrTypeForDelegateType(void* retType, const std::vector<void*>& paramTypes)
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForVoidPtrType()
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForStructType(const std::vector<void*>& elementTypes)
{
    // todo
    return nullptr;
}

void* Emitter::GetIrTypeForPtrType(void* baseIrType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForArrayType(void* arrayIrType, const std::vector<void*>& arrayOfDefaults)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForBool()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForSByte()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForByte()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForShort()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForUShort()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForInt()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForUInt()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForLong()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForULong()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForFloat()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForDouble()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForChar()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForWChar()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForUChar()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForStruct(void* irType, const std::vector<void*>& defaultMembers)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForDelegateType(void* irType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForVoidPtrType()
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForDerivedType(void* irType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateDefaultIrValueForPtrType(void* irType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForBool(bool value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForSByte(int8_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForByte(uint8_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForShort(int16_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForUShort(uint16_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForInt(int32_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForUInt(uint32_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForLong(int64_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForULong(uint64_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForFloat(float value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForDouble(double value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForChar(uint8_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForWChar(uint16_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForUChar(uint32_t value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForWString(void* wstringConstant)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForUString(void* ustringConstant)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForConstantArray(void* arrayIrType, const std::vector<void*>& elementConstants)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForConstantStruct(void* structIrType, const std::vector<void*>& elementConstants)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIrValueForUuid(void* uuidConstant)
{
    // todo
    return nullptr;
}

void* Emitter::CreateGlobalStringPtr(const std::string& name)
{
    // todo
    return nullptr;
}

void* Emitter::GetGlobalStringPtr(int stringId)
{
    // todo
    return nullptr;
}

void* Emitter::GetGlobalWStringConstant(int stringId)
{
    // todo
    return nullptr;
}

void* Emitter::GetGlobalUStringConstant(int stringId)
{
    // todo
    return nullptr;
}

void* Emitter::GetGlobalUuidConstant(int uuidId)
{
    // todo
    return nullptr;
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
    return uint64_t();
}

uint64_t Emitter::GetAlignmentInBits(void* irType)
{
    // todo
    return uint64_t();
}

void Emitter::SetCurrentDebugLocation(const Span& span)
{
    // todo
}

void* Emitter::GetArrayBeginAddress(void* arrayPtr)
{
    // todo
    return nullptr;
}

void* Emitter::GetArrayEndAddress(void* arrayPtr, uint64_t size)
{
    // todo
    return nullptr;
}

void* Emitter::CreateBasicBlock(const std::string& name)
{
    // todo
    return nullptr;
}

void Emitter::CreateBr(void* targetBasicBlock)
{
    // todo
}

void* Emitter::CurrentBasicBlock() const
{
    // todo
    return nullptr;
}

void Emitter::SetCurrentBasicBlock(void* basicBlock)
{
    // todo
}

void Emitter::CreateCondBr(void* cond, void* trueBasicBlock, void* falseBasicBlock)
{
    // todo
}

void* Emitter::CreateArrayIndexAddress(void* arrayPtr, void* index)
{
    // todo
    return nullptr;
}

void Emitter::CreateStore(void* value, void* ptr)
{
    // todo
}

void* Emitter::CreateLoad(void* ptr)
{
    // todo
    return nullptr;
}

void* Emitter::CreateAdd(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFAdd(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateSub(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFSub(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateMul(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFMul(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateUDiv(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateSDiv(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFDiv(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateURem(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateSRem(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateAnd(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateOr(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateXor(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateShl(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateAShr(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateLShr(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateICmpEQ(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFCmpOEQ(void* left, void* right)
{
    // todo
    return nullptr;
}

void* Emitter::CreateICmpULT(void* leftValue, void* rightValue)
{
    // todo
    return nullptr;
}

void* Emitter::CreateICmpSLT(void* leftValue, void* rightValue)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFCmpOLT(void* leftValue, void* rightValue)
{
    // todo
    return nullptr;
}

void* Emitter::CreateSExt(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateZExt(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFPExt(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateTrunc(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFPTrunc(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateBitCast(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateUIToFP(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateSIToFP(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFPToUI(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFPToSI(void* operand, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateIntToPtr(void* intValue, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreatePtrToInt(void* ptrValue, void* destinationType)
{
    // todo
    return nullptr;
}

void* Emitter::CreateNot(void* value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateNeg(void* value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateFNeg(void* value)
{
    // todo
    return nullptr;
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
    // todo
    return nullptr;
}

void Emitter::SetStaticObjectType(void* symbol, void* type)
{
    // todo
}

std::string Emitter::GetStaticObjectName(void* symbol) const
{
    // todo
    return std::string();
}

void Emitter::SetStaticObjectName(void* symbol, const std::string& staticObjectName)
{
    // todo
}

void* Emitter::GetOrInsertGlobal(const std::string& name, void* type)
{
    // todo
    return nullptr;
}

void* Emitter::GetOrInsertAnyComdat(const std::string& name, void* global)
{
    // todo
    return nullptr;
}

void* Emitter::GetOrInsertAnyFunctionComdat(const std::string& name, void* function)
{
    // todo
    return nullptr;
}

void* Emitter::GetOrInsertFunction(const std::string& name, void* type)
{
    // todo
    return nullptr;
}

void Emitter::SetInitializer(void* global, void* initializer)
{
    // todo
}

void Emitter::SetPrivateLinkage(void* global)
{
    // todo
}

bool Emitter::IsVmtObjectCreated(void* symbol) const
{
    // todo
    return bool();
}

void Emitter::SetVmtObjectCreated(void* symbol)
{
    // todo
}

bool Emitter::IsStaticObjectCreated(void* symbol) const
{
    // todo
    return bool();
}

void Emitter::SetStaticObjectCreated(void* symbol)
{
    // todo
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
    return nullptr;
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
    // todo
    return nullptr;
}

void* Emitter::CreateCallInst(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span)
{
    // todo
    return nullptr;
}

void* Emitter::CreateCallInstToBasicBlock(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, void* basicBlock, const Span& span)
{
    // todo
    return nullptr;
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
    // todo
    return nullptr;
}

void* Emitter::GetDelegateFromClassDelegate(void* classDelegatePtr)
{
    // todo
    return nullptr;
}

void* Emitter::GetObjectFromInterface(void* interfaceTypePtr)
{
    // todo
    return nullptr;
}

void* Emitter::GetObjectPtrFromInterface(void* interfaceTypePtr)
{
    // todo
    return nullptr;
}

void* Emitter::GetImtPtrFromInterface(void* interfaceTypePtr)
{
    // todo
    return nullptr;
}

void* Emitter::GetInterfaceMethod(void* imtPtr, int32_t methodIndex, void* interfaceMethodType)
{
    // todo
    return nullptr;
}

void* Emitter::GetFunctionIrType(void* functionSymbol) const
{
    // todo
    return nullptr;
}

void Emitter::SetFunctionIrType(void* symbol, void* irType)
{
    // todo
}

void* Emitter::GetVmtPtr(void* thisPtr, int32_t vmtPtrIndex, void* vmtPtrType)
{
    // todo
    return nullptr;
}

void* Emitter::GetMethodPtr(void* vmtPtr, int32_t vmtIndex)
{
    // todo
    return nullptr;
}

void* Emitter::GetImtArray(void* vmtObjectPtr, int32_t imtsVmtIndexOffset)
{
    // todo
    return nullptr;
}

void* Emitter::GetImt(void* imtArray, int32_t interfaceIndex)
{
    // todo
    return nullptr;
}

void* Emitter::GetIrObject(void* symbol) const
{
    // todo
    return nullptr;
}

void Emitter::SetIrObject(void* symbol, void* irObject)
{
    // todo
}

void* Emitter::GetMemberVariablePtr(void* classPtr, int32_t memberVariableLayoutIndex)
{
    // todo
    return nullptr;
}

void* Emitter::SizeOf(void* ptrType)
{
    // todo
    return nullptr;
}

void Emitter::SetLineNumber(int32_t lineNumber)
{
    // todo
}

void Emitter::SaveObjectPointer(void* objectPointer_)
{
    // todo
}

void Emitter::SetObjectPointer(void* objectPointer_)
{
    // todo
}

void* Emitter::GetObjectPointer()
{
    // todo
    return nullptr;
}

void* Emitter::GetClassIdPtr(void* vmtPtr)
{
    // todo
    return nullptr;
}

void* Emitter::GetClassName(void* vmtPtr, int32_t classNameVmtIndexOffset)
{
    // todo
    return nullptr;
}

void* Emitter::ComputeAddress(void* ptr, void* index)
{
    // todo
    return nullptr;
}

void* Emitter::CreatePtrDiff(void* left, void* right)
{
    // todo
    return nullptr;
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
    // todo
    return nullptr;
}

void Emitter::DestroyModule(void* module)
{
    // todo
}

void Emitter::SetModule(void* module_)
{
    // todo
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
}

void Emitter::SetFunction(void* function_)
{
    // todo
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
    // todo
    return nullptr;
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
    // todo
    return nullptr;
}

void Emitter::SetDebugLoc(void* callInst)
{
    // todo
}

void* Emitter::CreateRet(void* value)
{
    // todo
    return nullptr;
}

void* Emitter::CreateRetVoid()
{
    // todo
    return nullptr;
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
    // todo
    return nullptr;
}

void Emitter::AddCase(void* switchInst, void* caseValue, void* caseDest)
{
    // todo
}

} // namespace cmsxbe
