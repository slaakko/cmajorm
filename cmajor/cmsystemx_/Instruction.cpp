// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/Instruction.hpp>
#include <cmajor/cmsystemx/OpCode.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/Error.hpp>

namespace cmsystemx {

using namespace cmajor::util;

Instruction::Instruction(int opCode_) : opCode(opCode_)
{
}

Instruction::~Instruction()
{
}

void Instruction::Write(std::ostream& s)
{
    std::string labelField = Format(label, labelFieldWidth, FormatWidth::min);
    s << labelField << " " << Format(GetOpCodeName(opCode), GetMaxOpCodeNameLength());
    bool first = true;
    for (Value* operand : operands)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s << ",";
        }
        operand->Write(s);
    }
    s << std::endl;
}

void Instruction::SetLabel(const std::string& label_)
{
    label = label_;
}

void Instruction::AddOperand(Value* operand)
{
    operands.push_back(operand);
}

CompositeInstruction::CompositeInstruction() : Instruction(-1)
{
}

void CompositeInstruction::Write(std::ostream& s)
{
    for (const auto& inst : instructions)
    {
        inst->Write(s);
    }
}

void CompositeInstruction::SetLabel(const std::string& label_)
{
    Assert(!instructions.empty(), "composite instructions empty");
    instructions.front()->SetLabel(label_);
}

void CompositeInstruction::AddInstruction(Instruction* instruction)
{
    instructions.push_back(std::unique_ptr<Instruction>(instruction));
}

} // namespace cmsystemx
