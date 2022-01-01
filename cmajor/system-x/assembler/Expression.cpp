// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Expression.hpp>
#include <system-x/assembler/Visitor.hpp>

namespace cmsx::assembler {

UnaryExpression::UnaryExpression(const SourcePos& sourcePos_, Operator op_, Node* operand_) : 
    Node(NodeKind::unaryExprNode, sourcePos_), op(op_), operand(operand_)
{
}

void UnaryExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UnaryExpression::Write(CodeFormatter& formatter)
{
    switch (op)
    {
        case Operator::unaryPlus: formatter.Write("+"); break;
        case Operator::unaryMinus: formatter.Write("-"); break;
        case Operator::complement: formatter.Write("~"); break;
        case Operator::reg: formatter.Write("$"); break;
    }
    operand->Write(formatter);
}

BinaryExpression::BinaryExpression(const SourcePos& sourcePos_, Operator op_, Node* left_, Node* right_) : 
    Node(NodeKind::binaryExprNode, sourcePos_), op(op_), left(left_), right(right_)
{
}

void BinaryExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BinaryExpression::Write(CodeFormatter& formatter)
{
    left->Write(formatter);
    switch (op)
    {
        case Operator::multiply: formatter.Write("*"); break;
        case Operator::divide: formatter.Write("/"); break;
        case Operator::fractional_divide: formatter.Write("//"); break;
        case Operator::modulus: formatter.Write("%"); break;
        case Operator::shift_left: formatter.Write("<<"); break;
        case Operator::shift_right: formatter.Write(">>"); break;
        case Operator::bitwise_and: formatter.Write("&"); break;
        case Operator::add: formatter.Write("+"); break;
        case Operator::subtract: formatter.Write("-"); break;
        case Operator::bitwise_or: formatter.Write("|"); break;
        case Operator::bitwise_xor: formatter.Write("^"); break;
    }
    right->Write(formatter);
}

ParenthesizedExpression::ParenthesizedExpression(const SourcePos& sourcePos_, Node* expr_) : 
    Node(NodeKind::parenExprNode, sourcePos_), expr(expr_)
{
}

void ParenthesizedExpression::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ParenthesizedExpression::Write(CodeFormatter& formatter)
{
    formatter.Write("(");
    expr->Write(formatter);
    formatter.Write(")");
}

} // namespace cmsx::assembler
