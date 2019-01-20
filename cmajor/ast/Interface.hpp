// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_INTERFACE_INCLUDED
#define CMAJOR_AST_INTERFACE_INCLUDED
#include <cmajor/ast/Function.hpp>

namespace cmajor { namespace ast {

class InterfaceNode : public Node
{
public:
    InterfaceNode(const Span& span_);
    InterfaceNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    IdentifierNode* Id() const { return id.get(); }
    const NodeList<Node>& Members() const { return members; }
    void AddMember(Node* member);
    Attributes* GetAttributes() const { return attributes.get(); }
    void SetSpecifierSpan(const Span& specifierSpan_) { specifierSpan = specifierSpan_; }
    const Span& SpecifierSpan() const { return specifierSpan; }
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    Specifiers specifiers;
    Span specifierSpan;
    Span beginBraceSpan;
    Span endBraceSpan;
    std::unique_ptr<IdentifierNode> id;
    NodeList<Node> members;
    std::unique_ptr<Attributes> attributes;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_INTERFACE_INCLUDED
