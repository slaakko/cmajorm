// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_CONCEPT_INCLUDED
#define SNGCM_AST_CONCEPT_INCLUDED
#include <sngcm/ast/Parameter.hpp>
#include <sngcm/ast/NodeList.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API ConstraintNode : public Node
{
public:
    ConstraintNode(NodeType nodeType_, const Span& span_);
    bool NodeIsConstraintNode() const override { return true; }
    virtual bool IsHeaderConstraint() const { return false; }
};

class SNGCM_AST_API ParenthesizedConstraintNode : public ConstraintNode
{
public:
    ParenthesizedConstraintNode(const Span& span_);
    ParenthesizedConstraintNode(const Span& span_, ConstraintNode* constraint_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const ConstraintNode* Constraint() const { return constraint.get(); }
    ConstraintNode* Constraint() { return constraint.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<ConstraintNode> constraint;
};

class SNGCM_AST_API BinaryConstraintNode : public ConstraintNode
{
public:
    BinaryConstraintNode(NodeType nodeType_, const Span& span_);
    BinaryConstraintNode(NodeType nodeType_, const Span& span_, ConstraintNode* left_, ConstraintNode* right_);
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const ConstraintNode* Left() const { return left.get(); }
    ConstraintNode* Left() { return left.get(); }
    const ConstraintNode* Right() const { return right.get(); }
    ConstraintNode* Right() { return right.get(); }
private:
    std::unique_ptr<ConstraintNode> left;
    std::unique_ptr<ConstraintNode> right;
};

class SNGCM_AST_API DisjunctiveConstraintNode : public BinaryConstraintNode
{
public:
    DisjunctiveConstraintNode(const Span& span_);
    DisjunctiveConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API ConjunctiveConstraintNode : public BinaryConstraintNode
{
public:
    ConjunctiveConstraintNode(const Span& span_);
    ConjunctiveConstraintNode(const Span& span_, ConstraintNode* left_, ConstraintNode* right_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override;
};

class SNGCM_AST_API WhereConstraintNode : public ConstraintNode
{
public:
    WhereConstraintNode(const Span& span_);
    WhereConstraintNode(const Span& span_, ConstraintNode* constraint_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const ConstraintNode* Constraint() const { return constraint.get(); }
    ConstraintNode* Constraint() { return constraint.get(); }
    std::string ToString() const override;
    void SetHeaderConstraint() { headerConstraint = true; }
    bool IsHeaderConstraint() const override { return headerConstraint; }
    void SetSemicolon() { semicolon = true; }
    bool Semicolon() const { return semicolon; }
private:
    std::unique_ptr<ConstraintNode> constraint;
    bool headerConstraint;
    bool semicolon;
};

class SNGCM_AST_API PredicateConstraintNode : public ConstraintNode
{
public:
    PredicateConstraintNode(const Span& span_);
    PredicateConstraintNode(const Span& span_, Node* invokeExpr_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* InvokeExpr() const { return invokeExpr.get(); }
    Node* InvokeExpr() { return invokeExpr.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> invokeExpr;
};

class SNGCM_AST_API IsConstraintNode : public ConstraintNode
{
public:
    IsConstraintNode(const Span& span_);
    IsConstraintNode(const Span& span_, Node* typeExpr_, Node* conceptOrTypeName_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const Node* ConceptOrTypeName() const { return conceptOrTypeName.get(); }
    Node* ConceptOrTypeName() { return conceptOrTypeName.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<Node> conceptOrTypeName;
};

class SNGCM_AST_API MultiParamConstraintNode : public ConstraintNode
{
public:
    MultiParamConstraintNode(const Span& span_);
    MultiParamConstraintNode(const Span& span_, IdentifierNode* conceptId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const IdentifierNode* ConceptId() const { return conceptId.get(); }
    IdentifierNode* ConceptId() { return conceptId.get(); }
    const NodeList<Node>& TypeExprs() const { return typeExprs; }
    void AddTypeExpr(Node* typeExpr);
    std::string ToString() const override;
private:
    std::unique_ptr<IdentifierNode> conceptId;
    NodeList<Node> typeExprs;
};

class SNGCM_AST_API TypeNameConstraintNode : public ConstraintNode
{
public:
    TypeNameConstraintNode(const Span& span_);
    TypeNameConstraintNode(const Span& span_, Node* typeId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* TypeId() const { return typeId.get(); }
    Node* TypeId() { return typeId.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> typeId;
};

class SNGCM_AST_API SignatureConstraintNode : public ConstraintNode
{
public:
    SignatureConstraintNode(NodeType nodeType_, const Span& span_);
};

class SNGCM_AST_API ConstructorConstraintNode : public SignatureConstraintNode
{
public:
    ConstructorConstraintNode(const Span& span_);
    ConstructorConstraintNode(const Span& span_, IdentifierNode* typeParamId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    const IdentifierNode* TypeParamId() const { return typeParamId.get(); }
    IdentifierNode* TypeParamId() { return typeParamId.get(); }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
    std::string ToString() const override;
private:
    std::unique_ptr<IdentifierNode> typeParamId;
    NodeList<ParameterNode> parameters;
};

class SNGCM_AST_API DestructorConstraintNode : public SignatureConstraintNode
{
public:
    DestructorConstraintNode(const Span& span_);
    DestructorConstraintNode(const Span& span_, IdentifierNode* typeParamId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    IdentifierNode* TypeParamId() { return typeParamId.get(); }
    std::string ToString() const override;
private:
    std::unique_ptr<IdentifierNode> typeParamId;
};

class SNGCM_AST_API MemberFunctionConstraintNode : public SignatureConstraintNode
{
public:
    MemberFunctionConstraintNode(const Span& span_);
    MemberFunctionConstraintNode(const Span& span_, Node* returnTypeExpr_, IdentifierNode* typeParamId_, const std::u32string& groupId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    const Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    Node* ReturnTypeExpr() { return returnTypeExpr.get(); }
    const IdentifierNode* TypeParamId() const { return typeParamId.get(); }
    IdentifierNode* TypeParamId() { return typeParamId.get(); }
    const std::u32string& GroupId() const { return groupId; }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> typeParamId;
    std::u32string groupId;
    NodeList<ParameterNode> parameters;
};

class SNGCM_AST_API FunctionConstraintNode : public SignatureConstraintNode
{
public:
    FunctionConstraintNode(const Span& span_);
    FunctionConstraintNode(const Span& span_, Node* returnTypeExpr_, const std::u32string& groupId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    const Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    Node* ReturnTypeExpr() { return returnTypeExpr.get(); }
    const std::u32string& GroupId() const { return groupId; }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> returnTypeExpr;
    std::u32string groupId;
    NodeList<ParameterNode> parameters;
};

class SNGCM_AST_API AxiomStatementNode : public Node
{
public:
    AxiomStatementNode(const Span& span_);
    AxiomStatementNode(const Span& span_, Node* expression_, const std::u32string& text_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const Node* Expression() const { return expression.get(); }
    Node* Expression() { return expression.get(); }
    const std::u32string& Text() const { return text; }
    std::string ToString() const override;
private:
    std::unique_ptr<Node> expression;
    std::u32string text;
};

class SNGCM_AST_API AxiomNode : public Node
{
public:
    AxiomNode(const Span& span_);
    AxiomNode(const Span& span_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    void AddStatement(AxiomStatementNode* statement);
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
    const NodeList<AxiomStatementNode>& Statements() const { return statements; }
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    Span beginBraceSpan;
    Span endBraceSpan;
    std::unique_ptr<IdentifierNode> id;
    NodeList<ParameterNode> parameters;
    NodeList<AxiomStatementNode> statements;
};

class SNGCM_AST_API ConceptIdNode : public Node
{
public:
    ConceptIdNode(const Span& span_);
    ConceptIdNode(const Span& span_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    const NodeList<Node>& TypeParameters() const { return typeParameters; }
    void AddTypeParameter(Node* typeParameter);
    int Arity() const { return typeParameters.Count(); }
    std::string ToString() const override;
private:
    std::unique_ptr<IdentifierNode> id;
    NodeList<Node> typeParameters;
};

class SNGCM_AST_API ConceptNode : public Node
{
public:
    ConceptNode(const Span& span_);
    ConceptNode(NodeType nodeType_, const Span& span_);
    ConceptNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_);
    ConceptNode(NodeType nodeType_, const Span& span_, Specifiers specifiers_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    bool IsConceptNode() const override { return true; }
    virtual bool IsCommonConceptNode() const { return false; }
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    int Arity() const { return typeParameters.Count(); }
    const NodeList<IdentifierNode>& TypeParameters() const { return typeParameters; }
    void AddTypeParameter(IdentifierNode* typeParameter);
    void SetRefinement(ConceptIdNode* refinement_);
    ConceptIdNode* Refinement() { return refinement.get(); }
    void AddConstraint(ConstraintNode* constraint);
    const NodeList<ConstraintNode>& Constraints() const { return constraints; }
    void AddAxiom(AxiomNode* axiom_);
    const NodeList<AxiomNode>& Axioms() const { return axioms; }
    Specifiers GetSpecifiers() const override { return specifiers; }
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    Specifiers specifiers;
    Span beginBraceSpan;
    Span endBraceSpan;
    std::unique_ptr<IdentifierNode> id;
    NodeList<IdentifierNode> typeParameters;
    std::unique_ptr<ConceptIdNode> refinement;
    NodeList<ConstraintNode> constraints;
    NodeList<AxiomNode> axioms;
};

class SNGCM_AST_API IntrinsicConstraintNode : public ConstraintNode
{
public:
    IntrinsicConstraintNode(NodeType nodeType_);
};

class SNGCM_AST_API SameConstraintNode : public IntrinsicConstraintNode
{
public:
    SameConstraintNode();
    SameConstraintNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class SNGCM_AST_API DerivedConstraintNode : public IntrinsicConstraintNode
{
public:
    DerivedConstraintNode();
    DerivedConstraintNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class SNGCM_AST_API ConvertibleConstraintNode : public IntrinsicConstraintNode
{
public:
    ConvertibleConstraintNode();
    ConvertibleConstraintNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class SNGCM_AST_API ExplicitlyConvertibleConstraintNode : public IntrinsicConstraintNode
{
public:
    ExplicitlyConvertibleConstraintNode();
    ExplicitlyConvertibleConstraintNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class SNGCM_AST_API CommonConstraintNode : public IntrinsicConstraintNode
{
public:
    CommonConstraintNode();
    CommonConstraintNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class SNGCM_AST_API NonreferenceTypeConstraintNode : public IntrinsicConstraintNode
{
public:
    NonreferenceTypeConstraintNode();
    NonreferenceTypeConstraintNode(const Span& span_);
    void Accept(Visitor& visitor) override;
    Node* Clone(CloneContext& cloneContext) const override;
};

class SNGCM_AST_API SameConceptNode : public ConceptNode
{
public:
    SameConceptNode();
    SameConceptNode(const Span& span_);
    bool IsIntrinsicConceptNode() const override { return true; }
};

class SNGCM_AST_API DerivedConceptNode : public ConceptNode
{
public:
    DerivedConceptNode();
    DerivedConceptNode(const Span& span_);
    bool IsIntrinsicConceptNode() const override { return true; }
};

class SNGCM_AST_API ConvertibleConceptNode : public ConceptNode
{
public:
    ConvertibleConceptNode();
    ConvertibleConceptNode(const Span& span_);
    bool IsIntrinsicConceptNode() const override { return true; }
};

class SNGCM_AST_API ExplicitlyConvertibleConceptNode : public ConceptNode
{
public:
    ExplicitlyConvertibleConceptNode();
    ExplicitlyConvertibleConceptNode(const Span& span_);
    bool IsIntrinsicConceptNode() const override { return true; }
};

class SNGCM_AST_API CommonConceptNode : public ConceptNode
{
public:
    CommonConceptNode();
    CommonConceptNode(const Span& span_);
    bool IsCommonConceptNode() const override { return true; }
    bool IsIntrinsicConceptNode() const override { return true; }
};

class SNGCM_AST_API NonreferenceTypeConceptNode : public ConceptNode
{
public:
    NonreferenceTypeConceptNode();
    NonreferenceTypeConceptNode(const Span& span_);
    bool IsIntrinsicConceptNode() const override { return true; }
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_CONCEPT_INCLUDED
