// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Code.hpp>
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Visitor.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/Util.hpp>
#include <sngxml/dom/Document.hpp>
#include <soulng/util/Unicode.hpp>
#include <fstream>

namespace cmsx::intermediate {

using namespace soulng::unicode;

const char* opCodeStr[] =
{
    "store", "arg", "jmp", "branch", "call", "ret", "switch",
    "not", "neg", "signextend", "zeroextend", "truncate", "bitcast", "inttofloat", "floattoint", "inttoptr", "ptrtoint",
    "add", "sub", "mul", "div", "mod", "and", "or", "xor", "shl", "shr", "equal", "less",
    "param", "local", "load", "elemaddr", "ptroffset", "ptrdiff", "call", "trap", "phi",
    "nop"
};

RegValue::RegValue(const SourcePos& sourcePos_, Type* type_, int32_t reg_) : Value(sourcePos_, ValueKind::regValue, type_), reg(reg_), inst(nullptr)
{
}

void Use::Set(Value* value_)
{
    if (value == value_)
    {
        return;
    }
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->RemoveUser(user);
        }
    }
    value = value_;
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(user);
        }
    }
}

Instruction::Instruction(const SourcePos& sourcePos_, Type* type_, OpCode opCode_) : Value(sourcePos_, ValueKind::instruction, type_), opCode(opCode_), metadataRef(nullptr), index(-1)
{
}

std::string Instruction::Name() const
{
    return opCodeStr[static_cast<int>(opCode)];
}

bool Instruction::IsLeader() const
{
    BasicBlock* basicBlock = Parent();
    return this == basicBlock->FirstInstruction();
}

