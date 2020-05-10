// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMBPPBE_EMITTER_INCLUDED
#define CMAJOR_CMBPPBE_EMITTER_INCLUDED
#include <cmajor/cmcppbe/EmittingContext.hpp>
#include <cmajor/cmcppbe/ValueStack.hpp>
#include <cmajor/cmcppi/CompileUnit.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <unordered_set>

namespace cmcppbe {

using soulng::lexer::Span;

class Emitter : public cmajor::ir::Emitter
{
public:
    Emitter(EmittingContext* emittingContext_);
    EmittingContext* GetEmittingContext() const { return emittingContext; }
    void SetEmittingDelegate(cmajor::ir::EmittingDelegate* emittingDelegate_) override;
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
    void* GetIrTypeByTypeId(const boost::uuids::uuid& typeId) override;
    void SetIrTypeByTypeId(const boost::uuids::uuid& typeId, void* irType) override;
    void* GetIrTypeForArrayType(void* elementType, int64_t size) override;
    void* GetIrTypeForClassType(const std::vector<void*>& elementTypes) override;
    void* CreateFwdIrTypeForClassType() override;
    void SetFwdIrTypeBody(void* forwardDeclaredType, const std::vector<void*>& elementTypes) override;
    void* GetIrTypeForDelegateType(void* retType, const std::vector<void*>& paramTypes) override;
    void* GetIrTypeForVoidPtrType() override;
    void* GetIrTypeForStructType(const std::vector<void*>& elementTypes) override;
    void* GetIrTypeForPtrType(void* baseIrType) override;
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
    void* CreateGlobalStringPtr(const std::string& name) override;
    void* GetGlobalStringPtr(int stringId) override;
    void* GetGlobalWStringConstant(int stringId) override;
    void* GetGlobalUStringConstant(int stringId) override;
    void* GetGlobalUuidConstant(int uuidId) override;
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
    void* CreateDITypeForEnumConstant(const std::string& name, int64_t value) override;
    void* CreateDITypeForEnumType(const std::string& name, const std::string& mangledName, const Span& span, const std::vector<void*>& enumConstantElements,
        uint64_t sizeInBits, uint32_t alignInBits, void* underlyingDIType) override;
    void* CreateIrDIForwardDeclaration(void* irType, const std::string& name, const std::string& mangledName, const Span& span) override;
    uint64_t GetOffsetInBits(void* classIrType, int layoutIndex) override;
    void* CreateDITypeForClassType(void* irType, const std::vector<void*>& memberVariableElements, const Span& classSpan, const std::string& name, void* vtableHolderClass,
        const std::string& mangledName, void* baseClassDIType) override;
    void MapFwdDeclaration(void* fwdDeclaration, const boost::uuids::uuid& typeId) override;
    void* GetDITypeByTypeId(const boost::uuids::uuid& typeId) const override;
    void SetDITypeByTypeId(const boost::uuids::uuid& typeId, void* diType, const std::string& typeName) override;
    void* GetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId) override;
    void SetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId, void* diType) override;
    void* CreateDIMemberType(void* scope, const std::string& name, const Span& span, uint64_t sizeInBits, uint64_t alignInBits, uint64_t offsetInBits, void* diType) override;
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
    int GetBasicBlockId(void* basicBlock) override;
    void CreateBr(void* targetBasicBlock) override;
    void* CurrentBasicBlock() const override;
    void SetCurrentBasicBlock(void* basicBlock) override;
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
    std::string GetVmtObjectName(void* symbol) const override;
    void SetVmtObjectName(void* symbol, const std::string& vmtObjectName) override;
    std::string GetImtArrayObjectName(void* symbol) const override;
    void SetImtArrayObjectName(void* symbol, const std::string& imtArrayObjectName) override;
    void* GetVmtObjectType(void* symbol) const override;
    void SetVmtObjectType(void* symbol, void* vmtObjectType) override;
    void* GetStaticObjectType(void* symbol) const override;
    void SetStaticObjectType(void* symbol, void* type) override;
    std::string GetStaticObjectName(void* symbol) const override;
    void SetStaticObjectName(void* symbol, const std::string& staticObjectName) override;
    void* GetOrInsertGlobal(const std::string& name, void* type) override;
    void* GetOrInsertAnyComdat(const std::string& name, void* global) override;
    void* GetOrInsertAnyFunctionComdat(const std::string& name, void* function) override;
    void* GetOrInsertFunction(const std::string& name, void* type) override;
    void SetInitializer(void* global, void* initializer) override;
    void SetPrivateLinkage(void* global) override;
    bool IsVmtObjectCreated(void* symbol) const override;
    void SetVmtObjectCreated(void* symbol) override;
    bool IsStaticObjectCreated(void* symbol) const override;
    void SetStaticObjectCreated(void* symbol) override;
    void* HandlerBlock() override;
    void* CleanupBlock() override;
    bool NewCleanupNeeded() override;
    void CreateCleanup() override;
    cmajor::ir::Pad* CurrentPad() override;
    void* CreateCleanupPadWithParent(void* parentPad, const std::vector<void*>& args) override;
    void* CreateCleanupPad(const std::vector<void*>& args) override;
    void* CreateCleanupRet(void* cleanupPad, void* unwindTarget) override;
    void* CreateCatchRet(void* catchPad, void* returnTarget) override;
    void* CreateCatchSwitch(void* unwindBlock) override;
    void* CreateCatchSwitchWithParent(void* parentPad, void* unwindBlock) override;
    void AddHandlerToCatchSwitch(void* catchSwitch, void* target) override;
    void* CreateCatchPad(void* parentPad, const std::vector<void*>& args) override;
    void* CreateClassDIType(void* classPtr) override;
    void* CreateCall(void* callee, const std::vector<void*>& args) override;
    void* CreateCallInst(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span) override;
    void* CreateCallInstToBasicBlock(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, void* basicBlock, const Span& span) override;
    void* CreateInvoke(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args) override;
    void* CreateInvokeInst(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span) override;
    void* DIBuilder() override;
    void SetCurrentDIBuilder(void* diBuilder_) override;
    void* GetObjectFromClassDelegate(void* classDelegatePtr) override;
    void* GetDelegateFromClassDelegate(void* classDelegatePtr) override;
    void* GetObjectFromInterface(void* interfaceTypePtr) override;
    void* GetObjectPtrFromInterface(void* interfaceTypePtr) override;
    void* GetImtPtrFromInterface(void* interfaceTypePtr) override;
    void* GetInterfaceMethod(void* imtPtr, int32_t methodIndex, void* interfaceMethodType) override;
    void* GetFunctionIrType(void* functionSymbol) const override;
    void SetFunctionIrType(void* symbol, void* irType) override;
    void* GetVmtPtr(void* thisPtr, int32_t vmtPtrIndex, void* vmtPtrType) override;
    void* GetMethodPtr(void* vmtPtr, int32_t vmtIndex) override;
    void* GetImtArray(void* vmtObjectPtr, int32_t imtsVmtIndexOffset) override;
    void* GetImt(void* imtArray, int32_t interfaceIndex) override;
    void* GetIrObject(void* symbol) const override;
    void SetIrObject(void* symbol, void* irObject) override;
    void* GetMemberVariablePtr(void* classPtr, int32_t memberVariableLayoutIndex) override;
    void* SizeOf(void* ptrType) override;
    void SetLineNumber(int32_t lineNumber) override;
    void SaveObjectPointer(void* objectPointer_) override;
    void SetObjectPointer(void* objectPointer_) override;
    void* GetObjectPointer() override;
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
    void SetModule(void* module_) override;
    void SetTargetTriple(const std::string& targetTriple) override;
    void SetDataLayout(void* dataLayout_) override;
    void SetSourceFileName(const std::string& sourceFileName) override;
    void SetDICompileUnit(void* diCompileUnit_) override;
    void SetDIFile(void* diFile_) override;
    void SetColumnSpanProvider(cmajor::common::ColumnSpanProvider* columnSpanProvider_) override;
    void ResetCurrentDebugLocation() override;
    void StartDebugInfo(const std::string& sourceFilePath, const std::string& compilerVersion, bool optimized, cmajor::common::ColumnSpanProvider* columnSpanProvider_) override;
    void FinalizeDebugInfo() override;
    void EndDebugInfo() override;
    void EmitIrText(const std::string& filePath) override;
    void EmitIrFile(const std::string& filePath) override;
    void Optimize(const std::string& bcFilePath, const std::string& optBCFilePath, const std::string& optimizationFlags) override;
    void Disassemble(const std::string& bcFilePath, const std::string& filePath) override;
    void Compile(const std::string& bcFilePath, const std::string& objectFilePath, int optimizationLevel) override;
    void VerifyModule() override;
    void EmitObjectCodeFile(const std::string& objectFilePath) override;
    void* CreateDebugInfoForNamespace(void* scope, const std::string& name) override;
    void* GetDebugInfoForFile(int32_t fileIndex) override;
    void PushScope(void* scope) override;
    void PopScope() override;
    void* CurrentScope() override;
    uint64_t GetClassTypeSizeInBits(void* classIrType) override;
    uint64_t GetClassTypeAlignmentInBits(void* classIrType) override;
    void AddInlineFunctionAttribute(void* function) override;
    void SetFunctionLinkage(void* function, bool setInline) override;
    void SetFunctionLinkageToLinkOnceODRLinkage(void* function) override;
    void SetFunctionCallConventionToStdCall(void* function) override;
    void SetFunction(void* function_) override;
    void SetInPrologue(bool inPrologue_) override;
    void* CreateSubroutineType(const std::vector<void*>& elementTypes) override;
    unsigned GetPureVirtualVirtuality() override;
    unsigned GetVirtualVirtuality() override;
    unsigned GetFunctionFlags(bool isStatic, unsigned accessFlags, bool isExplicit) override;
    void* CreateDIMethod(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned virtuality, unsigned vtableIndex, void* vtableHolder,
        unsigned flags) override;
    void* CreateDIFunction(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned flags) override;
    void SetDISubprogram(void* function, void* subprogram) override;
    void* CreateAlloca(void* irType) override;
    void* NewAllocaInst(void* irType) override;
    void* CreateDIParameterVariable(const std::string& name, int index, const Span& span, void* irType, void* allocaInst) override;
    void* CreateDIAutoVariable(const std::string& name, const Span& span, void* irType, void* allocaInst) override;
    void* GetFunctionArgument(void* function, int argumentIndex) override;
    void SetDebugLoc(void* callInst) override;
    void* CreateRet(void* value) override;
    void* CreateRetVoid() override;
    void SetPersonalityFunction(void* function, void* personalityFunction) override;
    void AddNoUnwindAttribute(void* function) override;
    void AddUWTableAttribute(void* function) override;
    void* CreateLexicalBlock(const Span& span) override;
    void* CreateSwitch(void* condition, void* defaultDest, unsigned numCases) override;
    void AddCase(void* switchInst, void* caseValue, void* caseDest) override;
    void* GenerateTrap(const std::vector<void*>& args) override;
    void SetCompileUnitId(const std::string& compileUnitId) override;
    void* GetClsIdValue(const std::string& typeId) override;
    void* CreateMDBool(bool value) override;
    void* CreateMDLong(int64_t value) override;
    void* CreateMDString(const std::string& value) override;
    void* CreateMDStructRef(int id) override;
    int GetMDStructId(void* mdStruct) override;
    void* CreateMDStruct() override;
    void* CreateMDBasicBlockRef(void* bb) override;
    void AddMDItem(void* mdStruct, const std::string& fieldName, void* mdItem) override;
    void SetFunctionMdId(void* function, int mdId) override;
    void SetCurrentLineNumber(int currentLineNumber) override;
    void* GetMDStructRefForSourceFile(const std::string& sourceFileName) override;
    void SetMetadataRef(void* inst, void* mdStructRef) override;
    void FinalizeFunction(void* function) override;
    int Install(const std::string& str) override;
    int Install(const std::u16string& str) override;
    int Install(const std::u32string& str) override;
    void* CreateLandingPad(void* lpType) override;
    void SetLandindPadAsCleanup(void* landingPad) override;
    void InsertAllocaIntoBasicBlock(void* allocaInst, void* lastAlloca, void* basicBlock) override;
    void AddClauseToLangdingPad(void* landingPad, void* exceptionTypeId) override;
    void* CreateExtractValue(void* aggregate, const std::vector<unsigned int>& indeces) override;
    void* CreateInsertValue(void* aggregate, void* value, const std::vector<unsigned int>& indeces) override;
    void* CreateUndefValue(void* type) override;
    void CreateResume(void* exception) override;
    void DebugPrintDebugInfo(const std::string& filePath) override;
private:
    cmcppbe::EmittingContext* emittingContext;
    cmajor::ir::EmittingDelegate* emittingDelegate;
    ValueStack stack;
    cmcppi::CompileUnit* compileUnit;
    cmcppi::Context* context;
    std::unordered_map<void*, cmcppi::FunctionType*> functionIrTypeMap;
    std::unordered_map<boost::uuids::uuid, cmcppi::Type*, boost::hash<boost::uuids::uuid>> irTypeTypeIdMap;
    cmcppi::Function* currentFunction;
    std::unordered_map<void*, cmcppi::Value*> irObjectMap;
    std::unordered_set<void*> vmtObjectCreatedSet;
    std::unordered_set<void*> staticObjectCreatedSet;
    std::unordered_map<void*, cmcppi::StructureType*> staticTypeMap;
    std::unordered_map<void*, std::string> staticObjectNameMap;
    cmcppi::Value* objectPointer;
};

} // namespace cmcppbe

#endif // CMAJOR_CMBPPBE_EMITTER_INCLUDED
