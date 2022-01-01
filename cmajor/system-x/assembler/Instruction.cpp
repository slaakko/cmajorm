// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Instruction.hpp>
#include <system-x/assembler/Constant.hpp>
#include <system-x/assembler/Expression.hpp>
#include <system-x/assembler/Visitor.hpp>
#include <system-x/machine/Registers.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmsx::assembler {

using namespace soulng::util;

OperandList::OperandList(const SourcePos& sourcePos_) : 
    Node(NodeKind::operandListNode, sourcePos_)
{
}

void OperandList::AddOperand(Node* operand)
{
    operands.push_back(std::unique_ptr<Node>(operand));
}

void OperandList::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void OperandList::Write(CodeFormatter& formatter)
{
    int n = OperandCount();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            formatter.Write(",");
        }
        Node* operand = GetOperand(i);
        operand->Write(formatter);
    }
}

Instruction::Instruction(const SourcePos& sourcePos_, Symbol* label_, OpCode* opCode_) :
    Node(NodeKind::instructionNode, sourcePos_), label(label_), opCode(opCode_), operandList(new OperandList(sourcePos_))
{
}

Instruction::Instruction(int opCode_) : Node(NodeKind::instructionNode, SourcePos()), label(), opCode(new OpCode(opCode_)), operandList(new OperandList(SourcePos()))
{
}

void Instruction::SetLabel(Symbol* label_)
{
    label.reset(label_);
}

void Instruction::MakeImmediate()
{
    opCode->MakeImmediate();
}

void Instruction::AddOperand(Node* operand)
{
    operandList->AddOperand(operand);
}

void Instruction::SetOperands(std::vector<cmsx::object::Value>&& operands_)
{
    operands = std::move(operands_);
}

void Instruction::SetComment(const std::string& comment_)
{
    comment = comment_;
}

void Instruction::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void Instruction::Write(CodeFormatter& formatter)
{
    bool indentDecremented = false;
    if (label)
    {
        formatter.DecIndent();
        formatter.Write(Format(label->Name(), formatter.IndentSize() - 1, FormatWidth::min) + " ");
        indentDecremented = true;
    }
    opCode->Write(formatter);
    if (operandList->OperandCount() > 0)
    {
        formatter.Write(" ");
        operandList->Write(formatter);
    }
    if (!comment.empty())
    {
        formatter.Write(" // ");
        formatter.Write(comment);
    }
    formatter.WriteLine();
    if (indentDecremented)
    {
        formatter.IncIndent();
    }
}

Node* MakeLocalRegOperand(uint8_t localRegNumber)
{
    return new UnaryExpression(SourcePos(), Operator::reg, new DecimalConstant(SourcePos(), localRegNumber));
}

Node* MakeGlobalRegOperand(uint8_t globalRegNumber)
{
    return new GlobalSymbol(SourcePos(), cmsx::machine::GetGlobalRegName(globalRegNumber));
}

} // namespace cmsx::assembler
