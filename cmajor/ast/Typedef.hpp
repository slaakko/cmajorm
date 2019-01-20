// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_TYPEDEF_INCLUDED
#define CMAJOR_AST_TYPEDEF_INCLUDED
#include <cmajor/ast/Node.hpp>
#include <cmajor/ast/Specifier.hpp>

namespace cmajor { namespace ast {

class IdentifierNode;

class TypedefNode : public Node
{
public:
    TypedefNode(const Span& span_);
    TypedefNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_TYPEDEF_INCLUDED