bool Instruction::IsTerminator() const
{
    switch (opCode)
    {
        case OpCode::jmp:
        case OpCode::branch:
        case OpCode::ret:
        case OpCode::switch_:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

bool Instruction::IsValueInstruction() const
{
    switch (opCode)
    {
        case OpCode::not_:
        case OpCode::neg:
        case OpCode::signextend:
        case OpCode::zeroextend:
        case OpCode::truncate:
        case OpCode::bitcast:
        case OpCode::inttofloat:
        case OpCode::floattoint:
        case OpCode::inttoptr:
        case OpCode::ptrtoint:
        case OpCode::add:
        case OpCode::sub:
        case OpCode::mul:
        case OpCode::div_:
        case OpCode::mod:
        case OpCode::and_:
        case OpCode::or_:
        case OpCode::xor_:
        case OpCode::shl:
        case OpCode::shr:
        case OpCode::equal:
        case OpCode::less:
        case OpCode::param:
        case OpCode::local:
        case OpCode::load:
        case OpCode::elemaddr:
        case OpCode::ptroffset:
        case OpCode::ptrdiff:
        case OpCode::function_call:
        case OpCode::trap:
        case OpCode::phi:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

bool Instruction::IsUnaryInstruction() const
{
    switch (opCode)
    {
        case OpCode::not_:  
        case OpCode::neg:
        case OpCode::signextend:
        case OpCode::zeroextend:
        case OpCode::truncate:
        case OpCode::bitcast:
        case OpCode::inttofloat:
        case OpCode::floattoint:
        case OpCode::inttoptr:
        case OpCode::ptrtoint:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

bool Instruction::IsBinaryInstruction() const
{
    switch (opCode)
    {
        case OpCode::add:
        case OpCode::sub:
        case OpCode::mul:
        case OpCode::div_:
        case OpCode::mod:
        case OpCode::and_:
        case OpCode::or_:
        case OpCode::xor_:
        case OpCode::shl:
        case OpCode::shr:
        case OpCode::equal:
        case OpCode::less:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

bool Instruction::RequiresLocalRegister() const
{
    switch (opCode)
    {
        case OpCode::arg:
        case OpCode::procedure_call:
        case OpCode::not_:
        case OpCode::neg:
        case OpCode::signextend:
        case OpCode::zeroextend:
        case OpCode::truncate:
        case OpCode::bitcast:
        case OpCode::inttofloat:
        case OpCode::floattoint:
        case OpCode::inttoptr:
        case OpCode::ptrtoint:
        case OpCode::add:
        case OpCode::sub:
        case OpCode::mul:
        case OpCode::div_:
        case OpCode::mod:
        case OpCode::and_:
        case OpCode::or_:
        case OpCode::xor_:
        case OpCode::shl:
        case OpCode::shr:
        case OpCode::equal:
        case OpCode::less:
        case OpCode::param:
        case OpCode::load:
        case OpCode::elemaddr:
        case OpCode::ptroffset:
        case OpCode::ptrdiff:
        case OpCode::function_call:
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

std::vector<BasicBlock*> Instruction::Successors() const
{
    std::vector<BasicBlock*> successors;
    switch (opCode)
    {
        case OpCode::jmp:
        {
            const JmpInstruction* jmp = static_cast<const JmpInstruction*>(this);
            AddPtrToSet(jmp->TargetBasicBlock(), successors);
            break;
        }
        case OpCode::branch:
        {
            const BranchInstruction* branch = static_cast<const BranchInstruction*>(this);
            AddPtrToSet(branch->TrueTargetBasicBlock(), successors);
            AddPtrToSet(branch->FalseTargetBasicBlock(), successors);
            break;
        }
        case OpCode::switch_:
        {
            const SwitchInstruction* switch_ = static_cast<const SwitchInstruction*>(this);
            AddPtrToSet(switch_->DefaultTargetBlock(), successors);
            for (const CaseTarget& caseTarget : switch_->CaseTargets())
            {
                AddPtrToSet(caseTarget.targetBlock, successors);
            }
            break;
        }
    }
    return successors;
}

void Instruction::AddUser(Instruction* user)
{
    AddPtrToSet(user, users);
}

void Instruction::RemoveUser(Instruction* user)
{
    RemovePtrFromSet(user, users);
}

std::vector<Use> Instruction::Uses()
{
    std::vector<Use> uses;
    switch (opCode)
    {
        case OpCode::store:
        {
            StoreInstruction* store = static_cast<StoreInstruction*>(this);
            store->AddToUses(uses);
            break;
        }
        case OpCode::arg:
        {
            ArgInstruction* arg = static_cast<ArgInstruction*>(this);
            arg->AddToUses(uses);
            break;
        }
        case OpCode::branch:
        {
            BranchInstruction* branch = static_cast<BranchInstruction*>(this);
            branch->AddToUses(uses);
            break;
        }
        case OpCode::procedure_call:
        {
            ProcedureCallInstruction* call = static_cast<ProcedureCallInstruction*>(this);
            call->AddToUses(uses);
            break;
        }
        case OpCode::ret:
        {
            RetInstruction* ret = static_cast<RetInstruction*>(this);
            ret->AddToUses(uses);
            break;
        }
        case OpCode::switch_:
        {
            SwitchInstruction* switch_ = static_cast<SwitchInstruction*>(this);
            switch_->AddToUses(uses);
            break;
        }
        case OpCode::not_:
        case OpCode::neg: 
        case OpCode::signextend:
        case OpCode::zeroextend:
        case OpCode::truncate:
        case OpCode::bitcast:
        case OpCode::inttofloat:
        case OpCode::floattoint:
        case OpCode::inttoptr:
        case OpCode::ptrtoint:
        {
            UnaryInstruction* inst = static_cast<UnaryInstruction*>(this);
            inst->AddOperandToUses(uses);
            break;
        }
        case OpCode::add:
        case OpCode::sub:
        case OpCode::mul:
        case OpCode::div_:
        case OpCode::mod:
        case OpCode::and_:
        case OpCode::or_:
        case OpCode::xor_:
        case OpCode::shl:
        case OpCode::shr:
        case OpCode::equal:
        case OpCode::less:
        {
            BinaryInstruction* inst = static_cast<BinaryInstruction*>(this);
            inst->AddOperandsToUses(uses);
            break;
        }
        case OpCode::load:
        {
            LoadInstruction* load = static_cast<LoadInstruction*>(this);
            load->AddToUses(uses);
            break;
        }
        case OpCode::elemaddr:
        {
            ElemAddrInstruction* elemAddr = static_cast<ElemAddrInstruction*>(this);
            elemAddr->AddToUses(uses);
            break;
        }
        case OpCode::ptroffset:
        {
            PtrOffsetInstruction* ptrOffset = static_cast<PtrOffsetInstruction*>(this);
            ptrOffset->AddToUses(uses);
            break;
        }
        case OpCode::ptrdiff:
        {
            PtrDiffInstruction* ptrDiff = static_cast<PtrDiffInstruction*>(this);
            ptrDiff->AddToUses(uses);
            break;
        }
        case OpCode::function_call:
        {
            FunctionCallInstruction* call = static_cast<FunctionCallInstruction*>(this);
            call->AddToUses(uses);
            break;
        }
        case OpCode::trap:
        {
            TrapInstruction* trap = static_cast<TrapInstruction*>(this);
            trap->AddToUses(uses);
            break;
        }
        case OpCode::phi:
        {
            PhiInstruction* phi = static_cast<PhiInstruction*>(this);
            phi->AddToUses(uses);
            break;
        }
    }
    return uses;
}

void Instruction::ReplaceUsesWith(Value* value)
{
    std::vector<Instruction*> users = Users();
    for (Instruction* user : users)
    {
        std::vector<Use> uses = user->Uses();
        for (Use& use : uses)
        {
            use.Set(value);
        }
    }
}

std::unique_ptr<Instruction> Instruction::Remove()
{
    BasicBlock* parent = Parent();
    std::unique_ptr<Component> removed = parent->GetContainer()->RemoveChild(this);
    return std::unique_ptr<Instruction>(static_cast<Instruction*>(removed.release()));
}

BasicBlock* Instruction::Parent() const
{ 
    return static_cast<BasicBlock*>(GetContainer()->Parent()); 
}

StoreInstruction::StoreInstruction(const SourcePos& sourcePos_, Value* value_, Value* ptr_) : Instruction(sourcePos_, nullptr, OpCode::store), value(value_), ptr(ptr_)
{
}

void StoreInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void StoreInstruction::AddUse()
{
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    if (ptr->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(ptr);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void StoreInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, value));
    uses.push_back(Use(this, ptr));
}

ArgInstruction::ArgInstruction(const SourcePos& sourcePos_, Value* arg_) : Instruction(sourcePos_, nullptr, OpCode::arg), arg(arg_)
{
}

void ArgInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ArgInstruction::AddUse()
{
    if (arg->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(arg);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void ArgInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, arg));
}

JmpInstruction::JmpInstruction(const SourcePos& sourcePos_, int32_t targetLabelId_) : 
    Instruction(sourcePos_, nullptr, OpCode::jmp), targetLabelId(targetLabelId_), targetBasicBlock(nullptr)
{
}

void JmpInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BranchInstruction::BranchInstruction(const SourcePos& sourcePos_, Value* cond_, int32_t trueTargetLabelId_, int32_t falseTargetLabelId_) : 
    Instruction(sourcePos_, nullptr, OpCode::branch), 
    cond(cond_), trueTargetLabelId(trueTargetLabelId_), trueTargetBasicBlock(nullptr), falseTargetLabelId(falseTargetLabelId_), falseTargetBasicBlock(nullptr)
{
}

void BranchInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BranchInstruction::AddUse()
{
    if (cond->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(cond);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void BranchInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, cond));
}

ProcedureCallInstruction::ProcedureCallInstruction(const SourcePos& sourcePos_, Value* callee_) : Instruction(sourcePos_, nullptr, OpCode::procedure_call), callee(callee_)
{
}

void ProcedureCallInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ProcedureCallInstruction::SetArgs(std::vector<Value*>&& args_)
{
    args = std::move(args_);
}

void ProcedureCallInstruction::AddUse()
{
    if (callee->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(callee);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
/*  TODO: arg uses required?
    for (Value* arg : args)
    {
        if (arg->IsRegValue())
        {
            RegValue* regValue = static_cast<RegValue*>(arg);
            Instruction* inst = regValue->Inst();
            if (inst)
            {
                inst->AddUser(this);
            }
        }
    }
*/
}

void ProcedureCallInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, callee));
/*  TODO: arg uses required?
    for (Value*& arg : args)
    {
        uses.push_back(Use(this, arg));
    }
*/
}

RetInstruction::RetInstruction(const SourcePos& sourcePos_, Value* returnValue_) : Instruction(sourcePos_, nullptr, OpCode::ret), returnValue(returnValue_)
{
}

void RetInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void RetInstruction::AddUse()
{
    if (returnValue)
    {
        if (returnValue->IsRegValue())
        {
            RegValue* regValue = static_cast<RegValue*>(returnValue);
            Instruction* inst = regValue->Inst();
            if (inst)
            {
                inst->AddUser(this);
            }
        }
    }
}

void RetInstruction::AddToUses(std::vector<Use>& uses)
{
    if (returnValue)
    {
        uses.push_back(Use(this, returnValue));
    }
}

SwitchInstruction::SwitchInstruction(const SourcePos& sourcePos_, Value* cond_, int32_t defaultTargetLabelId_) : 
    Instruction(sourcePos_, nullptr, OpCode::switch_), cond(cond_), defaultTargetLabelId(defaultTargetLabelId_), defaultTargetBlock(nullptr)
{
}

void SwitchInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void SwitchInstruction::AddCaseTarget(const CaseTarget& caseTarget)
{
    caseTargets.push_back(caseTarget);
}

void SwitchInstruction::AddUse()
{
    if (cond->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(cond);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    for (CaseTarget& caseTarget : caseTargets)
    {
        if (caseTarget.caseValue->IsRegValue())
        {
            RegValue* regValue = static_cast<RegValue*>(caseTarget.caseValue);
            Instruction* inst = regValue->Inst();
            if (inst)
            {
                inst->AddUser(this);
            }
        }
    }
}

void SwitchInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, cond));
    for (CaseTarget& caseTarget : caseTargets)
    {
        uses.push_back(Use(this, caseTarget.caseValue));
    }
}

ValueInstruction::ValueInstruction(const SourcePos& sourcePos_, RegValue* result_, OpCode opCode_) : Instruction(sourcePos_, result_->GetType(), opCode_), result(result_)
{
}

UnaryInstruction::UnaryInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_, OpCode opCode_) : ValueInstruction(sourcePos_, result_, opCode_), operand(operand_)
{
}

void UnaryInstruction::AddUse()
{
    if (operand->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(operand);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void UnaryInstruction::AddOperandToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, operand));
}

NotInstruction::NotInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::not_)
{
}

void NotInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NegInstruction::NegInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::neg)
{
}

void NegInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SignExtendInstruction::SignExtendInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::signextend)
{
}

void SignExtendInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ZeroExtendInstruction::ZeroExtendInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::zeroextend)
{
}

void ZeroExtendInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

TruncateInstruction::TruncateInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::truncate)
{
}

void TruncateInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BitcastInstruction::BitcastInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::bitcast)
{
}

void BitcastInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IntToFloatInstruction::IntToFloatInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::inttofloat)
{
}

void IntToFloatInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

FloatToIntInstruction::FloatToIntInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::floattoint)
{
}

void FloatToIntInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IntToPtrInstruction::IntToPtrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::inttoptr)
{
}

void IntToPtrInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PtrToIntInstruction::PtrToIntInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* operand_) : UnaryInstruction(sourcePos_, result_, operand_, OpCode::ptrtoint)
{
}

void PtrToIntInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BinaryInstruction::BinaryInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_, OpCode opCode_) : 
    ValueInstruction(sourcePos_, result_, opCode_), left(left_), right(right_)
{
}

void BinaryInstruction::AddUse()
{
    if (Left()->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(Left());
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    if (Right()->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(Right());
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void BinaryInstruction::AddOperandsToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, left));
    uses.push_back(Use(this, right));
}

