// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_CONTEXT_INCLUDED
#define CMAJOR_CMCPPI_CONTEXT_INCLUDED
#include <cmajor/cmcppi/Type.hpp>
#include <cmajor/cmcppi/Data.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <boost/uuid/uuid.hpp>
#include <stack>

namespace cmcppi {

class Function;
class Instruction;
class BasicBlock;
class LocalInstruction;

class CMCPPI_API Context
{
public:
    Context();
    TypeRepository& GetTypeRepository() { return typeRepository; }
    DataRepository& GetDataRepository() { return dataRepository; }
    Type* GetVoidType() { return typeRepository.GetVoidType(); }
    Type* GetBoolType() { return typeRepository.GetBoolType(); }
    Type* GetSByteType() { return typeRepository.GetSByteType(); }
    Type* GetByteType() { return typeRepository.GetByteType(); }
    Type* GetShortType() { return typeRepository.GetShortType(); }
    Type* GetUShortType() { return typeRepository.GetUShortType(); }
    Type* GetIntType() { return typeRepository.GetIntType(); }
    Type* GetUIntType() { return typeRepository.GetUIntType(); }
    Type* GetLongType() { return typeRepository.GetLongType(); }
    Type* GetULongType() { return typeRepository.GetULongType(); }
    Type* GetFloatType() { return typeRepository.GetFloatType(); }
    Type* GetDoubleType() { return typeRepository.GetDoubleType(); }
    Type* GetCharType() { return typeRepository.GetCharType(); }
    Type* GetWCharType() { return typeRepository.GetWCharType(); }
    Type* GetUCharType() { return typeRepository.GetUCharType(); }
    Type* GetPtrType(Type* baseType) { return typeRepository.GetPtrType(baseType); }
    Type* GetStructureType(const std::vector<Type*>& memberTypes) { return typeRepository.GetStructureType(memberTypes); }
    Type* CreateStructureType() { return typeRepository.CreateStructureType(); }
    Type* GetArrayType(Type* elementType, uint64_t size) { return typeRepository.GetArrayType(elementType, size); }
    Type* GetFunctionType(Type* returnType, const std::vector<Type*>& paramTypes) { return typeRepository.GetFunctionType(returnType, paramTypes); }
    ConstantValue* GetDefaultBoolValue() { return GetBoolType()->DefaultValue(); }
    ConstantValue* GetDefaultSByteValue() { return GetSByteType()->DefaultValue(); }
    ConstantValue* GetDefaultByteValue() { return GetByteType()->DefaultValue(); }
    ConstantValue* GetDefaultShortValue() { return GetShortType()->DefaultValue(); }
    ConstantValue* GetDefaultUShortValue() { return GetUShortType()->DefaultValue(); }
    ConstantValue* GetDefaultIntValue() { return GetIntType()->DefaultValue(); }
    ConstantValue* GetDefaultUIntValue() { return GetUIntType()->DefaultValue(); }
    ConstantValue* GetDefaultLongValue() { return GetLongType()->DefaultValue(); }
    ConstantValue* GetDefaultULongValue() { return GetULongType()->DefaultValue(); }
    ConstantValue* GetDefaultFloatValue() { return GetFloatType()->DefaultValue(); }
    ConstantValue* GetDefaultDoubleValue() { return GetDoubleType()->DefaultValue(); }
    ConstantValue* GetDefaultCharValue() { return GetCharType()->DefaultValue(); }
    ConstantValue* GetDefaultWCharValue() { return GetWCharType()->DefaultValue(); }
    ConstantValue* GetDefaultUCharValue() { return GetUCharType()->DefaultValue(); }
    ConstantValue* GetNullValue(PtrType* ptrType) { return ptrType->DefaultValue(); }
    ConstantValue* GetBoolValue(bool value);
    ConstantValue* GetSByteValue(int8_t value);
    ConstantValue* GetByteValue(uint8_t value);
    ConstantValue* GetShortValue(int16_t value);
    ConstantValue* GetUShortValue(uint16_t value);
    ConstantValue* GetIntValue(int32_t value);
    ConstantValue* GetUIntValue(uint32_t value);
    ConstantValue* GetLongValue(int64_t value);
    ConstantValue* GetULongValue(uint64_t value);
    ConstantValue* GetFloatValue(float value);
    ConstantValue* GetDoubleValue(double value);
    ConstantValue* GetCharValue(char8_t value);
    ConstantValue* GetWCharValue(char16_t value);
    ConstantValue* GetUCharValue(char32_t value);
    ArrayValue* GetArrayValue(Type* arrayType, const std::vector<Value*>& elements, const std::string& prefix);
    StructureValue* GetStructureValue(Type* structureType, const std::vector<Value*>& members);
    StringValue* GetStringValue(Type* stringType, const std::string& value);
    WStringValue* GetWStringValue(Type* stringType, const std::u16string& value);
    UStringValue* GetUStringValue(Type* stringType, const std::u32string& value);
    ConversionValue* GetConversionValue(Type* type, ConstantValue* from);
    ClsIdValue* GetClsIdValue(const std::string& typeId);
    void AddValue(Value* value);
    void SetCurrentBasicBlock(BasicBlock* bb);
    BasicBlock* GetCurrentBasicBlock() const { return currentBasicBlock; }
    void SetCurrentFunction(Function* fun) { currentFunction = fun; }
    Function* CurrentFunction() const { return currentFunction; }
    Instruction* CreateNot(Value* arg);
    Instruction* CreateNeg(Value* arg);
    Instruction* CreateAdd(Value* left, Value* right);
    Instruction* CreateSub(Value* left, Value* right);
    Instruction* CreateMul(Value* left, Value* right);
    Instruction* CreateDiv(Value* left, Value* right);
    Instruction* CreateMod(Value* left, Value* right);
    Instruction* CreateAnd(Value* left, Value* right);
    Instruction* CreateOr(Value* left, Value* right);
    Instruction* CreateXor(Value* left, Value* right);
    Instruction* CreateShl(Value* left, Value* right);
    Instruction* CreateShr(Value* left, Value* right);
    Instruction* CreateEqual(Value* left, Value* right);
    Instruction* CreateLess(Value* left, Value* right);
    Instruction* CreateSignExtend(Value* arg, Type* destType);
    Instruction* CreateZeroExtend(Value* arg, Type* destType);
    Instruction* CreateTruncate(Value* arg, Type* destType);
    Instruction* CreateBitCast(Value* arg, Type* destType);
    Instruction* CreateIntToFloat(Value* arg, Type* destType);
    Instruction* CreateFloatToInt(Value* arg, Type* destType);
    Instruction* CreateIntToPtr(Value* arg, Type* destType);
    Instruction* CreatePtrToInt(Value* arg, Type* destType);
    Instruction* CreateLocal(Type* type);
    Instruction* CreateLoad(Value* ptr);
    Instruction* CreateStore(Value* value, Value* ptr);
    Instruction* CreateArg(Value* arg);
    Instruction* CreateElemAddr(Value* ptr, Value* index);
    Instruction* CreatePtrOffset(Value* ptr, Value* offset);
    Instruction* CreatePtrDiff(Value* leftPtr, Value* rightPtr);
    Instruction* CreateCall(Value* function, const std::vector<Value*>& args);
    Instruction* CreateInvoke(Value* function, const std::vector<Value*> args, BasicBlock* normalBlockNext, BasicBlock* unwindBlockNext);
    Instruction* CreateRet(Value* value);
    Instruction* CreateJump(BasicBlock* dest);
    Instruction* CreateBranch(Value* cond, BasicBlock* trueDest, BasicBlock* falseDest);
    Instruction* CreateSwitch(Value* cond, BasicBlock* defaultDest);
    Instruction* CreateNop();
    Instruction* CreateBeginTry();
    Instruction* CreateEndTry(BasicBlock* nextDest);
    Instruction* CreateBeginCatch();
    Instruction* CreateEndCatch(BasicBlock* nextDest);
    Instruction* CreateResume();
    Instruction* CreateIncludeBasicBlockInstruction(BasicBlock* block);
    GlobalVariable* GetOrInsertGlobal(const std::string& name, Type* type);
    GlobalVariable* CreateGlobalStringPtr(const std::string& stringValue);
    GlobalVariable* CreateGlobalWStringPtr(const std::u16string& stringValue);
    GlobalVariable* CreateGlobalUStringPtr(const std::u32string& stringValue);
    void SetCurrentSourceSpan(const cmajor::debug::SourceSpan& span);
    void BeginScope();
    void EndScope();
    int16_t CurrentScopeId() const { return currentScopeId; }
    void SetCurrentScopeId(int16_t scopeId) { currentScopeId = scopeId; }
    void BeginInstructionFlag(int16_t instructionFlag);
    void EndInstructionFlag(int16_t instructionFlag);
    void AddLineInfoScopeIdAndFlags(Instruction* inst);
    void SetCompileUnitId(const std::string& compileUnitId);
    const std::string& CompileUnitId() const { return dataRepository.CompileUnitId(); }
    void PushParent();
    void PopParent();
    void SetHandlerBlock(BasicBlock* tryBlock, BasicBlock* catchBlock);
    void SetCleanupBlock(BasicBlock* cleanupBlock);
    BasicBlock* CurrentParent() const { return currentParentBlock; }
    const cmajor::debug::SourceSpan& GetSourceSpan() const { return sourceSpan; }
    void SetSourceSpan(const cmajor::debug::SourceSpan& sourceSpan_) { sourceSpan = sourceSpan_; }
    int32_t CppLineIndex() const { return cppLineIndex; }
    void SetCppLineIndex(int32_t cppLineIndex_) { cppLineIndex = cppLineIndex_; }
    void AddLocalVariable(const std::string& name, const boost::uuids::uuid& typeId, LocalInstruction* inst);
    int32_t AddControlFlowGraphNode();
    void SetCurrentFlowGraphNodeId(int32_t controlFlowGraphNodeId);
    void AddControlFlowGraphEdge(int32_t startNodeId, int32_t endNodeId);
private:
    TypeRepository typeRepository;
    DataRepository dataRepository;
    std::vector<std::unique_ptr<Value>> values;
    Function* currentFunction;
    BasicBlock* currentBasicBlock;
    BasicBlock* currentParentBlock;
    int16_t currentScopeId;
    int16_t currentInstructionFlags;
    int32_t currentControlFlowGraphNodeId;
    std::stack<BasicBlock*> blockStack;
    cmajor::debug::SourceSpan currentSourceSpan;
    cmajor::debug::SourceSpan sourceSpan;
    int32_t cppLineIndex;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_CONTEXT_INCLUDED
