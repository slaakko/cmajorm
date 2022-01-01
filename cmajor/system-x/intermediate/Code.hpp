// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_CODE_INCLUDED
#define CMSX_INTERMEDIATE_CODE_INCLUDED
#include <system-x/intermediate/Data.hpp>
#include <system-x/intermediate/Metadata.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Container.hpp>
#include <soulng/util/Component.hpp>

namespace cmsx::intermediate {

class Instruction;
class BasicBlock;
class Function;
class Code;

class CMSX_INTERMEDIATE_API RegValue : public Value
{
public:
    RegValue(const SourcePos& sourcePos_, Type* type_, int32_t reg_);
    int32_t Reg() const { return reg; }
    void SetInst(Instruction* inst_) { inst = inst_; }
    Instruction* Inst() const { return inst; }
private:
    int32_t reg;
    Instruction* inst;
};

enum class OpCode : int
{
    store, arg, jmp, branch, procedure_call, ret, switch_, 
    not_, neg, signextend, zeroextend, truncate, bitcast, inttofloat, floattoint, inttoptr, ptrtoint,
    add, sub, mul, div_, mod, and_, or_, xor_, shl, shr, equal, less,
    param, local, load, elemaddr, ptroffset, ptrdiff, function_call, trap, phi,
    nop
};

class CMSX_INTERMEDIATE_API Use
{
public:
    Use(Instruction* user_, Value*& value_) : user(user_), value(value_) {}
    Instruction* User() const { return user; }
    Value* Get() const { return value; }
    void Set(Value* value_);
private:
    Instruction* user;
    Value*& value;
};

class CMSX_INTERMEDIATE_API Instruction : public Value, public soulng::util::Component
{
public:
    Instruction(const SourcePos& sourcePos_, Type* type_, OpCode opCode_);
    std::string Name() const;
    virtual void Accept(Visitor& visitor) = 0;
    BasicBlock* Parent() const;
    Instruction* Next() { return static_cast<Instruction*>(NextSibling()); }
    Instruction* Prev() { return static_cast<Instruction*>(PrevSibling()); }
    OpCode GetOpCode() const { return opCode; }
    void SetMetadataRef(MetadataRef* metadataRef_) { metadataRef = metadataRef_; }
    MetadataRef* GetMetadataRef() const { return metadataRef; }
    bool IsLeader() const;
    bool IsTerminator() const;
    bool IsValueInstruction() const;
    bool IsUnaryInstruction() const;
    bool IsBinaryInstruction() const;
    bool IsSwitchInstruction() const { return opCode == OpCode::switch_; }
    bool IsParamInstruction() const { return opCode == OpCode::param; }
    bool IsLocalInstruction() const { return opCode == OpCode::local; }
    bool IsLoadInstruction() const { return opCode == OpCode::load;  }
    bool IsStoreInstruction() const { return opCode == OpCode::store; }
    bool IsElemAddrInstruction() const { return opCode == OpCode::elemaddr; }
    bool IsPhiInstruction() const { return opCode == OpCode::phi; }
    bool RequiresLocalRegister() const;
    std::vector<BasicBlock*> Successors() const;
    const std::vector<Instruction*>& Users() const { return users; }
    void AddUser(Instruction* user);
    void RemoveUser(Instruction* user);
    std::vector<Use> Uses();
    std::unique_ptr<Instruction> Remove();
    void ReplaceUsesWith(Value* value);
    int Index() const { return index; }
    void SetIndex(int index_) { index = index_; }
private:
    OpCode opCode;
    MetadataRef* metadataRef;
    std::vector<Instruction*> users;
    int index;
};

class CMSX_INTERMEDIATE_API StoreInstruction : public Instruction
{
public:
    StoreInstruction(const SourcePos& sourcePos_, Value* value_, Value* ptr_);
    void Accept(Visitor& visitor) override;
    Value* GetValue() const { return value; }
    Value* GetPtr() const { return ptr; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* value;
    Value* ptr;
};

class CMSX_INTERMEDIATE_API ArgInstruction : public Instruction
{
public:
    ArgInstruction(const SourcePos& sourcePos_, Value* arg_);
    void Accept(Visitor& visitor) override;
    Value* Arg() const { return arg; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* arg;
};

class CMSX_INTERMEDIATE_API JmpInstruction : public Instruction
{
public:
    JmpInstruction(const SourcePos& sourcePos_, int32_t targetLabelId_);
    void Accept(Visitor& visitor) override;
    int32_t TargetLabelId() const { return targetLabelId; }
    BasicBlock* TargetBasicBlock() const { return targetBasicBlock; }
    void SetTargetBasicBlock(BasicBlock* targetBasicBlock_) { targetBasicBlock = targetBasicBlock_; }
private:
    int32_t targetLabelId;
    BasicBlock* targetBasicBlock;
};

class CMSX_INTERMEDIATE_API BranchInstruction : public Instruction
{
public:
    BranchInstruction(const SourcePos& sourcePos_, Value* cond_, int32_t trueTargetLabelId_, int32_t falseTargetLabelId_);
    void Accept(Visitor& visitor) override;
    Value* Cond() const { return cond; }
    int32_t TrueTargetLabelId() const { return trueTargetLabelId; }
    BasicBlock* TrueTargetBasicBlock() const { return trueTargetBasicBlock; }
    void SetTrueTargetBasicBlock(BasicBlock* trueTargetBasicBlock_) { trueTargetBasicBlock = trueTargetBasicBlock_; }
    int32_t FalseTargetLabelId() const { return falseTargetLabelId; }
    BasicBlock* FalseTargetBasicBlock() const { return falseTargetBasicBlock; }
    void SetFalseTargetBasicBlock(BasicBlock* falseTargetBasicBlock_) { falseTargetBasicBlock = falseTargetBasicBlock_; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* cond;
    int32_t trueTargetLabelId;
    BasicBlock* trueTargetBasicBlock;
    int32_t falseTargetLabelId;
    BasicBlock* falseTargetBasicBlock;
};

class CMSX_INTERMEDIATE_API ProcedureCallInstruction : public Instruction
{
public:
    ProcedureCallInstruction(const SourcePos& sourcePos_, Value* callee_);
    void Accept(Visitor& visitor) override;
    Value* Callee() const { return callee; }
    const std::vector<Value*>& Args() const { return args; }
    void SetArgs(std::vector<Value*>&& args_);
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* callee;
    std::vector<Value*> args;
};

class CMSX_INTERMEDIATE_API RetInstruction : public Instruction
{
public:
    RetInstruction(const SourcePos& sourcePos_, Value* returnValue_);
    void Accept(Visitor& visitor) override;
    Value* ReturnValue() const { return returnValue; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* returnValue;
};

struct CMSX_INTERMEDIATE_API CaseTarget
{
    CaseTarget(Value* caseValue_, int32_t targetLabelId_) : caseValue(caseValue_), targetLabelId(targetLabelId_), targetBlock(nullptr) {}
    Value* caseValue;
    int32_t targetLabelId;
    BasicBlock* targetBlock;
};

class CMSX_INTERMEDIATE_API SwitchInstruction : public Instruction
{
public:
    SwitchInstruction(const SourcePos& sourcePos_, Value* cond_, int32_t defaultTargetLabelId_);
    void Accept(Visitor& visitor) override;
    Value* Cond() const { return cond; }
    int32_t DefaultTargetLabelId() const { return defaultTargetLabelId; }
    void AddCaseTarget(const CaseTarget& caseTarget);
    const std::vector<CaseTarget>& CaseTargets() const { return caseTargets; }
    std::vector<CaseTarget>& CaseTargets() { return caseTargets; }
    BasicBlock* DefaultTargetBlock() const { return defaultTargetBlock; }
    void SetDefaultTargetBlock(BasicBlock* defaultTargetBlock_) { defaultTargetBlock = defaultTargetBlock_; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* cond;
    int32_t defaultTargetLabelId;
    BasicBlock* defaultTargetBlock;
    std::vector<CaseTarget> caseTargets;
};

class CMSX_INTERMEDIATE_API ValueInstruction : public Instruction
{
public:
    ValueInstruction(const SourcePos& sourcePos_, RegValue* result_, OpCode opCode_);
    RegValue* Result() const { return result; }
private:
    RegValue* result;
};

class CMSX_INTERMEDIATE_API UnaryInstruction : public ValueInstruction
{
public:
    UnaryInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_, OpCode opCode_);
    Value* Operand() const { return operand; }
    void SetOperand(Value* operand_) { operand = operand_; }
    void AddUse();
    void AddOperandToUses(std::vector<Use>& uses);
private:
    Value* operand;
};

class CMSX_INTERMEDIATE_API NotInstruction : public UnaryInstruction
{
public:
    NotInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API NegInstruction : public UnaryInstruction
{
public:
    NegInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API SignExtendInstruction : public UnaryInstruction
{
public:
    SignExtendInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API ZeroExtendInstruction : public UnaryInstruction
{
public:
    ZeroExtendInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API TruncateInstruction : public UnaryInstruction
{
public:
    TruncateInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API BitcastInstruction : public UnaryInstruction
{
public:
    BitcastInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API IntToFloatInstruction : public UnaryInstruction
{
public:
    IntToFloatInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API FloatToIntInstruction : public UnaryInstruction
{
public:
    FloatToIntInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API IntToPtrInstruction : public UnaryInstruction
{
public:
    IntToPtrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API PtrToIntInstruction : public UnaryInstruction
{
public:
    PtrToIntInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API BinaryInstruction : public ValueInstruction
{
public:
    BinaryInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_, OpCode opCode_);
    Value* Left() const { return left; }
    Value* Right() const { return right; }
    void AddUse();
    void AddOperandsToUses(std::vector<Use>& uses);
private:
    Value* left;
    Value* right;
};

class CMSX_INTERMEDIATE_API AddInstruction : public BinaryInstruction
{
public:
    AddInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API SubInstruction : public BinaryInstruction
{
public:
    SubInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API MulInstruction : public BinaryInstruction
{
public:
    MulInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API DivInstruction : public BinaryInstruction
{
public:
    DivInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API ModInstruction : public BinaryInstruction
{
public:
    ModInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API AndInstruction : public BinaryInstruction
{
public:
    AndInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API OrInstruction : public BinaryInstruction
{
public:
    OrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API XorInstruction : public BinaryInstruction
{
public:
    XorInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API ShlInstruction : public BinaryInstruction
{
public:
    ShlInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API ShrInstruction : public BinaryInstruction
{
public:
    ShrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API EqualInstruction : public BinaryInstruction
{
public:
    EqualInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API LessInstruction : public BinaryInstruction
{
public:
    LessInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API ParamInstruction : public ValueInstruction
{
public:
    ParamInstruction(const SourcePos& sourcePos_, RegValue* result_);
    void Accept(Visitor& visitor) override;
};

class CMSX_INTERMEDIATE_API LocalInstruction : public ValueInstruction
{
public:
    LocalInstruction(const SourcePos& sourcePos_, RegValue* result_, Type* localType_);
    void Accept(Visitor& visitor) override;
    Type* LocalType() const { return localType; }
private:
    Type* localType;
};

class CMSX_INTERMEDIATE_API LoadInstruction : public ValueInstruction
{
public:
    LoadInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* ptr_);
    void Accept(Visitor& visitor) override;
    Value* Ptr() const { return ptr; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* ptr;
};

enum class ElemAddrKind
{
    none, array, structure
};

class CMSX_INTERMEDIATE_API ElemAddrInstruction : public ValueInstruction
{
public:
    ElemAddrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* ptr_, Value* index_);
    void Accept(Visitor& visitor) override;
    Value* Ptr() const { return ptr; }
    Value* Index() const { return index; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
    ElemAddrKind GetElemAddrKind(Context* context) const;
private:
    Value* ptr;
    Value* index;
};

class CMSX_INTERMEDIATE_API PtrOffsetInstruction : public ValueInstruction
{
public:
    PtrOffsetInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* ptr_, Value* offset_);
    void Accept(Visitor& visitor) override;
    Value* Ptr() const { return ptr; }
    Value* Offset() const { return offset; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* ptr;
    Value* offset;
};

class CMSX_INTERMEDIATE_API PtrDiffInstruction : public ValueInstruction
{
public:
    PtrDiffInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* leftPtr_, Value* rightPtr_);
    void Accept(Visitor& visitor) override;
    Value* LeftPtr() const { return leftPtr; }
    Value* RightPtr() const { return rightPtr; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* leftPtr;
    Value* rightPtr;
};

class CMSX_INTERMEDIATE_API FunctionCallInstruction : public ValueInstruction
{
public:
    FunctionCallInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* callee_);
    void Accept(Visitor& visitor) override;
    Value* Callee() const { return callee; }
    const std::vector<Value*>& Args() const { return args; }
    void SetArgs(std::vector<Value*>&& args_);
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* callee;
    std::vector<Value*> args;
};

class CMSX_INTERMEDIATE_API TrapInstruction : public ValueInstruction
{
public:
    TrapInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* op1_, Value* op2_, Value* op3_);
    void Accept(Visitor& visitor) override;
    Value* Op1() const { return op1; }
    Value* Op2() const { return op2; }
    Value* Op3() const { return op3; }
    const std::vector<Value*>& Args() const { return args; }
    void SetArgs(std::vector<Value*>&& args_);
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    Value* op1;
    Value* op2;
    Value* op3;
    std::vector<Value*> args;
};

struct CMSX_INTERMEDIATE_API BlockValue
{
    BlockValue(Value* value_, int32_t blockId_) : value(value_), blockId(blockId_), block(nullptr) {}
    BlockValue(Value* value_, BasicBlock* block_);
    Value* value;
    int32_t blockId;
    BasicBlock* block;
};

class CMSX_INTERMEDIATE_API PhiInstruction : public ValueInstruction
{
public:
    PhiInstruction(const SourcePos& sourcePos_, RegValue* result_);
    void Accept(Visitor& visitor) override;
    void AddBlockValue(const BlockValue& blockValue);
    const std::vector<BlockValue>& BlockValues() const { return blockValues; }
    std::vector<BlockValue>& BlockValues() { return blockValues; }
    void AddUse();
    void AddToUses(std::vector<Use>& uses);
private:
    std::vector<BlockValue> blockValues;
};

class CMSX_INTERMEDIATE_API NoOperationInstruction : public Instruction
{
public:
    NoOperationInstruction(const SourcePos& sourcePos_);
    void Accept(Visitor& visitor) override;
};

const int32_t entryBlockId = -1;
const int32_t exitBlockId = -2;

class CMSX_INTERMEDIATE_API BasicBlock : public soulng::util::Component
{
public:
    BasicBlock(const SourcePos& sourcePos_, int32_t id_);
    void Accept(Visitor& visitor);
    void VisitInstructions(Visitor& visitor);
    const SourcePos& GetSourcePos() const { return sourcePos; }
    std::string Name() const;
    int32_t Id() const { return id; }
    Function* Parent() const;
    BasicBlock* Next() { return static_cast<BasicBlock*>(NextSibling()); }
    BasicBlock* Prev() { return static_cast<BasicBlock*>(PrevSibling()); }
    Instruction* FirstInstruction() { return static_cast<Instruction*>(instructions.FirstChild()); }
    Instruction* LastInstruction() { return static_cast<Instruction*>(instructions.LastChild()); }
    bool HasPhiInstructions() { return FirstInstruction()->IsPhiInstruction(); }
    void AddInstruction(Instruction* instruction, MetadataRef* metadataRef);
    void InsertFront(Instruction* instruction);
    bool IsEmpty() const { return instructions.IsEmpty(); }
    bool IsEntryBlock() const { return id == entryBlockId; }
    bool IsExitBlock() const { return id == exitBlockId; }
    const std::vector<BasicBlock*>& Successors() const { return successors; }
    void AddSuccessor(BasicBlock* successor);
    bool RemoveSuccessor(BasicBlock* successor);
    const std::vector<BasicBlock*>& Predecessors() const { return predecessors; }
    void AddPredecessor(BasicBlock* predecessor);
    bool RemovePredecessor(BasicBlock* predecessor);
    void ClearSuccessorsAndPredecessors();
    int IndexOf(Instruction* x);
private:
    SourcePos sourcePos;
    int32_t id;
    soulng::util::Container instructions;
    std::vector<BasicBlock*> successors;
    std::vector<BasicBlock*> predecessors;
};

enum class FunctionFlags : int
{
    none = 0, defined = 1 << 0, once = 1 << 1
};

CMSX_INTERMEDIATE_API inline FunctionFlags operator|(FunctionFlags left, FunctionFlags right)
{
    return FunctionFlags(static_cast<int>(left) | static_cast<int>(right));
}

CMSX_INTERMEDIATE_API inline FunctionFlags operator&(FunctionFlags left, FunctionFlags right)
{
    return FunctionFlags(static_cast<int>(left) & static_cast<int>(right));
}

CMSX_INTERMEDIATE_API inline FunctionFlags operator~(FunctionFlags flags)
{
    return FunctionFlags(~static_cast<int>(flags));
}

class CMSX_INTERMEDIATE_API Function : public soulng::util::Component
{
public:
    Function(const SourcePos& sourcePos_, FunctionType* functionType_, const std::string& name_, bool once_, bool definition_, MetadataRef* metadataRef_);
    Function(const Function&) = delete;
    Function& operator=(const Function&) = delete;
    bool GetFlag(FunctionFlags flag) const { return (flags & flag) != FunctionFlags::none; }
    void SetFlag(FunctionFlags flag) { flags = flags | flag; }
    void ResetFlag(FunctionFlags flag) { flags = flags & ~flag; }
    bool IsDefined() const { return GetFlag(FunctionFlags::defined); }
    bool IsExternal() const { return !IsDefined(); }
    void SetDefined() { SetFlag(FunctionFlags::defined); }
    bool Once() const { return GetFlag(FunctionFlags::once); }
    int Arity() const { return type->Arity(); }
    void Accept(Visitor& visitor);
    void VisitBasicBlocks(Visitor& visitor);
    Code* Parent() const;
    Function* Next() { return static_cast<Function*>(NextSibling()); }
    Function* Prev() { return static_cast<Function*>(PrevSibling()); }
    bool IsEmpty() const { return basicBlocks .IsEmpty(); }
    BasicBlock* GetBasicBlock(int32_t id) const;
    BasicBlock* AddBasicBlock(const SourcePos& sourcePos, int32_t id, Context* context);
    bool RemoveBasicBlock(BasicBlock* block);
    BasicBlock* FirstBasicBlock() { return static_cast<BasicBlock*>(basicBlocks.FirstChild()); }
    BasicBlock* LastBasicBlock() { return static_cast<BasicBlock*>(basicBlocks.LastChild()); }
    const SourcePos& GetSourcePos() const { return sourcePos; }
    FunctionType* GetType() const { return type; }
    const std::string& Name() const { return name; }
    MetadataRef* GetMetadataRef() const { return metadataRef; }
    RegValue* GetRegValue(int32_t reg) const;
    RegValue* GetRegRef(const SourcePos& sourcePos, Type* type, int32_t reg, Context* context) const;
    RegValue* MakeRegValue(const SourcePos& sourcePos, Type* type, int32_t reg, Context* context);
    Instruction* GetInstruction(int32_t reg) const;
    void MapInstruction(int32_t reg, Instruction* inst, Context* context);
    int NumBasicBlocks() const;
    const std::vector<BasicBlock*>& RetBlocks() const { return retBlocks; }
    void AddRetBlock(BasicBlock* retBlock);
    void AddEntryAndExitBlocks();
    void RemoveEntryAndExitBlocks();
    sngxml::dom::Element* ToXml();
    void WriteXmlDocument(const std::string& filePath);
    void SetNextRegNumber(int32_t nextRegNumber_) { nextRegNumber = nextRegNumber_; }
    int32_t NextRegNumber() const { return nextRegNumber; }
private:
    FunctionFlags flags;
    SourcePos sourcePos;
    FunctionType* type;
    std::string name;
    MetadataRef* metadataRef;
    soulng::util::Container basicBlocks;
    std::map<int32_t, BasicBlock*> basicBlockMap;
    std::map<int32_t, RegValue*> regValueMap;
    std::map<int32_t, Instruction*> instructionMap;
    std::vector<std::unique_ptr<RegValue>> regValues;
    std::vector<BasicBlock*> retBlocks;
    int32_t nextRegNumber;
};

class CMSX_INTERMEDIATE_API Code : public soulng::util::Component
{
public:
    Code();
    Code(const Code&) = delete;
    Code& operator=(const Code&) = delete;
    Context* GetContext() const { return context; }
    void SetContext(Context* context_) { context = context_; }
    Function* CurrentFunction() const { return currentFunction; }
    void SetCurrentFunction(Function* function);
    Function* GetFunction(const std::string& functionId) const;
    Function* AddFunctionDefinition(const SourcePos& sourcePos, FunctionType* functionType, const std::string& functionId, bool once, MetadataRef* metadataRef, Context* context);
    Function* AddFunctionDeclaration(const SourcePos& sourcePos, FunctionType* functionType, const std::string& functionId);
    Function* FirstFunction() { return static_cast<Function*>(functions.FirstChild()); }
    Function* LastFunction() { return static_cast<Function*>(functions.LastChild()); }
    void VisitFunctions(Visitor& visitor);
private:
    Context* context;
    Function* currentFunction;
    soulng::util::Container functions;
    std::map<std::string, Function*> functionMap;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_CODE_INCLUDED