AddInstruction::AddInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::add)
{
}

void AddInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SubInstruction::SubInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::sub)
{
}

void SubInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

MulInstruction::MulInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::mul)
{
}

void MulInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DivInstruction::DivInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::div_)
{
}

void DivInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ModInstruction::ModInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::mod)
{
}

void ModInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AndInstruction::AndInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::and_)
{
}

void AndInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

OrInstruction::OrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::or_)
{
}

void OrInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

XorInstruction::XorInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::xor_)
{
}

void XorInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ShlInstruction::ShlInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::shl)
{
}

void ShlInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ShrInstruction::ShrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::shr)
{
}

void ShrInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

EqualInstruction::EqualInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::equal)
{
}

void EqualInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LessInstruction::LessInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* left_, Value* right_) : BinaryInstruction(sourcePos_, result_, left_, right_, OpCode::less)
{
}

void LessInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ParamInstruction::ParamInstruction(const SourcePos& sourcePos_, RegValue* result_) : ValueInstruction(sourcePos_, result_, OpCode::param)
{
}

void ParamInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LocalInstruction::LocalInstruction(const SourcePos& sourcePos_, RegValue* result_, Type* localType_) : ValueInstruction(sourcePos_, result_, OpCode::local), localType(localType_)
{
}

void LocalInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LoadInstruction::LoadInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* ptr_) : ValueInstruction(sourcePos_, result_, OpCode::load), ptr(ptr_)
{
}

void LoadInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void LoadInstruction::AddUse()
{
    if (ptr->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(ptr);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void LoadInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, ptr));
}

ElemAddrInstruction::ElemAddrInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* ptr_, Value* index_) : 
    ValueInstruction(sourcePos_, result_, OpCode::elemaddr), ptr(ptr_), index(index_)
{
}

void ElemAddrInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ElemAddrInstruction::AddUse()
{
    if (ptr->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(ptr);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    if (index->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(index);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void ElemAddrInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, ptr));
    uses.push_back(Use(this, index));
}

ElemAddrKind ElemAddrInstruction::GetElemAddrKind(Context* context) const
{
    if (ptr->GetType()->IsPointerType())
    {
        PointerType* ptrType = static_cast<PointerType*>(ptr->GetType());
        Type* pointeeType = ptrType->RemovePointer(GetSourcePos(), context);
        if (pointeeType->IsArrayType())
        {
            return ElemAddrKind::array;
        }
        else if (pointeeType->IsStructureType())
        {
            return ElemAddrKind::structure;
        }
    }
    Error("invalid elem addr kind", GetSourcePos(), context);
    return ElemAddrKind::none;
}

PtrOffsetInstruction::PtrOffsetInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* ptr_, Value* offset_) :
    ValueInstruction(sourcePos_, result_, OpCode::ptroffset), ptr(ptr_), offset(offset_)
{
}

void PtrOffsetInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void PtrOffsetInstruction::AddUse()
{
    if (ptr->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(ptr);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    if (offset->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(offset);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void PtrOffsetInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, ptr));
    uses.push_back(Use(this, offset));
}

PtrDiffInstruction::PtrDiffInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* leftPtr_, Value* rightPtr_) : 
    ValueInstruction(sourcePos_, result_, OpCode::ptrdiff), leftPtr(leftPtr_), rightPtr(rightPtr_)
{
}

void PtrDiffInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void PtrDiffInstruction::AddUse()
{
    if (leftPtr->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(leftPtr);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    if (rightPtr->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(rightPtr);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
}

void PtrDiffInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, leftPtr));
    uses.push_back(Use(this, rightPtr));
}

FunctionCallInstruction::FunctionCallInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* callee_) : 
    ValueInstruction(sourcePos_, result_, OpCode::function_call), callee(callee_)
{
}

void FunctionCallInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void FunctionCallInstruction::SetArgs(std::vector<Value*>&& args_)
{
    args = std::move(args_);
}

void FunctionCallInstruction::AddUse()
{
    if (callee->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(callee);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
/*  TODO: arg uses required?
    for (Value* arg : args)
    {
        if (arg->IsRegValue())
        {
            RegValue* regValue = static_cast<RegValue*>(arg);
            Instruction* inst = regValue->Inst();
            if (inst)
            {
                inst->AddUser(this);
            }
        }
    }
*/
}

void FunctionCallInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, callee));
/*  TODO: arg uses required?
    for (Value*& arg : args)
    {
        uses.push_back(Use(this, arg));
    }
*/
}

TrapInstruction::TrapInstruction(const SourcePos& sourcePos_, RegValue* result_, Value* op1_, Value* op2_, Value* op3_) : 
    ValueInstruction(sourcePos_, result_, OpCode::trap), op1(op1_), op2(op2_), op3(op3_)
{
}

void TrapInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void TrapInstruction::SetArgs(std::vector<Value*>&& args_)
{
    args = std::move(args_);
}

