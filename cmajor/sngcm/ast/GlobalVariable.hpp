// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_GLOBAL_VARIABLE_INCLUDED
#define SNGCM_AST_GLOBAL_VARIABLE_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/Specifier.hpp>

namespace sngcm {  namespace ast {

class IdentifierNode;
class CompileUnitNode;

class SNGCM_AST_API GlobalVariableNode : public Node
{
public:
    GlobalVariableNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    GlobalVariableNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, CompileUnitNode* cu_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Specifiers GetSpecifiers() const override { return specifiers; }
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    Node* Initializer() { return initializer.get(); }
    void SetInitializer(Node* initializer_);
    CompileUnitNode* CompileUnit() const { return cu; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> initializer;
    CompileUnitNode* cu;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_GLOBAL_VARIABLE_INCLUDED
