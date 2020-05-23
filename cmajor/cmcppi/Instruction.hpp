// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_INSTRUCTION_INCLUDED
#define CMAJOR_CMCPPI_INSTRUCTION_INCLUDED
#include <cmajor/cmcppi/Value.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <vector>
#include <utility>

namespace cmcppi {

using namespace soulng::util;

class Type;
class BasicBlock;
class Function;
class Context;
class MDStructRef;

class CMCPPI_API Instruction : public Value
{
public:
    Instruction();
    Type* GetType(Context& context) override;
    virtual void Write(CodeFormatter& formatter, Function& function, Context& context) = 0;
    std::string Name(Context& context) override;
    void WriteResult(CodeFormatter& formatter, Function& function, Context& context);
    virtual bool IsNoOperation() const { return false; }
    virtual std::string IrName() const = 0;
    void SetSourceLineNumber(int sourceLineNumber_) { sourceLineNumber = sourceLineNumber_; }
    int SourceLineNumber() const { return sourceLineNumber; }
    void SetResultId(int resultId_) { resultId = resultId_; }
    void SetNoSemicolon() { noSemicolon = true; }
    bool NoSemicolon() const { return noSemicolon; }
private:
    int resultId;
    int sourceLineNumber;
    bool noSemicolon;
};

class CMCPPI_API UnaryInstruction : public Instruction
{
public:
    UnaryInstruction(Value* arg_);
    Type* GetType(Context& context) override { return arg->GetType(context); }
    void WriteArg(CodeFormatter& formatter, Context& context);
private:
    Value* arg;
};

class CMCPPI_API UnaryTypeInstruction : public UnaryInstruction
{
public:
    UnaryTypeInstruction(Value* arg_, Type* type_);
    Type* GetType(Context& context) override { return type; }
private:
    Type* type;
};

class CMCPPI_API BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(Value* left_, Value* right_);
    Type* GetType(Context& context) override;
    void WriteArgs(CodeFormatter& formatter, Context& context, const std::string& op);
private:
    Value* left;
    Value* right;
};

class CMCPPI_API NotInstruction : public UnaryInstruction
{
public:
    NotInstruction(Value* arg_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "not"; }
};

class CMCPPI_API NegInstruction : public UnaryInstruction
{
public:
    NegInstruction(Value* arg_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "neg"; }
};

class CMCPPI_API AddInstruction : public BinaryInstruction
{
public:
    AddInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "add"; }
};

class CMCPPI_API SubInstruction : public BinaryInstruction
{
public:
    SubInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "sub"; }
};

class CMCPPI_API MulInstruction : public BinaryInstruction
{
public:
    MulInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "mul"; }
};

class CMCPPI_API DivInstruction : public BinaryInstruction
{
public:
    DivInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "div"; }
};

class CMCPPI_API ModInstruction : public BinaryInstruction
{
public:
    ModInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "mod"; }
};

class CMCPPI_API AndInstruction : public BinaryInstruction
{
public:
    AndInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "and"; }
};

class CMCPPI_API OrInstruction : public BinaryInstruction
{
public:
    OrInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "or"; }
};

class CMCPPI_API XorInstruction : public BinaryInstruction
{
public:
    XorInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "xor"; }
};

class CMCPPI_API ShlInstruction : public BinaryInstruction
{
public:
    ShlInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "shl"; }
};

class CMCPPI_API ShrInstruction : public BinaryInstruction
{
public:
    ShrInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "shr"; }
};

class CMCPPI_API EqualInstruction : public BinaryInstruction
{
public:
    EqualInstruction(Value* left_, Value* right_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "equal"; }
};

class CMCPPI_API LessInstruction : public BinaryInstruction
{
public:
    LessInstruction(Value* left_, Value* right_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "less"; }
};

class CMCPPI_API SignExtendInstruction : public UnaryTypeInstruction
{
public:
    SignExtendInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "signextend"; }
};

class CMCPPI_API ZeroExtendInstruction : public UnaryTypeInstruction
{
public:
    ZeroExtendInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "zeroextend"; }
};

class CMCPPI_API TruncateInstruction : public UnaryTypeInstruction
{
public:
    TruncateInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "truncate"; }
};

class CMCPPI_API BitCastInstruction : public UnaryTypeInstruction
{
public:
    BitCastInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "bitcast"; }
};

class CMCPPI_API IntToFloatInstruction : public UnaryTypeInstruction
{
public:
    IntToFloatInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "inttofloat"; }
};

class CMCPPI_API FloatToIntInstruction : public UnaryTypeInstruction
{
public:
    FloatToIntInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "floattoint"; }
};

