// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_INSTRUCTION_INCLUDED
#define CMAJOR_CMSX_INSTRUCTION_INCLUDED
#include <cmajor/cmsystemx/Value.hpp>
#include <vector>
#include <memory>

namespace cmsystemx {

const int labelFieldWidth = 60;

class CMSYSTEMX_API Instruction
{
public:
    Instruction(int opCode_);
    virtual ~Instruction();
    Instruction(const Instruction&) = delete;
    Instruction& operator=(const Instruction&) = delete;
    virtual void Write(std::ostream& s);
    void AddOperand(Value* operand);
    virtual void SetLabel(const std::string& label_);
private:
    std::string label;
    int opCode;
    std::vector<Value*> operands;
};

class CMSYSTEMX_API CompositeInstruction : public Instruction
{
public:
    CompositeInstruction();
    CompositeInstruction(const CompositeInstruction&) = delete;
    CompositeInstruction& operator=(const CompositeInstruction&) = delete;
    void Write(std::ostream& s) override;
    void AddInstruction(Instruction* instruction);
    void SetLabel(const std::string& label_) override;
private:
    std::vector<std::unique_ptr<Instruction>> instructions;
};

} // namespace cmsystemx

#endif // CMAJOR_CMSX_INSTRUCTION_INCLUDED
