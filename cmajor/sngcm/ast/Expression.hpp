// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_EXPRESSION_INCLUDED
#define SNGCM_AST_EXPRESSION_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/NodeList.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API DotNode : public UnaryNode
{
public:
    DotNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    DotNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_, IdentifierNode* memberId_);
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

class SNGCM_AST_API ArrowNode : public UnaryNode
{
public:
    ArrowNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ArrowNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_, IdentifierNode* memberId_);
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

class SNGCM_AST_API EquivalenceNode : public BinaryNode
{
public:
    EquivalenceNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    EquivalenceNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API ImplicationNode : public BinaryNode
{
public:
    ImplicationNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ImplicationNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API DisjunctionNode : public BinaryNode
{
public:
    DisjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    DisjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API ConjunctionNode : public BinaryNode
{
public:
    ConjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API BitOrNode : public BinaryNode
{
public:
    BitOrNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    BitOrNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API BitXorNode : public BinaryNode
{
public:
    BitXorNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    BitXorNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API BitAndNode : public BinaryNode
{
public:
    BitAndNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    BitAndNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API EqualNode : public BinaryNode
{
public:
    EqualNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    EqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API NotEqualNode : public BinaryNode
{
public:
    NotEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    NotEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API LessNode : public BinaryNode
{
public:
    LessNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    LessNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API GreaterNode : public BinaryNode
{
public:
    GreaterNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    GreaterNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API LessOrEqualNode : public BinaryNode
{
public:
    LessOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    LessOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API GreaterOrEqualNode : public BinaryNode
{
public:
    GreaterOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    GreaterOrEqualNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API ShiftLeftNode : public BinaryNode
{
public:
    ShiftLeftNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ShiftLeftNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API ShiftRightNode : public BinaryNode
{
public:
    ShiftRightNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ShiftRightNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API AddNode : public BinaryNode
{
public:
    AddNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AddNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API SubNode : public BinaryNode
{
public:
    SubNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    SubNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API MulNode : public BinaryNode
{
public:
    MulNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    MulNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API DivNode : public BinaryNode
{
public:
    DivNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    DivNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API RemNode : public BinaryNode
{
public:
    RemNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    RemNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API NotNode : public UnaryNode
{
public:
    NotNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    NotNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API UnaryPlusNode : public UnaryNode
{
public:
    UnaryPlusNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    UnaryPlusNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API UnaryMinusNode : public UnaryNode
{
public:
    UnaryMinusNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    UnaryMinusNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API PrefixIncrementNode : public UnaryNode
{
public:
    PrefixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    PrefixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API PrefixDecrementNode : public UnaryNode
{
public:
    PrefixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    PrefixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API ComplementNode : public UnaryNode
{
public:
    ComplementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ComplementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API DerefNode : public UnaryNode
{
public:
    DerefNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    DerefNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API AddrOfNode : public UnaryNode
{
public:
    AddrOfNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AddrOfNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API IsNode : public Node
{
public:
    IsNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    IsNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expr_, Node* targetTypeExpr_);
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

class SNGCM_AST_API AsNode : public Node
{
public:
    AsNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AsNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expr_, Node* targetTypeExpr_);
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

class SNGCM_AST_API IndexingNode : public Node
{
public:
    IndexingNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    IndexingNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_, Node* index_);
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

class SNGCM_AST_API InvokeNode : public Node
{
public:
    InvokeNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    InvokeNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
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

class SNGCM_AST_API PostfixIncrementNode : public UnaryNode
{
public:
    PostfixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    PostfixIncrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API PostfixDecrementNode : public UnaryNode
{
public:
    PostfixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    PostfixDecrementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API SizeOfNode : public Node
{
public:
    SizeOfNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    SizeOfNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_);
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

class SNGCM_AST_API TypeNameNode : public Node
{
public:
    TypeNameNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    TypeNameNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_);
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

class SNGCM_AST_API TypeIdNode : public Node
{
public:
    TypeIdNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    TypeIdNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_);
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

class SNGCM_AST_API CastNode : public Node
{
public:
    CastNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    CastNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* targetTypeExpr_, Node* sourceExpr_);
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

class SNGCM_AST_API ConstructNode : public Node
{
public:
    ConstructNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConstructNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* typeExpr_);
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

class SNGCM_AST_API NewNode : public Node
{
public:
    NewNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    NewNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* typeExpr_);
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

class SNGCM_AST_API ThisNode : public Node
{
public:
    ThisNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API BaseNode : public Node
{
public:
    BaseNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API ParenthesizedExpressionNode : public UnaryNode
{
public:
    ParenthesizedExpressionNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ParenthesizedExpressionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* child_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_EXPRESSION_INCLUDED
