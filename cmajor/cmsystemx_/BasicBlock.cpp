// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/BasicBlock.hpp>
#include <cmajor/cmsystemx/OpCode.hpp>
#include <cmajor/cmsystemx/Context.hpp>
#include <cmajor/util/TextUtils.hpp>

namespace cmsystemx {

using namespace cmajor::util;

BasicBlock::BasicBlock(int id_, const std::string& name_, Function* function_) : id(id_), name(name_), function(function_)
{
}

void BasicBlock::Write(std::ostream& s)
{
    bool first = true;
    for (auto& inst : instructions)
    {
        if (first)
        {
            inst->SetLabel("@" + std::to_string(id));
            first = false;
        }
        inst->Write(s);
    }
}

Instruction* BasicBlock::CreateRetVoid(Context& context)
{
    CompositeInstruction* compositeInstruction = new CompositeInstruction();
    Instruction* setSPInst = new Instruction(SET);
    setSPInst->AddOperand(context.SP());
    setSPInst->AddOperand(context.FP());
    compositeInstruction->AddInstruction(setSPInst);
    Instruction* retInst = new Instruction(RET);
    compositeInstruction->AddInstruction(retInst);
    instructions.push_back(std::unique_ptr<Instruction>(compositeInstruction));
    return compositeInstruction;
}

Instruction* BasicBlock::CreateRet(Value* value, Context& context)
{
    CompositeInstruction* compositeInstruction = new CompositeInstruction();
    Instruction* setRetValInst = new Instruction(SET);
    setRetValInst->AddOperand(context.GetSymbolicRegisters().Ax());
    setRetValInst->AddOperand(value);
    compositeInstruction->AddInstruction(setRetValInst);
    Instruction* setSPInst = new Instruction(SET);
    setSPInst->AddOperand(context.SP());
    setSPInst->AddOperand(context.FP());
    compositeInstruction->AddInstruction(setSPInst);
    Instruction* retInst = new Instruction(RET);
    compositeInstruction->AddInstruction(retInst);
    instructions.push_back(std::unique_ptr<Instruction>(compositeInstruction));
    return compositeInstruction;
}

Instruction* BasicBlock::CreateInstruction(int opCode)
{
    Instruction* inst = new Instruction(opCode);
    instructions.push_back(std::unique_ptr<Instruction>(inst));
    return inst;
}

void BasicBlock::InsertFront(Instruction* instruction)
{
    instructions.emplace(instructions.cbegin(), std::unique_ptr<Instruction>(instruction));
}

} // namesapce cmsystemx
