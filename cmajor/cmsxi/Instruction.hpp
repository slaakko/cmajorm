// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_INSTRUCTION_INCLUDED
#define CMAJOR_CMSXI_INSTRUCTION_INCLUDED
#include <cmajor/cmsxi/Value.hpp>
#include <cmajor/util/CodeFormatter.hpp>
#include <vector>
#include <utility>

namespace cmsxi {

using namespace cmajor::util;

class Type;
class BasicBlock;
class Function;
class Context;
class MDStructRef;

class CMSXI_API Instruction : public Value
{
public:
    Instruction();
    Type* GetType(Context& context) override;
    virtual void Write(CodeFormatter& formatter, Function& function, Context& context) = 0;
    std::string Name(Context& context) override;
    void WriteResult(CodeFormatter& formatter, Function& function, Context& context);
    void SetMetadataRef(MDStructRef* metadataRef_) { metadataRef = metadataRef_; }
    void WriteMetadataRef(CodeFormatter& formatter);
    virtual bool IsNoOperation() const { return false; }
private:
    int resultId;
    MDStructRef* metadataRef;
};

class CMSXI_API UnaryInstruction : public Instruction
{
public:
    UnaryInstruction(Value* arg_);
    Type* GetType(Context& context) override { return arg->GetType(context); }
    void WriteArg(CodeFormatter& formatter, Context& context);
private:
    Value* arg;
};

class CMSXI_API UnaryTypeInstruction : public UnaryInstruction
{
public:
    UnaryTypeInstruction(Value* arg_, Type* type_);
    Type* GetType(Context& context) override { return type; }
private:
    Type* type;
};

class CMSXI_API BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(Value* left_, Value* right_);
    Type* GetType(Context& context) override;
    void WriteArgs(CodeFormatter& formatter, Context& context);
private:
    Value* left;
    Value* right;
};

class CMSXI_API NotInstruction : public UnaryInstruction
{
public:
    NotInstruction(Value* arg_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API NegInstruction : public UnaryInstruction
{
public:
    NegInstruction(Value* arg_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API AddInstruction : public BinaryInstruction
{
public:
    AddInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API SubInstruction : public BinaryInstruction
{
public:
    SubInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API MulInstruction : public BinaryInstruction
{
public:
    MulInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API DivInstruction : public BinaryInstruction
{
public:
    DivInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API ModInstruction : public BinaryInstruction
{
public:
    ModInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API AndInstruction : public BinaryInstruction
{
public:
    AndInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API OrInstruction : public BinaryInstruction
{
public:
    OrInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API XorInstruction : public BinaryInstruction
{
public:
    XorInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API ShlInstruction : public BinaryInstruction
{
public:
    ShlInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API ShrInstruction : public BinaryInstruction
{
public:
    ShrInstruction(Value* left_, Value* right_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API EqualInstruction : public BinaryInstruction
{
public:
    EqualInstruction(Value* left_, Value* right_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API LessInstruction : public BinaryInstruction
{
public:
    LessInstruction(Value* left_, Value* right_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API SignExtendInstruction : public UnaryTypeInstruction
{
public:
    SignExtendInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API ZeroExtendInstruction : public UnaryTypeInstruction
{
public:
    ZeroExtendInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API TruncateInstruction : public UnaryTypeInstruction
{
public:
    TruncateInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API BitCastInstruction : public UnaryTypeInstruction
{
public:
    BitCastInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API IntToFloatInstruction : public UnaryTypeInstruction
{
public:
    IntToFloatInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API FloatToIntInstruction : public UnaryTypeInstruction
{
public:
    FloatToIntInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API IntToPtrInstruction : public UnaryTypeInstruction
{
public:
    IntToPtrInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API PtrToIntInstruction : public UnaryTypeInstruction
{
public:
    PtrToIntInstruction(Value* arg_, Type* destType_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
};

class CMSXI_API ParamInstruction : public Instruction
{
public:
    ParamInstruction(Type* type_);
    Type* GetType(Context& context) override { return type; }
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Type* type;
};

class CMSXI_API LocalInstruction : public Instruction
{
public:
    LocalInstruction(Type* type_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Type* type;
};

class CMSXI_API LoadInstruction : public Instruction
{
public:
    LoadInstruction(Value* ptr_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* ptr;
};

class CMSXI_API StoreInstruction : public Instruction
{
public:
    StoreInstruction(Value* value_, Value* ptr_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* value;
    Value* ptr;
};

class CMSXI_API ArgInstruction : public Instruction
{
public:
    ArgInstruction(Value* arg_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* arg;
};

class CMSXI_API ElemAddrInstruction : public Instruction
{
public:
    ElemAddrInstruction(Value* ptr_, Value* index_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* ptr;
    Value* index;
};

class CMSXI_API PtrOffsetInstruction : public Instruction
{
public:
    PtrOffsetInstruction(Value* ptr_, Value* offset_);
    Type* GetType(Context& context) override { return ptr->GetType(context); }
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* ptr;
    Value* offset;
};

class CMSXI_API PtrDiffInstruction : public Instruction
{
public:
    PtrDiffInstruction(Value* leftPtr_, Value* rightPtr_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* leftPtr;
    Value* rightPtr;
};

class CMSXI_API CallInstruction : public Instruction
{
public:
    CallInstruction(Value* function_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* function;
};

class CMSXI_API RetInstruction : public Instruction
{
public:
    RetInstruction(Value* value_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* value;
};

class CMSXI_API JumpInstruction : public Instruction
{
public:
    JumpInstruction(BasicBlock* dest_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    BasicBlock* dest;
};

class CMSXI_API BranchInstruction : public Instruction
{
public:
    BranchInstruction(Value* cond_, BasicBlock* trueDest_, BasicBlock* falseDest_);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* cond;
    BasicBlock* trueDest;
    BasicBlock* falseDest;
};

class CMSXI_API SwitchInstruction : public Instruction
{
public:
    SwitchInstruction(Value* cond_, BasicBlock* defaultDest_);
    void AddCase(Value* caseValue, BasicBlock* dest);
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* cond;
    BasicBlock* defaultDest;
    std::vector<std::pair<Value*, BasicBlock*>> destinations;
};

class CMSXI_API TrapInstruction : public Instruction
{
public:
    TrapInstruction(Value* b0_, Value* b1_, Value* b2_);
    Type* GetType(Context& context) override;
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
private:
    Value* b0;
    Value* b1;
    Value* b2;
};

class CMSXI_API NoOperationInstruction : public Instruction
{
public:
    NoOperationInstruction();
    void Write(CodeFormatter& formatter, Function& function, Context& context) override;
    bool IsNoOperation() const override { return true; }
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_INSTRUCTION_INCLUDED
