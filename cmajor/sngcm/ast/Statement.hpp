// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_STATEMENT_INCLUDED
#define SNGCM_AST_STATEMENT_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/NodeList.hpp>

namespace sngcm { namespace ast {

class IdentifierNode;

class SNGCM_AST_API LabelNode : public Node
{
public:
    LabelNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    LabelNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& label_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const std::u32string& Label() const { return label; }
private:
    std::u32string label;
};

class SNGCM_AST_API StatementNode : public Node
{
public:
    StatementNode(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_);
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsStatementNode() const override { return true; }
    virtual bool IsFunctionTerminatingNode() const { return false; }
    virtual bool IsCaseTerminatingNode() const { return false; }
    virtual bool IsDefaultTerminatingNode() const { return false; }
    virtual bool IsBreakEnclosingStatementNode() const { return false; }
    virtual bool IsContinueEnclosingStatementNode() const { return false; }
};

class SNGCM_AST_API LabeledStatementNode : public StatementNode
{
public:
    LabeledStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    LabeledStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, StatementNode* stmt_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const LabelNode* Label() const { return labelNode.get(); }
    LabelNode* Label() { return labelNode.get(); }
    void SetLabelNode(LabelNode* labelNode_);
    StatementNode* Stmt() { return stmt.get(); }
private:
    std::unique_ptr<LabelNode> labelNode;
    std::unique_ptr<StatementNode> stmt;
};

class SNGCM_AST_API CompoundStatementNode : public StatementNode
{
public:
    CompoundStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddStatement(StatementNode* statement);
    NodeList<StatementNode>& Statements() { return statements; }
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    NodeList<StatementNode> statements;
    Span beginBraceSpan;
    Span endBraceSpan;
};

class SNGCM_AST_API ReturnStatementNode : public StatementNode
{
public:
    ReturnStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ReturnStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsFunctionTerminatingNode() const override { return true; }
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsDefaultTerminatingNode() const override { return true; }
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
private:
    std::unique_ptr<Node> expression;
};

class SNGCM_AST_API IfStatementNode : public StatementNode
{
public:
    IfStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    IfStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* condition_, StatementNode* thenS_, StatementNode* elseS_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Condition() const { return condition.get(); }
    Node* Condition() { return condition.get(); }
    const StatementNode* ThenS() const { return thenS.get(); }
    StatementNode* ThenS() { return thenS.get(); }
    const StatementNode* ElseS() const { return elseS.get(); }
    StatementNode* ElseS() { return elseS.get(); }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
    void SetElseSpan(const Span& elseSpan_) { elseSpan = elseSpan_; }
    const Span& ElseSpan() const { return elseSpan; }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<StatementNode> thenS;
    std::unique_ptr<StatementNode> elseS;
    Span leftParenSpan;
    Span rightParenSpan;
    Span elseSpan;
};

class SNGCM_AST_API WhileStatementNode : public StatementNode
{
public:
    WhileStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    WhileStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* condition_, StatementNode* statement_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    const Node* Condition() const { return condition.get(); }
    Node* Condition() { return condition.get(); }
    const StatementNode* Statement() const { return statement.get(); }
    StatementNode* Statement() { return statement.get(); }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
private:
    std::unique_ptr<Node> condition;
    std::unique_ptr<StatementNode> statement;
    Span leftParenSpan;
    Span rightParenSpan;
};

class SNGCM_AST_API DoStatementNode : public StatementNode
{
public:
    DoStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    DoStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, StatementNode* statement_, Node* condition_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    const StatementNode* Statement() const { return statement.get(); }
    StatementNode* Statement() { return statement.get(); }
    const Node* Condition() const { return condition.get(); }
    Node* Condition() { return condition.get(); }
    void SetWhileSpan(const Span& whileSpan_) { whileSpan = whileSpan_; }
    const Span& WhileSpan() const { return whileSpan; }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
private:
    std::unique_ptr<StatementNode> statement;
    std::unique_ptr<Node> condition;
    Span whileSpan;
    Span leftParenSpan;
    Span rightParenSpan;
};

class SNGCM_AST_API ForStatementNode : public StatementNode
{
public:
    ForStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ForStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, StatementNode* initS_, Node* condition_, StatementNode* loopS_, StatementNode* actionS_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    const StatementNode* InitS() const { return initS.get(); }
    StatementNode* InitS() { return initS.get(); }
    const Node* Condition() const { return condition.get(); }
    Node* Condition() { return condition.get(); }
    const StatementNode* LoopS() const { return loopS.get(); }
    StatementNode* LoopS() { return loopS.get(); }
    const StatementNode* ActionS() const { return actionS.get(); }
    StatementNode* ActionS() { return actionS.get(); }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
private:
    std::unique_ptr<StatementNode> initS;
    std::unique_ptr<Node> condition;
    std::unique_ptr<StatementNode> loopS;
    std::unique_ptr<StatementNode> actionS;
    Span leftParenSpan;
    Span rightParenSpan;
};

class SNGCM_AST_API BreakStatementNode : public StatementNode
{
public:
    BreakStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsDefaultTerminatingNode() const override { return true; }
};

class SNGCM_AST_API ContinueStatementNode : public StatementNode
{
public:
    ContinueStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsDefaultTerminatingNode() const override { return true; }
};

class SNGCM_AST_API GotoStatementNode : public StatementNode
{
public:
    GotoStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    GotoStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& target_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const std::u32string& Target() const { return target; }
private:
    std::u32string target;
};

class SNGCM_AST_API ConstructionStatementNode : public StatementNode
{
public:
    ConstructionStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConstructionStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* typeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddArgument(Node* argument) override;
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    const NodeList<Node>& Arguments() const { return arguments; }
    void SetAssignment() { assignment = true; }
    bool Assignment() const { return assignment; }
    void SetEmpty() { empty = true; }
    bool Empty() const { return empty; }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList<Node> arguments;
    bool assignment;
    bool empty;
};

class SNGCM_AST_API DeleteStatementNode : public StatementNode
{
public:
    DeleteStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    DeleteStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
private:
    std::unique_ptr<Node> expression;
};

class SNGCM_AST_API DestroyStatementNode : public StatementNode
{
public:
    DestroyStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    DestroyStatementNode(const Span&, const boost::uuids::uuid& moduleId_span_, Node* expression_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
private:
    std::unique_ptr<Node> expression;
};

class SNGCM_AST_API AssignmentStatementNode : public StatementNode
{
public:
    AssignmentStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AssignmentStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* targetExpr_, Node* sourceExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* TargetExpr() const { return targetExpr.get(); }
    Node* TargetExpr() { return targetExpr.get(); }
    const Node* SourceExpr() const { return sourceExpr.get(); }
    Node* SourceExpr() { return sourceExpr.get(); }
private:
    std::unique_ptr<Node> targetExpr;
    std::unique_ptr<Node> sourceExpr;
};

class SNGCM_AST_API ExpressionStatementNode : public StatementNode
{
public:
    ExpressionStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ExpressionStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
private:
    std::unique_ptr<Node> expression;
};

class SNGCM_AST_API EmptyStatementNode : public StatementNode
{
public:
    EmptyStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
};

class SNGCM_AST_API RangeForStatementNode : public StatementNode
{
public:
    RangeForStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    RangeForStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* typeExpr_, IdentifierNode* id_, Node* container_, StatementNode* action_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsBreakEnclosingStatementNode() const override { return true; }
    bool IsContinueEnclosingStatementNode() const override { return true; }
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    const Node* Container() const { return container.get(); }
    Node* Container() { return container.get(); }
    const StatementNode* Action() const { return action.get(); }
    StatementNode* Action() { return action.get(); }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
    void SetColonSpan(const Span& colonSpan_) { colonSpan = colonSpan_; }
    const Span& ColonSpan() const { return colonSpan; }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> container;
    std::unique_ptr<StatementNode> action;
    Span leftParenSpan;
    Span rightParenSpan;
    Span colonSpan;
};

class CaseStatementNode;
class DefaultStatementNode;

class SNGCM_AST_API SwitchStatementNode : public StatementNode
{
public:
    SwitchStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    SwitchStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* condition_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsBreakEnclosingStatementNode() const override { return true; }
    const Node* Condition() const { return condition.get(); }
    Node* Condition() { return condition.get(); }
    void AddCase(CaseStatementNode* caseS);
    const NodeList<CaseStatementNode>& Cases() const { return cases; }
    void SetDefault(DefaultStatementNode* defaultS_);
    const DefaultStatementNode* Default() const { return defaultS.get(); }
    DefaultStatementNode* Default() { return defaultS.get(); }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    std::unique_ptr<Node> condition;
    NodeList<CaseStatementNode> cases;
    std::unique_ptr<DefaultStatementNode> defaultS;
    Span leftParenSpan;
    Span rightParenSpan;
    Span beginBraceSpan;
    Span endBraceSpan;
};

class SNGCM_AST_API CaseStatementNode : public StatementNode
{
public:
    CaseStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddCaseExpr(Node* caseExpr);
    const NodeList<Node>& CaseExprs() const { return caseExprs; }
    void AddStatement(StatementNode* statement);
    const NodeList<StatementNode>& Statements() const { return statements; }
    void AddCaseSpan(const Span& caseSpan);
    const std::vector<Span>& CaseSpans() const { return caseSpans; }
private:
    NodeList<Node> caseExprs;
    NodeList<StatementNode> statements;
    std::vector<Span> caseSpans;
};

class SNGCM_AST_API DefaultStatementNode : public StatementNode
{
public:
    DefaultStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const NodeList<StatementNode>& Statements() const { return statements; }
    void AddStatement(StatementNode* statement);
private:
    NodeList<StatementNode> statements;
};

class SNGCM_AST_API GotoCaseStatementNode : public StatementNode
{
public:
    GotoCaseStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    GotoCaseStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* caseExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsDefaultTerminatingNode() const override { return true; }
    const Node* CaseExpr() const { return caseExpr.get(); }
    Node* CaseExpr() { return caseExpr.get(); }
private:
    std::unique_ptr<Node> caseExpr;
};

class SNGCM_AST_API GotoDefaultStatementNode : public StatementNode
{
public:
    GotoDefaultStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsCaseTerminatingNode() const override { return true; }
};

class SNGCM_AST_API ThrowStatementNode : public StatementNode
{
public:
    ThrowStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ThrowStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* expression_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsFunctionTerminatingNode() const override { return true; }
    bool IsCaseTerminatingNode() const override { return true; }
    bool IsDefaultTerminatingNode() const override { return true; }
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
private:
    std::unique_ptr<Node> expression;
};

class CatchNode;

class SNGCM_AST_API TryStatementNode : public StatementNode
{
public:
    TryStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    TryStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, CompoundStatementNode* tryBlock_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const CompoundStatementNode* TryBlock() const { return tryBlock.get(); }
    CompoundStatementNode* TryBlock() { return tryBlock.get(); }
    const NodeList<CatchNode>& Catches() const { return catches; }
    void AddCatch(CatchNode* catch_);
private:
    std::unique_ptr<CompoundStatementNode> tryBlock;
    NodeList<CatchNode> catches;
};

class SNGCM_AST_API CatchNode : public Node
{
public:
    CatchNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    CatchNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* typeExpr_, IdentifierNode* id_, CompoundStatementNode* catchBlock_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    const CompoundStatementNode* CatchBlock() const { return catchBlock.get(); }
    CompoundStatementNode* CatchBlock() { return catchBlock.get(); }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<CompoundStatementNode> catchBlock;
    Span leftParenSpan;
    Span rightParenSpan;
};

class SNGCM_AST_API AssertStatementNode : public StatementNode
{
public:
    AssertStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AssertStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* assertExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* AssertExpr() const { return assertExpr.get(); }
    Node* AssertExpr() { return assertExpr.get(); }
private:
    std::unique_ptr<Node> assertExpr;
};

class SNGCM_AST_API ConditionalCompilationExpressionNode : public Node
{
public:
    ConditionalCompilationExpressionNode(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_);
    bool IsConditionalCompilationExpressionNode() const override { return true; }
};

class SNGCM_AST_API ConditionalCompilationBinaryExpressionNode : public ConditionalCompilationExpressionNode
{
public:
    ConditionalCompilationBinaryExpressionNode(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_);
    ConditionalCompilationBinaryExpressionNode(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* left_, ConditionalCompilationExpressionNode* right_);
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    ConditionalCompilationExpressionNode* Left() const { return left.get(); }
    ConditionalCompilationExpressionNode* Right() const { return right.get(); }
private:
    std::unique_ptr<ConditionalCompilationExpressionNode> left;
    std::unique_ptr<ConditionalCompilationExpressionNode> right;
};

class SNGCM_AST_API ConditionalCompilationDisjunctionNode : public ConditionalCompilationBinaryExpressionNode
{
public:
    ConditionalCompilationDisjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConditionalCompilationDisjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* left_, ConditionalCompilationExpressionNode* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
};

class SNGCM_AST_API ConditionalCompilationConjunctionNode : public ConditionalCompilationBinaryExpressionNode
{
public:
    ConditionalCompilationConjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConditionalCompilationConjunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* left_, ConditionalCompilationExpressionNode* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
};

class SNGCM_AST_API ConditionalCompilationNotNode : public ConditionalCompilationExpressionNode
{
public:
    ConditionalCompilationNotNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConditionalCompilationNotNode(const Span& span_, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* expr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    ConditionalCompilationExpressionNode* Expr() const { return expr.get(); }
private:
    std::unique_ptr<ConditionalCompilationExpressionNode> expr;
};

class SNGCM_AST_API ConditionalCompilationPrimaryNode : public ConditionalCompilationExpressionNode
{
public:
    ConditionalCompilationPrimaryNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConditionalCompilationPrimaryNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& symbol_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const std::u32string& Symbol() const { return symbol; }
private:
    std::u32string symbol;
};

class SNGCM_AST_API ParenthesizedConditionalCompilationExpressionNode : public ConditionalCompilationExpressionNode
{
public:
    ParenthesizedConditionalCompilationExpressionNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ParenthesizedConditionalCompilationExpressionNode(const Span& span_, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* expr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    ConditionalCompilationExpressionNode* Expr() const { return expr.get(); }
private:
    std::unique_ptr<ConditionalCompilationExpressionNode> expr;
};

class SNGCM_AST_API ConditionalCompilationPartNode : public Node
{
public:
    ConditionalCompilationPartNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConditionalCompilationPartNode(const Span& span_, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* expr_);
    void AddStatement(StatementNode* statement);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    ConditionalCompilationExpressionNode* Expr() const { return expr.get();  }
    const NodeList<StatementNode>& Statements() const { return statements; }
    void SetKeywordSpan(const Span& keywordSpan_) { keywordSpan = keywordSpan_; }
    const Span& KeywordSpan() const { return keywordSpan; }
    void SetLeftParenSpan(const Span& leftParenSpan_) { leftParenSpan = leftParenSpan_; }
    const Span& LeftParenSpan() const { return leftParenSpan; }
    void SetRightParenSpan(const Span& rightParenSpan_) { rightParenSpan = rightParenSpan_; }
    const Span& RightParenSpan() const { return rightParenSpan; }
private:
    std::unique_ptr<ConditionalCompilationExpressionNode> expr;
    NodeList<StatementNode> statements;
    Span keywordSpan;
    Span leftParenSpan;
    Span rightParenSpan;
};

class SNGCM_AST_API ConditionalCompilationStatementNode : public StatementNode
{
public:
    ConditionalCompilationStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConditionalCompilationStatementNode(const Span& span_, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* ifExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddIfStatement(StatementNode* statement);
    void AddElifExpr(const Span& span, const boost::uuids::uuid& moduleId_, ConditionalCompilationExpressionNode* expr);
    void AddElifStatement(StatementNode* statement);
    void AddElseStatement(const Span& span, const boost::uuids::uuid& moduleId_, StatementNode* statement);
    ConditionalCompilationPartNode* IfPart() { return ifPart.get(); }
    const NodeList<ConditionalCompilationPartNode>& ElifParts() const { return elifParts; }
    ConditionalCompilationPartNode* ElsePart() { return elsePart.get(); }
    void SetElifLeftParenSpan(const Span& span);
    void SetElifRightParenSpan(const Span& span);
    void SetElifKeywordSpan(const Span& span);
    void SetEndIfSpan(const Span& endifSpan_) { endifSpan = endifSpan_; }
    const Span& EndIfSpan() const { return endifSpan; }
    void SetIfPart(ConditionalCompilationPartNode* ifPart_);
    void AddElifPart(ConditionalCompilationPartNode* elifPart);
    void SetElsePart(ConditionalCompilationPartNode* elsePart_);
private:
    std::unique_ptr<ConditionalCompilationPartNode> ifPart;
    NodeList<ConditionalCompilationPartNode> elifParts;
    std::unique_ptr<ConditionalCompilationPartNode> elsePart;
    Span endifSpan;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_STATEMENT_INCLUDED