void TrapInstruction::AddUse()
{
    if (op1->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(op1);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    if (op2->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(op2);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    if (op3->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(op3);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            inst->AddUser(this);
        }
    }
    for (Value* arg : args)
    {
        if (arg->IsRegValue())
        {
            RegValue* regValue = static_cast<RegValue*>(arg);
            Instruction* inst = regValue->Inst();
            if (inst)
            {
                inst->AddUser(this);
            }
        }
    }
}

void TrapInstruction::AddToUses(std::vector<Use>& uses)
{
    uses.push_back(Use(this, op1));
    uses.push_back(Use(this, op2));
    uses.push_back(Use(this, op3));
    for (Value*& arg : args)
    {
        uses.push_back(Use(this, arg));
    }
}

BlockValue::BlockValue(Value* value_, BasicBlock* block_) : value(value_), blockId(block_->Id()), block(block_)
{
}

PhiInstruction::PhiInstruction(const SourcePos& sourcePos_, RegValue* result_) : ValueInstruction(sourcePos_, result_, OpCode::phi)
{
}

void PhiInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void PhiInstruction::AddBlockValue(const BlockValue& blockValue)
{
    blockValues.push_back(blockValue);
}

void PhiInstruction::AddUse()
{
    for (BlockValue& blockValue : blockValues)
    {
        if (blockValue.value->IsRegValue())
        {
            RegValue* regValue = static_cast<RegValue*>(blockValue.value);
            Instruction* inst = regValue->Inst();
            if (inst)
            {
                inst->AddUser(this);
            }
        }
    }
}

void PhiInstruction::AddToUses(std::vector<Use>& uses)
{
    for (BlockValue& blockValue : blockValues)
    {
        uses.push_back(Use(this, blockValue.value));
    }
}

NoOperationInstruction::NoOperationInstruction(const SourcePos& sourcePos_) : Instruction(sourcePos_, nullptr, OpCode::nop)
{
}

void NoOperationInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BasicBlock::BasicBlock(const SourcePos& sourcePos_, int32_t id_) : sourcePos(sourcePos_), id(id_), instructions(this)
{
}

void BasicBlock::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BasicBlock::VisitInstructions(Visitor& visitor)
{
    Instruction* inst = FirstInstruction();
    while (inst != nullptr)
    {
        inst->Accept(visitor);
        inst = inst->Next();
    }
}

std::string BasicBlock::Name() const
{
    if (id == entryBlockId)
    {
        return "entry";
    }
    else if (id == exitBlockId)
    {
        return "exit";
    }
    else
    {
        return std::to_string(id);
    }
}

void BasicBlock::AddInstruction(Instruction* instruction, MetadataRef* metadataRef)
{
    instruction->SetMetadataRef(metadataRef);
    instructions.AddChild(instruction);
    if (instruction->IsValueInstruction())
    {
        ValueInstruction* valueInstruction = static_cast<ValueInstruction*>(instruction);
        Function* function = Parent();
        Context* context = function->Parent()->GetContext();
        function->MapInstruction(valueInstruction->Result()->Reg(), valueInstruction, context);
    }
}

void BasicBlock::InsertFront(Instruction* instruction)
{
    instructions.InsertBefore(instruction, instructions.FirstChild());
    // todo
}

void BasicBlock::AddSuccessor(BasicBlock* successor)
{
    AddPtrToSet(successor, successors);
}

bool BasicBlock::RemoveSuccessor(BasicBlock* successor)
{
    return RemovePtrFromSet(successor, successors);
}

void BasicBlock::AddPredecessor(BasicBlock* predecessor)
{
    AddPtrToSet(predecessor, predecessors);
}

bool BasicBlock::RemovePredecessor(BasicBlock* predecessor)
{
    return RemovePtrFromSet(predecessor, predecessors);
}

void BasicBlock::ClearSuccessorsAndPredecessors()
{
    successors.clear();
    predecessors.clear();
}

int BasicBlock::IndexOf(Instruction* x)
{
    int index = 0;
    Instruction* inst = FirstInstruction();
    while (inst)
    {
        if (inst == x)
        {
            return index;
        }
        ++index;
        inst = inst->Next();
    }
    return -1;
}

Function* BasicBlock::Parent() const
{
    return static_cast<Function*>(GetContainer()->Parent());
}

Function::Function(const SourcePos& sourcePos_, FunctionType* type_, const std::string& name_, bool once_, bool definition_, MetadataRef* metadataRef_) :
    flags(FunctionFlags::none), sourcePos(sourcePos_), type(type_), name(name_), metadataRef(metadataRef_), basicBlocks(this), nextRegNumber(0)
{
    if (once_)
    {
        SetFlag(FunctionFlags::once);
    }
    if (definition_)
    {
        SetFlag(FunctionFlags::defined);
    }
}

void Function::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void Function::VisitBasicBlocks(Visitor& visitor)
{
    BasicBlock* basicBlock = FirstBasicBlock();
    while (basicBlock)
    {
        basicBlock->Accept(visitor);
        basicBlock = basicBlock->Next();
    }
}

Code* Function::Parent() const
{
    return static_cast<Code*>(GetContainer()->Parent());
}

BasicBlock* Function::GetBasicBlock(int32_t id) const
{
    auto it = basicBlockMap.find(id);
    if (it != basicBlockMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

BasicBlock* Function::AddBasicBlock(const SourcePos& sourcePos, int32_t id, Context* context)
{
    BasicBlock* prev = GetBasicBlock(id);
    if (prev)
    {
        Error("error adding basic block: basic block id not unique", sourcePos, context, prev->GetSourcePos());
    }
    BasicBlock* basicBlock = new BasicBlock(sourcePos, id);
    basicBlocks.AddChild(basicBlock);
    basicBlockMap[id] = basicBlock;
    return basicBlock;
}

bool Function::RemoveBasicBlock(BasicBlock* block)
{
    if (!GetBasicBlock(block->Id())) return false;
    RemovePtrFromSet(block, retBlocks);
    basicBlockMap.erase(block->Id());
    for (BasicBlock* successor : block->Successors())
    {
        successor->RemovePredecessor(block);
    }
    for (BasicBlock* predecessor : block->Predecessors())
    {
        predecessor->RemoveSuccessor(block);
    }
    basicBlocks.RemoveChild(block);
    return true;
}

RegValue* Function::GetRegValue(int32_t reg) const
{
    auto it = regValueMap.find(reg);
    if (it != regValueMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

RegValue* Function::GetRegRef(const SourcePos& sourcePos, Type* type, int32_t reg, Context* context) const
{
    RegValue* regValue = GetRegValue(reg);
    if (regValue)
    {
        if (regValue->GetType() == type)
        {
            return regValue;
        }
        else
        {
            Error("error referencing register value " + std::to_string(reg) + ": type conflick", sourcePos, context, regValue->GetSourcePos());
        }
    }
    else
    {
        Error("error referencing register: register " + std::to_string(reg) + " not found", sourcePos, context);
    }
    return nullptr;
}

RegValue* Function::MakeRegValue(const SourcePos& sourcePos, Type* type, int32_t reg, Context* context)
{
    RegValue* prev = GetRegValue(reg);
    if (prev)
    {
        Error("error adding register " + std::to_string(reg) + ": register not unique", sourcePos, context, prev->GetSourcePos());
    }
    RegValue* regValue = new RegValue(sourcePos, type, reg);
    regValues.push_back(std::unique_ptr<RegValue>(regValue));
    regValueMap[reg] = regValue;
    return regValue;
}

Instruction* Function::GetInstruction(int32_t reg) const
{
    auto it = instructionMap.find(reg);
    if (it != instructionMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Function::MapInstruction(int32_t reg, Instruction* inst, Context* context)
{
    Instruction* prev = GetInstruction(reg);
    if (prev)
    {
        Error("error mappint instruction " + std::to_string(reg) + ": register number not unique", sourcePos, context, prev->GetSourcePos());
    }
    instructionMap[reg] = inst;
}

int Function::NumBasicBlocks() const
{
    return basicBlockMap.size();
}

void Function::AddRetBlock(BasicBlock* retBlock)
{
    AddPtrToSet(retBlock, retBlocks);
}

void Function::AddEntryAndExitBlocks()
{
    if (basicBlocks.IsEmpty())
    {
        Error("error adding entry and exit blocks: function '" + Name() + "' has no basic blocks", GetSourcePos(), Parent()->GetContext());
    }
    BasicBlock* prevEntryBlock = GetBasicBlock(entryBlockId);
    if (prevEntryBlock)
    {
        Error("error adding entry and exit blocks: function '" + Name() + "' has already an entry block", GetSourcePos(), Parent()->GetContext());
    }
    BasicBlock* prevExitBlock = GetBasicBlock(exitBlockId);
    if (prevExitBlock)
    {
        Error("error adding entry and exit blocks: function '" + Name() + "' has already an exit block", GetSourcePos(), Parent()->GetContext());
    }
    if (RetBlocks().empty())
    {
        Error("error adding entry and exit blocks: function '" + Name() + "' has no ret blocks", GetSourcePos(), Parent()->GetContext());
    }
    BasicBlock* firstBasicBlock = FirstBasicBlock();
    BasicBlock* entryBlock = new BasicBlock(GetSourcePos(), entryBlockId);
    basicBlockMap[entryBlock->Id()] = entryBlock;
    entryBlock->AddSuccessor(firstBasicBlock);
    firstBasicBlock->AddPredecessor(entryBlock);
    basicBlocks.InsertBefore(entryBlock, firstBasicBlock);
    BasicBlock* exitBlock = new BasicBlock(GetSourcePos(), exitBlockId);
    for (BasicBlock* retBlock : RetBlocks())
    {
        retBlock->AddSuccessor(exitBlock);
        exitBlock->AddPredecessor(retBlock);
    }
    basicBlocks.AddChild(exitBlock);
}


void Function::RemoveEntryAndExitBlocks()
{
    BasicBlock* entryBlock = GetBasicBlock(entryBlockId);
    if (!entryBlock)
    {
        Error("error removing entry and exit blocks: function '" + Name() + "' has no entry block", GetSourcePos(), Parent()->GetContext());
    }
    BasicBlock* exitBlock = GetBasicBlock(exitBlockId);
    if (!exitBlock)
    {
        Error("error removing entry and exit blocks: function '" + Name() + "' has no exit block", GetSourcePos(), Parent()->GetContext());
    }
    BasicBlock* firstBasicBlock = entryBlock->Next();
    if (!firstBasicBlock)
    {
        Error("error removing entry and exit blocks: function '" + Name() + "' has no basic blocks other than entry and exit blocks", GetSourcePos(), Parent()->GetContext());
    }
    if (!firstBasicBlock->RemovePredecessor(entryBlock))
    {
        Error("error removing entry and exit blocks: function '" + Name() + "' old first block does not have the entry block as a predecessor", GetSourcePos(), Parent()->GetContext());
    }
    basicBlocks.RemoveChild(entryBlock);
    basicBlockMap.erase(entryBlockId);
    for (BasicBlock* retBlock : RetBlocks())
    {
        if (!retBlock->RemoveSuccessor(exitBlock))
        {
            Error("error removing entry and exit blocks: function '" + Name() + "' ret block does not have the exit block as a successor", GetSourcePos(), Parent()->GetContext());
        }
    }
    basicBlocks.RemoveChild(exitBlock);
    basicBlockMap.erase(exitBlockId);
}

sngxml::dom::Element* Function::ToXml() 
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"function");
    element->SetAttribute(U"name", ToUtf32(Name())); 
    BasicBlock* block = FirstBasicBlock();
    while (block)
    {
        sngxml::dom::Element* blockElement = new sngxml::dom::Element(U"block");
        blockElement->SetAttribute(U"id", ToUtf32(block->Name()));
        for (BasicBlock* successor : block->Successors())
        {
            sngxml::dom::Element* successorElement = new sngxml::dom::Element(U"successor");
            successorElement->SetAttribute(U"id", ToUtf32(successor->Name()));
            blockElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(successorElement));
        }
        for (BasicBlock* predecessor : block->Predecessors())
        {
            sngxml::dom::Element* predecessorElement = new sngxml::dom::Element(U"predecessor");
            predecessorElement->SetAttribute(U"id", ToUtf32(predecessor->Name()));
            blockElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(predecessorElement));
        }
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(blockElement));
        block = block->Next();
    }
    return element;
}

void Function::WriteXmlDocument(const std::string& filePath)
{
    sngxml::dom::Element* element = ToXml();
    sngxml::dom::Document document;
    document.AppendChild(std::unique_ptr<sngxml::dom::Node>(element));
    std::ofstream file(filePath); 
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    document.Write(formatter);
}

Code::Code() : context(nullptr), currentFunction(nullptr), functions(this)
{
}

void Code::SetCurrentFunction(Function* function)
{
    currentFunction = function;
}

Function* Code::GetFunction(const std::string& functionId) const
{
    auto it = functionMap.find(functionId);
    if (it != functionMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Function* Code::AddFunctionDefinition(const SourcePos& sourcePos, FunctionType* functionType, const std::string& functionId, bool once, MetadataRef* metadataRef, Context* context)
{
    Function* prev = GetFunction(functionId);
    if (prev)
    {
        if (prev->IsDefined())
        {
            Error("error adding function '" + functionId + "': function id not unique", sourcePos, context, prev->GetSourcePos());
        }
        else
        {
            if (prev->GetType() != functionType)
            {
                Error("error adding function '" + functionId + "': type '" + functionType->Name() + "' conflicts with earlier declaration", sourcePos, context, prev->GetSourcePos());
            }
            prev->SetDefined();
            return prev;
        }
    }
    Function* function = new Function(sourcePos, functionType, functionId, once, true, metadataRef);
    functions.AddChild(function);
    functionMap[function->Name()] = function;
    return function;
}

Function* Code::AddFunctionDeclaration(const SourcePos& sourcePos, FunctionType* functionType, const std::string& functionId)
{
    Function* prev = GetFunction(functionId);
    if (prev)
    {
        if (prev->GetType() != functionType)
        {
            Error("error adding function declaration '" + functionId + "': type '" + functionType->Name() + "' conflicts with earlier declaration", sourcePos, context, prev->GetSourcePos());
        }
        return prev;
    }
    Function* function = new Function(sourcePos, functionType, functionId, false, false, nullptr);
    functions.AddChild(function);
    functionMap[function->Name()] = function;
    return function;
}

void Code::VisitFunctions(Visitor& visitor)
{
    Function* function = FirstFunction();
    while (function)
    {
        function->Accept(visitor);
        function = function->Next();
    }
}

} // cmsx::intermediate
