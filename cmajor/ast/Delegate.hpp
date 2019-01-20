// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_DELEGATE_INCLUDED
#define CMAJOR_AST_DELEGATE_INCLUDED
#include <cmajor/ast/Parameter.hpp>
#include <cmajor/ast/NodeList.hpp>
#include <cmajor/ast/Specifier.hpp>

namespace cmajor { namespace ast {

class DelegateNode : public Node
{
public:
    DelegateNode(const Span& span_);
    DelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList<ParameterNode> parameters;
};

class ClassDelegateNode : public Node
{
public:
    ClassDelegateNode(const Span& span_);
    ClassDelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList<ParameterNode> parameters;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_DELEGATE_INCLUDED
