// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_DELEGATE_INCLUDED
#define SNGCM_AST_DELEGATE_INCLUDED
#include <sngcm/ast/Parameter.hpp>
#include <sngcm/ast/NodeList.hpp>
#include <sngcm/ast/Specifier.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API DelegateNode : public Node
{
public:
    DelegateNode(const Span& span_);
    DelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    Specifiers GetSpecifiers() const override { return specifiers; }
    Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList<ParameterNode> parameters;
};

class SNGCM_AST_API ClassDelegateNode : public Node
{
public:
    ClassDelegateNode(const Span& span_);
    ClassDelegateNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddParameter(ParameterNode* parameter) override;
    Specifiers GetSpecifiers() const override { return specifiers; }
    Node* ReturnTypeExpr() const { return returnTypeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    const NodeList<ParameterNode>& Parameters() const { return parameters; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> returnTypeExpr;
    std::unique_ptr<IdentifierNode> id;
    NodeList<ParameterNode> parameters;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_DELEGATE_INCLUDED
