// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_CONSTANT_INCLUDED
#define SNGCM_AST_CONSTANT_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/Specifier.hpp>

namespace sngcm { namespace ast {

class IdentifierNode;

class SNGCM_AST_API ConstantNode : public Node
{
public:
    ConstantNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    ConstantNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, Node* value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Specifiers GetSpecifiers() const override { return specifiers; }
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    const Node* Value() const { return value.get(); }
    Node* Value() { return value.get(); }
    void SetStrValue(const std::u32string& strValue_) { strValue = strValue_; }
    const std::u32string& StrValue() const { return strValue; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> value;
    std::u32string strValue;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_CONSTANT_INCLUDED
