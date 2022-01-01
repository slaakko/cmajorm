// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMLLVM_EMITTER_INCLUDED
#define CMAJOR_CMLLVM_EMITTER_INCLUDED
#include <cmajor/ir/EmittingContext.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/common/Common.hpp>
#include <cmajor/cmllvm/ValueStack.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DIBuilder.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <unordered_set>

namespace cmllvm {

using soulng::lexer::Span;

typedef llvm::SmallVector<llvm::Value*, 4> ArgVector;

class Emitter : public cmajor::ir::Emitter
{
public:
    Emitter(llvm::LLVMContext& context_, cmajor::ir::EmittingContext& emittingContext_);
    void SetEmittingDelegate(cmajor::ir::EmittingDelegate* emittingDelegate_) override { emittingDelegate = emittingDelegate_; }
    void* GetIrTypeForBool() override;
    void* GetIrTypeForSByte() override;
    void* GetIrTypeForByte() override;
    void* GetIrTypeForShort() override;
    void* GetIrTypeForUShort() override;
    void* GetIrTypeForInt() override;
    void* GetIrTypeForUInt() override;
    void* GetIrTypeForLong() override;
    void* GetIrTypeForULong() override;
    void* GetIrTypeForFloat() override;
    void* GetIrTypeForDouble() override;
    void* GetIrTypeForChar() override;
    void* GetIrTypeForWChar() override;
    void* GetIrTypeForUChar() override;
    void* GetIrTypeForVoid() override;
    void* GetIrTypeForFunction(void* retType, const std::vector<void*>& paramTypes) override;
    void* GetIrTypeForVariableParamFunction(void* retType) override;
    void* GetIrTypeForArrayType(void* elementType, int64_t size) override;
    void* GetIrTypeForClassType(const std::vector<void*>& elementTypes) override;
    void* CreateFwdIrTypeForClassType() override;
    void SetFwdIrTypeBody(void* forwardDeclaredType, const std::vector<void*>& elementTypes) override;
    void* GetIrTypeForDelegateType(void* retType, const std::vector<void*>& paramTypes) override;
    void* GetIrTypeForVoidPtrType() override;
    void* GetIrTypeForStructType(const std::vector<void*>& elementTypes) override;
    void* GetIrTypeByTypeId(const boost::uuids::uuid& typeId) override;
    void SetIrTypeByTypeId(const boost::uuids::uuid& typeId, void* irType) override;
    void* GetIrTypeForPtrType(void* baseIrType) override;
    std::string GetIrTypeName(void* irType) override;
    std::string MakeVmtVariableName(const std::string& vmtObjectName) override;
    void* CreateDefaultIrValueForArrayType(void* arrayIrType, const std::vector<void*>& arrayOfDefaults) override;
    void* CreateDefaultIrValueForBool() override;
    void* CreateDefaultIrValueForSByte() override;
    void* CreateDefaultIrValueForByte() override;
    void* CreateDefaultIrValueForShort() override;
    void* CreateDefaultIrValueForUShort() override;
    void* CreateDefaultIrValueForInt() override;
    void* CreateDefaultIrValueForUInt() override;
    void* CreateDefaultIrValueForLong() override;
    void* CreateDefaultIrValueForULong() override;
    void* CreateDefaultIrValueForFloat() override;
    void* CreateDefaultIrValueForDouble() override;
    void* CreateDefaultIrValueForChar() override;
    void* CreateDefaultIrValueForWChar() override;
    void* CreateDefaultIrValueForUChar() override;
    void* CreateDefaultIrValueForStruct(void* irType, const std::vector<void*>& defaultMembers) override;
    void* CreateDefaultIrValueForDelegateType(void* irType) override;
    void* CreateDefaultIrValueForVoidPtrType() override;
    void* CreateDefaultIrValueForDerivedType(void* irType) override;
    void* CreateDefaultIrValueForPtrType(void* irType) override;
    void* CreateIrValueForBool(bool value) override;
    void* CreateIrValueForSByte(int8_t value) override;
    void* CreateIrValueForByte(uint8_t value) override;
    void* CreateIrValueForShort(int16_t value) override;
    void* CreateIrValueForUShort(uint16_t value) override;
    void* CreateIrValueForInt(int32_t value) override;
    void* CreateIrValueForUInt(uint32_t value) override;
    void* CreateIrValueForLong(int64_t value) override;
    void* CreateIrValueForULong(uint64_t value) override;
    void* CreateIrValueForFloat(float value) override;
    void* CreateIrValueForDouble(double value) override;
    void* CreateIrValueForChar(uint8_t value) override;
    void* CreateIrValueForWChar(uint16_t value) override;
    void* CreateIrValueForUChar(uint32_t value) override;
    void* CreateIrValueForWString(void* wstringConstant) override;
    void* CreateIrValueForUString(void* ustringConstant) override;
    void* CreateIrValueForConstantArray(void* arrayIrType, const std::vector<void*>& elementConstants, const std::string& prefix) override;
    void* CreateIrValueForConstantStruct(void* structIrType, const std::vector<void*>& elementConstants) override;
    void* CreateIrValueForUuid(void* uuidConstant) override;
    void* GetConversionValue(void* type, void* from) override;
    void* CreateDITypeForBool() override;
    void* CreateDITypeForSByte() override;
    void* CreateDITypeForByte() override;
    void* CreateDITypeForShort() override;
    void* CreateDITypeForUShort() override;
    void* CreateDITypeForInt() override;
    void* CreateDITypeForUInt() override;
    void* CreateDITypeForLong() override;
    void* CreateDITypeForULong() override;
    void* CreateDITypeForFloat() override;
    void* CreateDITypeForDouble() override;
    void* CreateDITypeForChar() override;
    void* CreateDITypeForWChar() override;
    void* CreateDITypeForUChar() override;
    void* CreateDITypeForVoid() override;
    void* CreateDITypeForArray(void* elementDIType, const std::vector<void*>& elements) override;
    void* CreateIrDIForwardDeclaration(void* irType, const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId) override;
    uint64_t GetOffsetInBits(void* classIrType, int layoutIndex) override;
    void* CreateDITypeForClassType(void* irType, const std::vector<void*>& memberVariableElements, const Span& classSpan, const boost::uuids::uuid& moduleId, const std::string& name, void* vtableHolderClass,
        const std::string& mangledName, void* baseClassDIType) override;
    void* CreateDITypeForEnumConstant(const std::string& name, int64_t value) override;
    void* CreateDITypeForEnumType(const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId, const std::vector<void*>& enumConstantElements,
        uint64_t sizeInBits, uint32_t alignInBits, void* underlyingDIType) override;
    void MapFwdDeclaration(void* fwdDeclaration, const boost::uuids::uuid& typeId) override;
    void* GetDITypeByTypeId(const boost::uuids::uuid& typeId) const override;
    void SetDITypeByTypeId(const boost::uuids::uuid& typeId, void* diType, const std::string& typeName) override;
    void* GetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId) override;
    void SetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId, void* diType) override;
    void* CreateDIMemberType(void* scope, const std::string& name, const Span& span, const boost::uuids::uuid& moduleId, uint64_t sizeInBits, uint64_t alignInBits, uint64_t offsetInBits, void* diType) override;
    void* CreateConstDIType(void* diType) override;
    void* CreateLValueRefDIType(void* diType) override;
    void* CreateRValueRefDIType(void* diType) override;
    void* CreatePointerDIType(void* diType) override;
    void* CreateUnspecifiedDIType(const std::string& name) override;
    void MapClassPtr(const boost::uuids::uuid& typeId, void* classPtr, const std::string& className) override;
    uint64_t GetSizeInBits(void* irType) override;
    uint64_t GetAlignmentInBits(void* irType) override;
    void SetCurrentDebugLocation(const Span& span) override;
    void* GetArrayBeginAddress(void* arrayPtr) override;
    void* GetArrayEndAddress(void* arrayPtr, uint64_t size) override;
    void* CreateBasicBlock(const std::string& name) override;
    void* CreateIncludeBasicBlockInstruction(void* basicBlock) override;
    void PushParentBlock() override;
    void PopParentBlock() override;
    void SetHandlerBlock(void* tryBlock, void* catchBlock) override;
    void SetCleanupBlock(void* cleanupBlock) override;
    int GetBasicBlockId(void* basicBlock) override;
    void CreateBr(void* targetBasicBlock) override;
    void CreateCondBr(void* cond, void* trueBasicBlock, void* falseBasicBlock) override;
    void* CreateArrayIndexAddress(void* arrayPtr, void* index) override;
    void CreateStore(void* value, void* ptr) override;
    void* CreateLoad(void* ptr) override;
    void* CreateAdd(void* left, void* right) override;
    void* CreateFAdd(void* left, void* right) override;
    void* CreateSub(void* left, void* right) override;
    void* CreateFSub(void* left, void* right) override;
    void* CreateMul(void* left, void* right) override;
    void* CreateFMul(void* left, void* right) override;
    void* CreateUDiv(void* left, void* right) override;
    void* CreateSDiv(void* left, void* right) override;
    void* CreateFDiv(void* left, void* right) override;
    void* CreateURem(void* left, void* right) override;
    void* CreateSRem(void* left, void* right) override;
    void* CreateAnd(void* left, void* right) override;
    void* CreateOr(void* left, void* right) override;
    void* CreateXor(void* left, void* right) override;
    void* CreateShl(void* left, void* right) override;
    void* CreateAShr(void* left, void* right) override;
    void* CreateLShr(void* left, void* right) override;
    void* CreateICmpEQ(void* left, void* right) override;
    void* CreateFCmpOEQ(void* left, void* right) override;
    void* CreateICmpULT(void* leftValue, void* rightValue) override;
    void* CreateICmpSLT(void* leftValue, void* rightValue) override;
    void* CreateFCmpOLT(void* leftValue, void* rightValue) override;
    void* CreateSExt(void* operand, void* destinationType) override;
    void* CreateZExt(void* operand, void* destinationType) override;
    void* CreateFPExt(void* operand, void* destinationType) override;
    void* CreateTrunc(void* operand, void* destinationType) override;
    void* CreateFPTrunc(void* operand, void* destinationType) override;
    void* CreateBitCast(void* operand, void* destinationType) override;
    void* CreateUIToFP(void* operand, void* destinationType) override;
    void* CreateSIToFP(void* operand, void* destinationType) override;
    void* CreateFPToUI(void* operand, void* destinationType) override;
    void* CreateFPToSI(void* operand, void* destinationType) override;
    void* CreateIntToPtr(void* intValue, void* destinationType) override;
    void* CreatePtrToInt(void* ptrValue, void* destinationType) override;
    void* CreateNot(void* value) override;
    void* CreateNeg(void* value) override;
    void* CreateFNeg(void* value) override;
    void* CreateNop() override;
    void* CreateSave() override;
    void* CreateBeginTry() override;
    void* CreateEndTry(void* nextDest) override;
    void* CreateBeginCatch() override;
    void* CreateEndCatch(void* nextDest) override;
    void* GetOrInsertGlobal(const std::string& name, void* type) override;
    void* GetOrInsertAnyComdat(const std::string& name, void* global) override;
    void* GetOrInsertAnyFunctionComdat(const std::string& name, void* function) override;
    void SetInitializer(void* global, void* initializer) override;
    void SetPrivateLinkage(void* global) override;
    void* GetOrInsertFunction(const std::string& name, void* type, bool nothrow) override;
    void* CreateGlobalStringPtr(const std::string& name) override;
    void* CreateGlobalWStringPtr(const std::u16string& name) override;
    void* CreateGlobalUStringPtr(const std::u32string& name) override;
    void* CreateCall(void* callee, const std::vector<void*>& args) override;
    void* CreateCallInst(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span) override;
    void* CreateCallInstToBasicBlock(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, void* basicBlock, const Span& span) override;
    void* CreateInvoke(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args) override;
    void* CreateInvokeInst(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span) override;
    void* DIBuilder() override { return currentDIBuilder; }
    void SetCurrentDIBuilder(void* diBuilder_) override { currentDIBuilder = static_cast<llvm::DIBuilder*>(diBuilder_); }
    void* GetObjectFromClassDelegate(void* classDelegatePtr) override;
    void* GetDelegateFromClassDelegate(void* classDelegatePtr) override;
    void* GetObjectFromInterface(void* interfaceTypePtr) override;
    void* GetObjectPtrFromInterface(void* interfaceTypePtr) override;
    void* GetImtPtrPtrFromInterface(void* interfaceTypePtr) override;
    void* GetImtPtrFromInterface(void* interfaceTypePtr) override;
    void* GetInterfaceMethod(void* imtPtr, int32_t methodIndex, void* interfaceMethodType) override;
    void* GetVmtPtr(void* thisPtr, int32_t vmtPtrIndex, void* vmtPtrType) override;
    void* GetMethodPtr(void* vmtPtr, int32_t vmtIndex) override;
    void* GetImtArray(void* vmtObjectPtr, int32_t imtsVmtIndexOffset) override;
    void* GetImt(void* imtArray, int32_t interfaceIndex) override;
    void* GetMemberVariablePtr(void* classPtr, int32_t memberVariableLayoutIndex) override;
    void* SizeOf(void* ptrType) override;
    void* GetClassIdPtr(void* vmtPtr, int32_t classIdVmtIndexOffset) override;
    void* GetClassName(void* vmtPtr, int32_t classNameVmtIndexOffset) override;
    void* ComputeAddress(void* ptr, void* index) override;
    void* CreatePtrDiff(void* left, void* right) override;
    uint32_t GetPrivateFlag() override;
    uint32_t GetProtectedFlag() override;
    uint32_t GetPublicFlag() override;
    uint32_t GetNoFlags() override;
    void* CreateModule(const std::string& moduleName) override;
    void DestroyModule(void* module) override;
    void SetModule(void* module_) override { module = static_cast<llvm::Module*>(module_); }
    llvm::LLVMContext& Context() { return context; }
    llvm::IRBuilder<>& Builder() { return builder; }
    llvm::Module* Module() { return module; }
    llvm::DataLayout* DataLayout() { return dataLayout; }
    llvm::DICompileUnit* DICompileUnit() { return diCompileUnit; }
    llvm::DIFile* DIFile() { return diFile; }
    ValueStack& Stack() { return stack; }
    void SetTargetTriple(const std::string& targetTriple) override { module->setTargetTriple(targetTriple); }
    void SetDataLayout(void* dataLayout_) override { dataLayout = static_cast<llvm::DataLayout*>(dataLayout_); module->setDataLayout(*dataLayout); }
    void SetSourceFileName(const std::string& sourceFileName) override { module->setSourceFileName(sourceFileName); }
    void SetDICompileUnit(void* diCompileUnit_) override { diCompileUnit = static_cast<llvm::DICompileUnit*>(diCompileUnit_); }
    void SetDIFile(void* diFile_) override;
    void SetColumnSpanProvider(cmajor::common::ColumnSpanProvider* columnSpanProvider_) override { columnSpanProvider = columnSpanProvider_; }
    void SaveObjectPointer(void* objectPointer_) override;
    void SetObjectPointer(void* objectPointer_) override { objectPointer = static_cast<llvm::Value*>(objectPointer_); }
    void* GetObjectPointer() override { return objectPointer; }
    void SetFunction(void* function_, int32_t fileIndex, const boost::uuids::uuid& sourceModuleId, const boost::uuids::uuid& functionId) override { function = static_cast<llvm::Function*>(function_); }
    void SetFunctionName(const std::string& functionName) override;
    void BeginScope() override;
    void EndScope() override;
    int16_t GetCurrentScopeId() const override;
    void SetCurrentScopeId(int16_t scopeId) override;
    int32_t AddControlFlowGraphNode() override;
    void SetCurrentControlFlowGraphNodeId(int32_t controlFlowGraphNodeId) override;
    void AddControlFlowGraphEdge(int32_t startNodeId, int32_t endNodeId) override;
    void AddLocalVariable(const std::string& localVariableName, const boost::uuids::uuid& typeId, void* irObject) override;
    void BeginInstructionFlag(int16_t flag) override;
    void EndInstructionFlag(int16_t flag) override;
    llvm::Function* Function() { return function; }
    void* CurrentBasicBlock() const override { return currentBasicBlock; }
    void SetCurrentBasicBlock(void* currentBasicBlock_) override { currentBasicBlock = static_cast<llvm::BasicBlock*>(currentBasicBlock_); builder.SetInsertPoint(currentBasicBlock); }
    void SetInPrologue(bool inPrologue_) override { inPrologue = inPrologue_; }
    void PushScope(void* scope) override;
    void PopScope() override;
    void* CurrentScope() override;
    int GetColumn(const Span& span) const;
    void StartDebugInfo(const std::string& sourceFilePath, const std::string& compilerVersion, bool optimized, cmajor::common::ColumnSpanProvider* columnSpanProvider_) override;
    void FinalizeDebugInfo() override;
    void EndDebugInfo() override;
    void* CreateDebugInfoForNamespace(void* scope, const std::string& name) override;
    void EmitIrText(const std::string& filePath) override;
    void EmitIrFile(const std::string& filePath) override;
    void Optimize(const std::string& bcFilePath, const std::string& optBCFilePath, const std::string& optimizationFlags) override;
    void Disassemble(const std::string& bcFilePath, const std::string& llFilePath) override;
    void Compile(const std::string& bcFilePath, const std::string& objectFilePath, int optimizationLevel) override;
    void VerifyModule() override;
    void EmitObjectCodeFile(const std::string& objectFilePath) override;
    void ResetCurrentDebugLocation() override;
    llvm::DebugLoc GetDebugLocation(const Span& span);
    llvm::DebugLoc GetCurrentDebugLocation() { return currentDebugLocation; }
    void* GetDebugInfoForFile(const Span& span, const boost::uuids::uuid& moduleId) override;
    void ReplaceForwardDeclarations();
    void* GetIrObject(void* symbol) const override;
    void SetIrObject(void* symbol, void* irObject) override;
    void* GetFunctionIrType(void* symbol) const override;
    void SetFunctionIrType(void* symbol, void* irType) override;
    bool IsVmtObjectCreated(void* symbol) const override;
    void SetVmtObjectCreated(void* symbol) override;
    bool IsStaticObjectCreated(void* symbol) const override;
    void SetStaticObjectCreated(void* symbol) override;
    void* GetStaticObjectType(void* symbol) const override;
    void SetStaticObjectType(void* symbol, void* type) override;
    void* GetVmtObjectType(void* symbol) const override;
    void SetVmtObjectType(void* symbol, void* vmtObjectType) override;
    std::string GetStaticObjectName(void* symbol) const override;
    void SetStaticObjectName(void* symbol, const std::string& staticObjectName) override;
    std::string GetVmtObjectName(void* symbol) const override;
    void SetVmtObjectName(void* symbol, const std::string& vmtObjectName) override;
    std::string GetImtArrayObjectName(void* symbol) const override;
    void SetImtArrayObjectName(void* symbol, const std::string& imtArrayObjectName) override;
    uint64_t GetClassTypeSizeInBits(void* classIrType) override;
    uint64_t GetClassTypeAlignmentInBits(void* classIrType) override;
    void AddInlineFunctionAttribute(void* function) override;
    void SetFunctionLinkage(void* function, bool setInline) override;
    void SetFunctionLinkageToLinkOnceODRLinkage(void* function) override;
    void SetFunctionCallConventionToStdCall(void* function) override;
    void* CreateSubroutineType(const std::vector<void*>& elementTypes) override;
     unsigned GetPureVirtualVirtuality() override;
    unsigned GetVirtualVirtuality() override;
    unsigned GetFunctionFlags(bool isStatic, unsigned accessFlags, bool isExplicit) override;
    void* CreateDIMethod(const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId, void* subroutineType, unsigned virtuality, unsigned vtableIndex, void* vtableHolder,
        unsigned flags) override;
    void* CreateDIFunction(const std::string& name, const std::string& mangledName, const Span& span, const boost::uuids::uuid& moduleId, void* subroutineType, unsigned flags) override;
    void SetDISubprogram(void* function, void* subprogram) override;
    void* CreateAlloca(void* irType) override;
    void* CreateDIParameterVariable(const std::string& name, int index, const Span& span, const boost::uuids::uuid& moduleId, void* irType, void* allocaInst) override;
    void* CreateDIAutoVariable(const std::string& name, const Span& span, const boost::uuids::uuid& moduleId, void* irType, void* allocaInst) override;
    void* GetFunctionArgument(void* function, int argumentIndex) override;
    void SetDebugLoc(void* callInst) override;
    void* CreateRet(void* value) override;
    void* CreateRetVoid() override;
    void SetPersonalityFunction(void* function, void* personalityFunction) override;
    void AddNoUnwindAttribute(void* function) override;
    void AddUWTableAttribute(void* function) override;
    void* CreateLexicalBlock(const Span& span, const boost::uuids::uuid& moduleId) override;
    void* CreateSwitch(void* condition, void* defaultDest, unsigned numCases) override;
    void AddCase(void* switchInst, void* caseValue, void* caseDest) override;
    void* CreateCleanupPadWithParent(void* parentPad, const std::vector<void*>& args) override;
    void* CreateCleanupPad(const std::vector<void*>& args) override;
    void* CreateCleanupRet(void* cleanupPad, void* unwindTarget) override;
    void* CreateCatchRet(void* catchPad, void* returnTarget) override;
    void* CreateCatchSwitch(void* unwindBlock) override;
    void* CreateCatchSwitchWithParent(void* parentPad, void* unwindBlock) override;
    void AddHandlerToCatchSwitch(void* catchSwitch, void* target) override;
    void* CreateCatchPad(void* parentPad, const std::vector<void*>& args) override;
    void SetLineNumber(int32_t lineNumber) override { emittingDelegate->SetLineNumber(lineNumber); }
    void* HandlerBlock() override { return emittingDelegate->HandlerBlock(); }
    void* CleanupBlock() override { return emittingDelegate->CleanupBlock(); }
    bool NewCleanupNeeded() override { return emittingDelegate->NewCleanupNeeded(); }
    void CreateCleanup() override { emittingDelegate->CreateCleanup(); }
    std::string GetSourceFilePath(const Span& span, const boost::uuids::uuid& moduleId) { return emittingDelegate->GetSourceFilePath(span, moduleId); }
    cmajor::ir::Pad* CurrentPad() override { return emittingDelegate->CurrentPad(); }
    void* CreateClassDIType(void* classPtr) override { return emittingDelegate->CreateClassDIType(classPtr); }
    void* GetGlobalStringPtr(int stringId) override { return emittingDelegate->GetGlobalStringPtr(stringId); }
    void* GetGlobalWStringConstant(int stringId) override { return emittingDelegate->GetGlobalWStringConstant(stringId); }
    void* GetGlobalUStringConstant(int stringId) override { return emittingDelegate->GetGlobalUStringConstant(stringId); }
    void* GetGlobalUuidConstant(int uuidId) override { return emittingDelegate->GetGlobalUuidConstant(uuidId); }
    void* GenerateTrap(const std::vector<void*>& args) override;
    void SetCompileUnitId(const std::string& compileUnitId_) override;
    void* GetClsIdValue(const std::string& typeId) override;
    void* CreateMDBool(bool value) override;
    void* CreateMDLong(int64_t value) override;
    void* CreateMDString(const std::string& value) override;
    void* CreateMDStructRef(int id) override;
    void* CreateMDStruct() override;
    void* CreateMDBasicBlockRef(void* bb) override;
    int GetMDStructId(void* mdStruct) override;
    void AddMDItem(void* mdStruct, const std::string& fieldName, void* mdItem) override;
    void SetFunctionMdId(void* function, int mdId) override;
    void SetCurrentSourceSpan(int32_t line, int16_t scol, int16_t ecol) override;
    void* GetMDStructRefForSourceFile(const std::string& sourceFileName) override;
    void SetMetadataRef(void* inst, void* mdStructRef) override;
    void FinalizeFunction(void* function, bool hasCleanup) override;
    int Install(const std::string& str) override;
    int Install(const std::u16string& str) override;
    int Install(const std::u32string& str) override;
    void* CreateLandingPad(void* lpType) override;
    void SetLandindPadAsCleanup(void* landingPad) override;
    void MoveAllocaIntoBasicBlock(void* allocaInst, void* lastAlloca, void* basicBlock) override;
    void AddClauseToLangdingPad(void* landingPad, void* exceptionTypeId) override;
    void* CreateExtractValue(void* aggregate, const std::vector<unsigned int>& indeces) override;
    void* CreateInsertValue(void* aggregate, void* value, const std::vector<unsigned int>& indeces) override;
    void* CreateUndefValue(void* type) override;
    void CreateResume(void* exception) override;
    void DebugPrintDebugInfo(const std::string& filePath) override;
    void BeginSubstituteLineNumber(int32_t lineNumber) override;
    void EndSubstituteLineNumber() override;
    void SetBoundCompileUnit(void* boundCompileUnit_) override { }
    void* GetBoundCompileUnit() const override { return nullptr; }