class CMCPPI_API IntToPtrInstruction : public UnaryTypeInstruction
{
public:
    IntToPtrInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "inttoptr"; }
};

class CMCPPI_API PtrToIntInstruction : public UnaryTypeInstruction
{
public:
    PtrToIntInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "ptrtoint"; }
};

class CMCPPI_API ParamInstruction : public Instruction
{
public:
    ParamInstruction(Type* type_, const std::string& paramName_);
    Type* GetType(Context& context) override { return type; }
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string Name(Context& context) override { return paramName; }
    std::string IrName() const override { return "param"; }
private:
    Type* type;
    std::string paramName;
};

class CMCPPI_API LocalInstruction : public Instruction
{
public:
    LocalInstruction(Type* type_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "local"; }
    const std::string& LocalName() const { return localName; }
private:
    Type* type;
    std::string localName;
};

class CMCPPI_API LoadInstruction : public Instruction
{
public:
    LoadInstruction(Value* ptr_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "load"; }
private:
    Value* ptr;
};

class CMCPPI_API StoreInstruction : public Instruction
{
public:
    StoreInstruction(Value* value_, Value* ptr_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "store"; }
private:
    Value* value;
    Value* ptr;
};

class CMCPPI_API ArgInstruction : public Instruction
{
public:
    ArgInstruction(Value* arg_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "arg"; }
    std::string Name(Context& context) override { return argName; }
private:
    Value* arg;
    std::string argName;
};

class CMCPPI_API ElemAddrInstruction : public Instruction
{
public:
    ElemAddrInstruction(Value* ptr_, Value* index_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "elemaddr"; }
private:
    Value* ptr;
    Value* index;
};

class CMCPPI_API PtrOffsetInstruction : public Instruction
{
public:
    PtrOffsetInstruction(Value* ptr_, Value* offset_);
    Type* GetType(Context& context) override { return ptr->GetType(context); }
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "ptroffset"; }
private:
    Value* ptr;
    Value* offset;
};

class CMCPPI_API PtrDiffInstruction : public Instruction
{
public:
    PtrDiffInstruction(Value* leftPtr_, Value* rightPtr_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "ptrdiff"; }
private:
    Value* leftPtr;
    Value* rightPtr;
};

class CMCPPI_API CallInstruction : public Instruction
{
public:
    CallInstruction(Value* function_, const std::vector<Value*>& args_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "call"; }
private:
    Value* function;
    std::vector<Value*> args;
};

class CMCPPI_API InvokeInstruction : public Instruction
{
public:
    InvokeInstruction(Value* function_, const std::vector<Value*> args_, BasicBlock* normalBlockNext_, BasicBlock* unwindBlockNext_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "invoke"; }
private:
    Value* function;
    std::vector<Value*> args;
    BasicBlock* normalBlockNext;
    BasicBlock* unwindBlockNext;
};

class CMCPPI_API RetInstruction : public Instruction
{
public:
    RetInstruction(Value* value_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "ret"; }
private:
    Value* value;
};

class CMCPPI_API JumpInstruction : public Instruction
{
public:
    JumpInstruction(BasicBlock* dest_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "jmp"; }
private:
    BasicBlock* dest;
};

class CMCPPI_API BranchInstruction : public Instruction
{
public:
    BranchInstruction(Value* cond_, BasicBlock* trueDest_, BasicBlock* falseDest_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "branch"; }
private:
    Value* cond;
    BasicBlock* trueDest;
    BasicBlock* falseDest;
};

class CMCPPI_API SwitchInstruction : public Instruction
{
public:
    SwitchInstruction(Value* cond_, BasicBlock* defaultDest_);
    void AddCase(Value* caseValue, BasicBlock* dest);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "switch"; }
private:
    Value* cond;
    BasicBlock* defaultDest;
    std::vector<std::pair<Value*, BasicBlock*>> destinations;
};

class CMCPPI_API NoOperationInstruction : public Instruction
{
public:
    NoOperationInstruction();
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    bool IsNoOperation() const override { return true; }
    std::string IrName() const override { return "nop"; }
};

class CMCPPI_API BeginTryInstruction : public Instruction
{
public:
    BeginTryInstruction();
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "beginTry"; }
};

class CMCPPI_API EndTryInstruction : public Instruction
{
public:
    EndTryInstruction(BasicBlock* nextDest_, BasicBlock* handlersDest_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "endTry"; }
private:
    BasicBlock* nextDest;
    BasicBlock* handlersDest;
};

class CMCPPI_API ResumeInstruction : public Instruction
{
public:
    ResumeInstruction();
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    std::string IrName() const override { return "resume"; }
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_INSTRUCTION_INCLUDED
