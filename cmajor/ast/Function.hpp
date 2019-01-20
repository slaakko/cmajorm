// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_FUNCTION_INCLUDED
#define CMAJOR_AST_FUNCTION_INCLUDED
#include <cmajor/ast/Attribute.hpp>
#include <cmajor/ast/Specifier.hpp>
#include <cmajor/ast/Template.hpp>
#include <cmajor/ast/Concept.hpp>
#include <cmajor/ast/Parameter.hpp>
#include <cmajor/ast/Statement.hpp>
#include <cmajor/ast/Identifier.hpp>

namespace cmajor { namespace ast {

class FunctionNode : public Node
{
public:
    FunctionNode(const Span& span_);
    FunctionNode(NodeType nodeType_, const Span& span_);
    FunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, const std::u32string& groupId_, Attributes* attributes_);
    FunctionNode(NodeType nodeType_, const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, const std::u32string& groupId_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    bool IsFunctionNode() const override { return true; }
    void CloneContent(FunctionNode* clone, CloneContext& cloneContext) const;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddTemplateParameter(TemplateParameterNode* templateParameter) override;
    void AddParameter(ParameterNode* parameter) override;
    void SwitchToBody();
    Specifiers GetSpecifiers() const { return specifiers; }
    const Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    Node* ReturnTypeExpr() { return returnTypeExpr.get(); }
    const std::u32string& GroupId() const { return groupId; }
    const NodeList<TemplateParameterNode>& TemplateParameters() const { return templateParameters; }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
    void SetReturnTypeExpr(Node* returnTypeExpr_);
    const WhereConstraintNode* WhereConstraint() const { return whereConstraint.get(); }
    WhereConstraintNode* WhereConstraint() { return whereConstraint.get(); }
    void SetSpecifiers(Specifiers specifiers_);
    void SetConstraint(WhereConstraintNode* whereConstraint_);
    const CompoundStatementNode* Body() const { return body.get(); }
    CompoundStatementNode* Body() { return body.get(); }
    void SetBody(CompoundStatementNode* body_);
    const CompoundStatementNode* BodySource() const { return bodySource.get(); }
    void SetBodySource(CompoundStatementNode* bodySource_);
    bool IsProgramMain() const { return programMain; }
    void SetProgramMain() { programMain = true; }
    Attributes* GetAttributes() const { return attributes.get(); }
    void SetSpecifierSpan(const Span& specifierSpan_) { specifierSpan = specifierSpan_; }
    const Span& SpecifierSpan() const { return specifierSpan; }
    void SetGroupIdSpan(const Span& groupIdSpan_) { groupIdSpan = groupIdSpan_; }
    const Span& GroupIdSpan() const { return groupIdSpan; }
private:
    Specifiers specifiers;
    Span specifierSpan;
    std::unique_ptr<Node> returnTypeExpr;
    std::u32string groupId;
    Span groupIdSpan;
    NodeList<TemplateParameterNode> templateParameters;
    NodeList<ParameterNode> parameters;
    std::unique_ptr<WhereConstraintNode> whereConstraint;
    std::unique_ptr<CompoundStatementNode> body;
    std::unique_ptr<CompoundStatementNode> bodySource;
    std::unique_ptr<Attributes> attributes;
    bool programMain;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_FUNCTION_INCLUDED