private:
    cmajor::ir::EmittingContext& emittingContext;
    cmajor::ir::EmittingDelegate* emittingDelegate;
    llvm::LLVMContext& context;
    llvm::IRBuilder<> builder;
    llvm::Module* module;
    llvm::DataLayout* dataLayout;
    llvm::DICompileUnit* diCompileUnit;
    llvm::DIFile* diFile;
    std::unique_ptr<llvm::DIBuilder> diBuilder;
    llvm::DIBuilder* currentDIBuilder;
    cmajor::common::ColumnSpanProvider* columnSpanProvider;
    int32_t compileUnitIndex;
    std::string compileUnitId;
    ValueStack stack;
    llvm::Value* objectPointer;
    llvm::Function* function;
    llvm::BasicBlock* currentBasicBlock;
    std::vector<llvm::DIScope*> scopes;
    llvm::DebugLoc currentDebugLocation;
    bool inPrologue;
    std::unordered_map<int32_t, llvm::DIFile*> fileMap;
    std::unordered_map<boost::uuids::uuid, llvm::DIType*, boost::hash<boost::uuids::uuid>> diTypeTypeIdMap;
    std::unordered_map<llvm::DIType*, std::string> diTypeNameMap;
    std::unordered_map<std::pair<boost::uuids::uuid, int32_t>, llvm::DIDerivedType*, boost::hash<std::pair<boost::uuids::uuid, int32_t>>> diMemberTypeMap;
    std::unordered_map<llvm::DIType*, boost::uuids::uuid> fwdDeclarationMap;
    std::unordered_map<boost::uuids::uuid, void*, boost::hash<boost::uuids::uuid>> classPtrMap;
    std::unordered_map<void*, std::string> classNameMap;
    std::unordered_map<void*, llvm::Value*> irObjectMap;
    std::unordered_map<void*, llvm::Type*> irTypeMap;
    std::unordered_map<boost::uuids::uuid, llvm::Type*, boost::hash<boost::uuids::uuid>> irTypeTypeIdMap;
    std::unordered_map<void*, llvm::FunctionType*> functionIrTypeMap;
    std::unordered_set<void*> vmtObjectCreatedSet;
    std::unordered_set<void*> staticObjectCreatedSet;
    std::unordered_map<void*, llvm::StructType*> staticTypeMap;
    std::unordered_map<void*, llvm::ArrayType*> vmtObjectTypeMap;
    std::unordered_map<void*, std::string> staticObjectNameMap;
    std::unordered_map<void*, std::string> vmtObjectNameMap;
    std::unordered_map<void*, std::string> imtArrayObjectNameMap;
    int32_t currentLineNumber;
    bool substituteLineNumber;
};

} // namespace cmllvm

#endif // CMAJOR_CMLLVM_EMITTER_INCLUDED
