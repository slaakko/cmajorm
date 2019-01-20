// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_EXPRESSION_INCLUDED
#define CMAJOR_AST_EXPRESSION_INCLUDED
#include <cmajor/ast/Node.hpp>
#include <cmajor/ast/NodeList.hpp>

namespace cmajor { namespace ast {

class DotNode : public UnaryNode
{
public:
    DotNode(const Span& span_);
    DotNode(const Span& span_, Node* subject_, IdentifierNode* memberId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const IdentifierNode* MemberId() const { return memberId.get(); }
    IdentifierNode* MemberId() { return memberId.get(); }
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class ArrowNode : public UnaryNode
{
public:
    ArrowNode(const Span& span_);
    ArrowNode(const Span& span_, Node* subject_, IdentifierNode* memberId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const IdentifierNode* MemberId() const { return memberId.get(); }
    IdentifierNode* MemberId() { return memberId.get(); }
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class EquivalenceNode : public BinaryNode
{
public:
    EquivalenceNode(const Span& span_);
    EquivalenceNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class ImplicationNode : public BinaryNode
{
public:
    ImplicationNode(const Span& span_);
    ImplicationNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class DisjunctionNode : public BinaryNode
{
public:
    DisjunctionNode(const Span& span_);
    DisjunctionNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class ConjunctionNode : public BinaryNode
{
public:
    ConjunctionNode(const Span& span_);
    ConjunctionNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class BitOrNode : public BinaryNode
{
public:
    BitOrNode(const Span& span_);
    BitOrNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class BitXorNode : public BinaryNode
{
public:
    BitXorNode(const Span& span_);
    BitXorNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class BitAndNode : public BinaryNode
{
public:
    BitAndNode(const Span& span_);
    BitAndNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class EqualNode : public BinaryNode
{
public:
    EqualNode(const Span& span_);
    EqualNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class NotEqualNode : public BinaryNode
{
public:
    NotEqualNode(const Span& span_);
    NotEqualNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class LessNode : public BinaryNode
{
public:
    LessNode(const Span& span_);
    LessNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class GreaterNode : public BinaryNode
{
public:
    GreaterNode(const Span& span_);
    GreaterNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class LessOrEqualNode : public BinaryNode
{
public:
    LessOrEqualNode(const Span& span_);
    LessOrEqualNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class GreaterOrEqualNode : public BinaryNode
{
public:
    GreaterOrEqualNode(const Span& span_);
    GreaterOrEqualNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class ShiftLeftNode : public BinaryNode
{
public:
    ShiftLeftNode(const Span& span_);
    ShiftLeftNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class ShiftRightNode : public BinaryNode
{
public:
    ShiftRightNode(const Span& span_);
    ShiftRightNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class AddNode : public BinaryNode
{
public:
    AddNode(const Span& span_);
    AddNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SubNode : public BinaryNode
{
public:
    SubNode(const Span& span_);
    SubNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class MulNode : public BinaryNode
{
public:
    MulNode(const Span& span_);
    MulNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class DivNode : public BinaryNode
{
public:
    DivNode(const Span& span_);
    DivNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class RemNode : public BinaryNode
{
public:
    RemNode(const Span& span_);
    RemNode(const Span& span_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class NotNode : public UnaryNode
{
public:
    NotNode(const Span& span_);
    NotNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class UnaryPlusNode : public UnaryNode
{
public:
    UnaryPlusNode(const Span& span_);
    UnaryPlusNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class UnaryMinusNode : public UnaryNode
{
public:
    UnaryMinusNode(const Span& span_);
    UnaryMinusNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class PrefixIncrementNode : public UnaryNode
{
public:
    PrefixIncrementNode(const Span& span_);
    PrefixIncrementNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class PrefixDecrementNode : public UnaryNode
{
public:
    PrefixDecrementNode(const Span& span_);
    PrefixDecrementNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class ComplementNode : public UnaryNode
{
public:
    ComplementNode(const Span& span_);
    ComplementNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class DerefNode : public UnaryNode
{
public:
    DerefNode(const Span& span_);
    DerefNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class AddrOfNode : public UnaryNode
{
public:
    AddrOfNode(const Span& span_);
    AddrOfNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class IsNode : public Node
{
public:
    IsNode(const Span& span_);
    IsNode(const Span& span_, Node* expr_, Node* targetTypeExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expr() const { return expr.get(); }
    Node* Expr() { return expr.get(); }
    const Node* TargetTypeExpr() const { return targetTypeExpr.get();  }
    Node* TargetTypeExpr() { return targetTypeExpr.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> expr;
    std::unique_ptr<Node> targetTypeExpr;
};

class AsNode : public Node
{
public:
    AsNode(const Span& span_);
    AsNode(const Span& span_, Node* expr_, Node* targetTypeExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expr() const { return expr.get(); }
    Node* Expr() { return expr.get(); }
    const Node* TargetTypeExpr() const { return targetTypeExpr.get(); }
    Node* TargetTypeExpr() { return targetTypeExpr.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> expr;
    std::unique_ptr<Node> targetTypeExpr;
};

class IndexingNode : public Node
{
public:
    IndexingNode(const Span& span_);
    IndexingNode(const Span& span_, Node* subject_, Node* index_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
    const Node* Index() const { return index.get(); }
    Node* Index() { return index.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> subject;
    std::unique_ptr<Node> index;
};

class InvokeNode : public Node
{
public:
    InvokeNode(const Span& span_);
    InvokeNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddArgument(Node* argument) override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
    const NodeList<Node>& Arguments() const { return arguments; }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> subject;
    NodeList<Node> arguments;
};

class PostfixIncrementNode : public UnaryNode
{
public:
    PostfixIncrementNode(const Span& span_);
    PostfixIncrementNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class PostfixDecrementNode : public UnaryNode
{
public:
    PostfixDecrementNode(const Span& span_);
    PostfixDecrementNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SizeOfNode : public Node
{
public:
    SizeOfNode(const Span& span_);
    SizeOfNode(const Span& span_, Node* expression_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> expression;
};

class TypeNameNode : public Node
{
public:
    TypeNameNode(const Span& span_);
    TypeNameNode(const Span& span_, Node* expression_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
    std::string ToString() const override;
    bool Static() const { return static_; }
    void SetStatic() { static_ = true; }
private:
    std::unique_ptr<Node> expression;
    bool static_;
};

class CastNode : public Node
{
public:
    CastNode(const Span& span_);
    CastNode(const Span& span_, Node* targetTypeExpr_, Node* sourceExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* TargetTypeExpr() const { return targetTypeExpr.get(); }
    Node* TargetTypeExpr() { return targetTypeExpr.get(); }
    const Node* SourceExpr() const { return sourceExpr.get(); }
    Node* SourceExpr() { return sourceExpr.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> targetTypeExpr;
    std::unique_ptr<Node> sourceExpr;
};

class ConstructNode : public Node
{
public:
    ConstructNode(const Span& span_);
    ConstructNode(const Span& span_, Node* typeExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddArgument(Node* argument) override;
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const NodeList<Node>& Arguments() const { return arguments; }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> typeExpr;
    NodeList<Node> arguments;
};

class NewNode : public Node
{
public:
    NewNode(const Span& span_);
    NewNode(const Span& span_, Node* typeExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddArgument(Node* argument) override;
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const NodeList<Node>& Arguments() const { return arguments; }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> typeExpr;
    NodeList<Node> arguments;
};

class ThisNode : public Node
{
public:
    ThisNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class BaseNode : public Node
{
public:
    BaseNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class ParenthesizedExpressionNode : public UnaryNode
{
public:
    ParenthesizedExpressionNode(const Span& span_);
    ParenthesizedExpressionNode(const Span& span_, Node* child_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_EXPRESSION_INCLUDED
