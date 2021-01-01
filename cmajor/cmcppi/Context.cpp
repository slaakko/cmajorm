// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Context.hpp>
#include <cmajor/cmcppi/Instruction.hpp>
#include <cmajor/cmcppi/BasicBlock.hpp>
#include <cmajor/cmcppi/Function.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <cmajor/cmdebug/DIVariable.hpp>

namespace cmcppi {

Context::Context() :
    currentSourceSpan(), currentFunction(nullptr), currentBasicBlock(nullptr), currentParentBlock(nullptr), sourceSpan(), cppLineIndex(0),
    currentScopeId(-1), currentInstructionFlags(0), currentControlFlowGraphNodeId(-1), typeRepository(*this)
{
}

void Context::AddValue(Value* value)
{
    values.push_back(std::unique_ptr<Value>(value));
}

void Context::SetCurrentBasicBlock(BasicBlock* bb)
{
    currentBasicBlock = bb;
}

ConstantValue* Context::GetBoolValue(bool value)
{
    ConstantValue* boolValue = new BoolValue(value);
    AddValue(boolValue);
    return boolValue;
}

ConstantValue* Context::GetSByteValue(int8_t value)
{
    ConstantValue* sbyteValue = new SByteValue(value);
    AddValue(sbyteValue);
    return sbyteValue;
}

ConstantValue* Context::GetByteValue(uint8_t value)
{
    ConstantValue* byteValue = new ByteValue(value);
    AddValue(byteValue);
    return byteValue;
}

ConstantValue* Context::GetShortValue(int16_t value)
{
    ConstantValue* shortValue = new ShortValue(value);
    AddValue(shortValue);
    return shortValue;
}

ConstantValue* Context::GetUShortValue(uint16_t value)
{
    ConstantValue* ushortValue = new UShortValue(value);
    AddValue(ushortValue);
    return ushortValue;
}

ConstantValue* Context::GetIntValue(int32_t value)
{
    ConstantValue* intValue = new IntValue(value);
    AddValue(intValue);
    return intValue;
}

ConstantValue* Context::GetUIntValue(uint32_t value)
{
    ConstantValue* uintValue = new UIntValue(value);
    AddValue(uintValue);
    return uintValue;
}

ConstantValue* Context::GetLongValue(int64_t value)
{
    ConstantValue* longValue = new LongValue(value);
    AddValue(longValue);
    return longValue;
}

ConstantValue* Context::GetULongValue(uint64_t value)
{
    ConstantValue* ulongValue = new ULongValue(value);
    AddValue(ulongValue);
    return ulongValue;
}

ConstantValue* Context::GetFloatValue(float value)
{
    ConstantValue* floatValue = new FloatValue(value);
    AddValue(floatValue);
    return floatValue;
}

ConstantValue* Context::GetDoubleValue(double value)
{
    ConstantValue* doubleValue = new DoubleValue(value);
    AddValue(doubleValue);
    return doubleValue;
}

ConstantValue* Context::GetCharValue(char8_t value)
{
    ConstantValue* charValue = new CharValue(value);
    AddValue(charValue);
    return charValue;
}

ConstantValue* Context::GetWCharValue(char16_t value)
{
    ConstantValue* wcharValue = new WCharValue(value);
    AddValue(wcharValue);
    return wcharValue;
}

ConstantValue* Context::GetUCharValue(char32_t value)
{
    ConstantValue* ucharValue = new UCharValue(value);
    AddValue(ucharValue);
    return ucharValue;
}

ArrayValue* Context::GetArrayValue(Type* arrayType, const std::vector<Value*>& elements, const std::string& prefix)
{
    ArrayValue* arrayValue = new ArrayValue(arrayType, elements, prefix);
    AddValue(arrayValue);
    return arrayValue;
}

StructureValue* Context::GetStructureValue(Type* structureType, const std::vector<Value*>& members)
{
    StructureValue* structureValue = new StructureValue(structureType, members);
    AddValue(structureValue);
    return structureValue;
}

StringValue* Context::GetStringValue(Type* stringType, const std::string& value)
{
    StringValue* stringValue = new StringValue(stringType, value);
    AddValue(stringValue);
    return stringValue;
}

WStringValue* Context::GetWStringValue(Type* stringType, const std::u16string& value)
{
    WStringValue* stringValue = new WStringValue(stringType, value);
    AddValue(stringValue);
    return stringValue;
}

UStringValue* Context::GetUStringValue(Type* stringType, const std::u32string& value)
{
    UStringValue* stringValue = new UStringValue(stringType, value);
    AddValue(stringValue);
    return stringValue;
}

ConversionValue* Context::GetConversionValue(Type* type, ConstantValue* from)
{
    ConversionValue* conversionValue = new ConversionValue(type, from);
    AddValue(conversionValue);
    return conversionValue;
}

ClsIdValue* Context::GetClsIdValue(const std::string& typeId)
{
    ClsIdValue* clsIdValue = new ClsIdValue(typeId);
    AddValue(clsIdValue);
    return clsIdValue;
}

Instruction* Context::CreateNot(Value* arg)
{
    Instruction* inst = new NotInstruction(arg);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateNeg(Value* arg)
{
    Instruction* inst = new NegInstruction(arg);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateAdd(Value* left, Value* right)
{
    Instruction* inst = new AddInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateSub(Value* left, Value* right)
{
    Instruction* inst = new SubInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateMul(Value* left, Value* right)
{
    Instruction* inst = new MulInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateDiv(Value* left, Value* right)
{
    Instruction* inst = new DivInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateMod(Value* left, Value* right)
{
    Instruction* inst = new ModInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateAnd(Value* left, Value* right)
{
    Instruction* inst = new AndInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateOr(Value* left, Value* right)
{
    Instruction* inst = new OrInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateXor(Value* left, Value* right)
{
    Instruction* inst = new XorInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateShl(Value* left, Value* right)
{
    Instruction* inst = new ShlInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateShr(Value* left, Value* right)
{
    Instruction* inst = new ShrInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateEqual(Value* left, Value* right)
{
    Instruction* inst = new EqualInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateLess(Value* left, Value* right)
{
    Instruction* inst = new LessInstruction(left, right);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateSignExtend(Value* arg, Type* destType)
{
    Instruction* inst = new SignExtendInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateZeroExtend(Value* arg, Type* destType)
{
    Instruction* inst = new ZeroExtendInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateTruncate(Value* arg, Type* destType)
{
    Instruction* inst = new TruncateInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateBitCast(Value* arg, Type* destType)
{
    Instruction* inst = new BitCastInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateIntToFloat(Value* arg, Type* destType)
{
    Instruction* inst = new IntToFloatInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateFloatToInt(Value* arg, Type* destType)
{
    Instruction* inst = new FloatToIntInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateIntToPtr(Value* arg, Type* destType)
{
    Instruction* inst = new IntToPtrInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreatePtrToInt(Value* arg, Type* destType)
{
    Instruction* inst = new PtrToIntInstruction(arg, destType);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateLocal(Type* type)
{
    LocalInstruction* inst = new LocalInstruction(type);
    inst->ObtainLocalName(*currentFunction);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateLoad(Value* ptr)
{
    Instruction* inst = new LoadInstruction(ptr);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateStore(Value* value, Value* ptr)
{
    Instruction* inst = new StoreInstruction(value, ptr);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateArg(Value* arg)
{
    Instruction* inst = new ArgInstruction(arg);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateElemAddr(Value* ptr, Value* index)
{
    Instruction* inst = new ElemAddrInstruction(ptr, index);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreatePtrOffset(Value* ptr, Value* offset)
{
    Instruction* inst = new PtrOffsetInstruction(ptr, offset);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreatePtrDiff(Value* leftPtr, Value* rightPtr)
{
    Instruction* inst = new PtrDiffInstruction(leftPtr, rightPtr);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateCall(Value* function, const std::vector<Value*>& args)
{
    Instruction* inst = new CallInstruction(function, args);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateInvoke(Value* function, const std::vector<Value*> args, BasicBlock* normalBlockNext, BasicBlock* unwindBlockNext)
{
    Instruction* inst = new InvokeInstruction(function, args, normalBlockNext, unwindBlockNext);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateRet(Value* value)
{
    Instruction* inst = new RetInstruction(value);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateJump(BasicBlock* dest)
{
    Instruction* inst = new JumpInstruction(dest);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateBranch(Value* cond, BasicBlock* trueDest, BasicBlock* falseDest)
{
    Instruction* inst = new BranchInstruction(cond, trueDest, falseDest);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateSwitch(Value* cond, BasicBlock* defaultDest)
{
    Instruction* inst = new SwitchInstruction(cond, defaultDest);
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateNop()
{
    Instruction* inst = new NoOperationInstruction();
    AddLineInfoScopeIdAndFlags(inst);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateBeginTry()
{
    Instruction* inst = new BeginTryInstruction();
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateEndTry(BasicBlock* nextDest)
{
    Instruction* inst = new EndTryInstruction(nextDest);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateBeginCatch()
{
    Instruction* inst = new BeginCatchInstruction();
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}


Instruction* Context::CreateEndCatch(BasicBlock* nextDest)
{
    Instruction* inst = new EndCatchInstruction(nextDest);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateResume()
{
    Instruction* inst = new ResumeInstruction();
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

Instruction* Context::CreateIncludeBasicBlockInstruction(BasicBlock* block)
{
    Instruction* inst = new IncludeBasicBlockInstruction(block);
    currentBasicBlock->AddInstruction(inst);
    if (inst->IsResultInstruction(*this))
    {
        currentFunction->AddResultInstruction(inst);
    }
    return inst;
}

GlobalVariable* Context::GetOrInsertGlobal(const std::string& name, Type* type)
{
    return dataRepository.GetOrInsertGlobal(name, type);
}

GlobalVariable* Context::CreateGlobalStringPtr(const std::string& stringValue)
{
    return dataRepository.CreateGlobalStringPtr(*this, stringValue);
}

GlobalVariable* Context::CreateGlobalWStringPtr(const std::u16string& stringValue)
{
    return dataRepository.CreateGlobalWStringPtr(*this, stringValue);
}

GlobalVariable* Context::CreateGlobalUStringPtr(const std::u32string& stringValue)
{
    return dataRepository.CreateGlobalUStringPtr(*this, stringValue);
}

void Context::SetCurrentSourceSpan(const cmajor::debug::SourceSpan& span)
{
    if (span.line != 0)
    {
        currentSourceSpan = span;
    }
}

void Context::BeginScope()
{
    int16_t scopeId = currentFunction->Scopes().size();
    std::unique_ptr<Scope> scope(new Scope(scopeId, currentScopeId));
    currentFunction->AddScope(scope.release());
    currentScopeId = scopeId;
}

void Context::EndScope()
{
    const std::unique_ptr<Scope>& currentScope = currentFunction->Scopes()[currentScopeId];
    currentScopeId = currentScope->ParentScopeId();
}

void Context::BeginInstructionFlag(int16_t instructionFlag)
{
    currentInstructionFlags = currentInstructionFlags | instructionFlag;
}

void Context::EndInstructionFlag(int16_t instructionFlag)
{
    currentInstructionFlags = currentInstructionFlags & ~instructionFlag;
}

void Context::AddLineInfoScopeIdAndFlags(Instruction* inst)
{
    if (currentControlFlowGraphNodeId != -1)
    {
        ControlFlowGraphNode* node = currentFunction->GetControlFlowGraphNode(currentControlFlowGraphNodeId);
        node->SetInstruction(inst);
        currentControlFlowGraphNodeId = -1;
    }
    inst->SetSourceSpan(currentSourceSpan);
    inst->SetScopeId(currentScopeId);
    inst->SetFlags(currentInstructionFlags);
}

void Context::SetCompileUnitId(const std::string& compileUnitId)
{
    dataRepository.SetCompileUnitId(compileUnitId);
}

void Context::PushParent()
{
    blockStack.push(currentParentBlock);
    currentParentBlock = currentBasicBlock;
}

void Context::PopParent()
{
    currentParentBlock = blockStack.top();
    blockStack.pop();
}

void Context::SetHandlerBlock(BasicBlock* tryBlock, BasicBlock* catchBlock)
{
    tryBlock->SetHandlerBlock(catchBlock);
}

void Context::SetCleanupBlock(BasicBlock* cleanupBlock)
{
    cleanupBlock->SetAsCleanupBlock();
    currentBasicBlock->SetCleanupBlock(cleanupBlock);
}

void Context::AddLocalVariable(const std::string& name, const boost::uuids::uuid& typeId, LocalInstruction* inst)
{
    Scope* scope = currentFunction->GetScope(currentScopeId);
    cmajor::debug::DIVariable* localVariable = new cmajor::debug::DIVariable(cmajor::debug::DIVariable::Kind::localVariable);
    localVariable->SetInitLineNumber(currentSourceSpan.line);
    localVariable->SetName(name);
    inst->ObtainLocalName(*currentFunction);
    localVariable->SetIrName(inst->LocalName());
    localVariable->SetTypeId(typeId);
    scope->AddLocalVariable(localVariable);
}

int32_t Context::AddControlFlowGraphNode()
{
    currentControlFlowGraphNodeId = currentFunction->AddControlFlowGraphNode();
    return currentControlFlowGraphNodeId;
}

void Context::SetCurrentFlowGraphNodeId(int32_t controlFlowGraphNodeId)
{
    currentControlFlowGraphNodeId = controlFlowGraphNodeId;
}

void Context::AddControlFlowGraphEdge(int32_t startNodeId, int32_t endNodeId)
{
    currentFunction->AddControlFlowGraphEdge(startNodeId, endNodeId);
}

} // namespace cmcppi
