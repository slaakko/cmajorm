// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_TYPEDEF_INCLUDED
#define SNGCM_AST_TYPEDEF_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/Specifier.hpp>

namespace sngcm { namespace ast {

class IdentifierNode;

class SNGCM_AST_API TypedefNode : public Node
{
public:
    TypedefNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    TypedefNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Specifiers GetSpecifiers() const override { return specifiers; }
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_TYPEDEF_INCLUDED
