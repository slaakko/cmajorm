// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_INSTRUCTION_INCLUDED
#define CMSX_ASSEMBLER_INSTRUCTION_INCLUDED
#include <system-x/assembler/Symbol.hpp>
#include <system-x/assembler/OpCode.hpp>
#include <system-x/object/Symbol.hpp>
#include <vector>
#include <memory>

namespace cmsx::assembler {

class CMSX_ASSEMBLER_API OperandList : public Node
{
public:
    OperandList(const SourcePos& sourcePos_);
    OperandList(const OperandList&) = delete;
    OperandList& operator=(const OperandList&) = delete;
    int OperandCount() const { return operands.size(); }
    Node* GetOperand(int index) const { return operands[index].get(); }
    void AddOperand(Node* operand);
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    std::vector<std::unique_ptr<Node>> operands;
};

class CMSX_ASSEMBLER_API Instruction : public Node
{
public:
    Instruction(const SourcePos& sourcePos_, Symbol* label_, OpCode* opCode_);
    Instruction(int opCode_);
    Instruction(const Instruction&) = delete;
    Instruction& operator=(const Instruction&) = delete;
    Symbol* Label() const { return label.get(); }
    void SetLabel(Symbol* label_);
    OpCode* GetOpCode() const { return opCode.get(); }
    void MakeImmediate();
    OperandList* GetOperandList() const { return operandList.get(); }
    void AddOperand(Node* operand);
    void SetOperands(std::vector<cmsx::object::Value>&& operands_);
    const std::vector<cmsx::object::Value>& Operands() const { return operands; }
    void SetComment(const std::string& comment_);
    const std::string& Comment() const { return comment; }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    std::unique_ptr<Symbol> label;
    std::unique_ptr<OpCode> opCode;
    std::unique_ptr<OperandList> operandList;
    std::string comment;
    std::vector<cmsx::object::Value> operands;
};

CMSX_ASSEMBLER_API Node* MakeLocalRegOperand(uint8_t localRegNumber);
CMSX_ASSEMBLER_API Node* MakeGlobalRegOperand(uint8_t globalRegNumber);

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_INSTRUCTION_INCLUDED
