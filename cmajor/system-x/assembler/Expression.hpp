// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_EXPRESSION_INCLUDED
#define CMSX_ASSEMBLER_EXPRESSION_INCLUDED
#include <system-x/assembler/Node.hpp>
#include <memory>

namespace cmsx::assembler {

enum class Operator : int
{
    unaryPlus, unaryMinus, complement, reg, serial, 
    multiply, divide, fractional_divide, modulus, shift_left, shift_right, bitwise_and,
    add, subtract, bitwise_or, bitwise_xor
};

class CMSX_ASSEMBLER_API UnaryExpression : public Node
{
public:
    UnaryExpression(const SourcePos& sourcePos_, Operator op_, Node* operand_);
    Operator Op() const { return op; }
    Node* Operand() const { return operand.get(); }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    Operator op;
    std::unique_ptr<Node> operand;
};

class CMSX_ASSEMBLER_API BinaryExpression : public Node
{
public:
    BinaryExpression(const SourcePos& sourcePos_, Operator op_, Node* left_, Node* right_);
    Operator Op() const { return op; }
    Node* Left() const { return left.get(); }
    Node* Right() const { return right.get(); }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    Operator op;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

class CMSX_ASSEMBLER_API ParenthesizedExpression : public Node
{
public:
    ParenthesizedExpression(const SourcePos& sourcePos_, Node* expr_);
    Node* Expr() const { return expr.get(); }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    std::unique_ptr<Node> expr;
};

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_EXPRESSION_INCLUDED
