// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_GLOBAL_VARIABLE_INCLUDED
#define CMAJOR_AST_GLOBAL_VARIABLE_INCLUDED
#include <cmajor/ast/Node.hpp>
#include <cmajor/ast/Specifier.hpp>

namespace cmajor {  namespace ast {

class IdentifierNode;
class CompileUnitNode;

class AST_API GlobalVariableNode : public Node
{
public:
    GlobalVariableNode(const Span& span_);
    GlobalVariableNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, CompileUnitNode* cu_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    const Node* TypeExpr() const { return typeExpr.get(); }
    Node* TypeExpr() { return typeExpr.get(); }
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    Node* Initializer() { return initializer.get(); }
    void SetInitializer(std::unique_ptr<Node>&& initializer_);
    CompileUnitNode* CompileUnit() const { return cu; }
private:
    Specifiers specifiers;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> initializer;
    CompileUnitNode* cu;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_GLOBAL_VARIABLE_INCLUDED
