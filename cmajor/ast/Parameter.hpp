// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_PARAMETER_INCLUDED
#define CMAJOR_AST_PARAMETER_INCLUDED
#include <cmajor/ast/Node.hpp>

namespace cmajor { namespace ast {

class IdentifierNode;

class AST_API ParameterNode : public Node
{
public:
    ParameterNode(const Span& span_);
    ParameterNode(const Span& span_, Node* typeExpr_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    void SetId(IdentifierNode* id_);
    bool ArtificialId() const { return artificialId; }
private:
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    bool artificialId;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_PARAMETER_INCLUDED
