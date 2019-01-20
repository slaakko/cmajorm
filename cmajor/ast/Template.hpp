// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_TEMPLATE_INCLUDED
#define CMAJOR_AST_TEMPLATE_INCLUDED
#include <cmajor/ast/Node.hpp>
#include <cmajor/ast/NodeList.hpp>

namespace cmajor { namespace ast {

class IdentifierNode;

class TemplateIdNode : public Node
{
public:
    TemplateIdNode(const Span& span_);
    TemplateIdNode(const Span& span_, Node* primary_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    Node* Primary() { return primary.get(); }
    void AddTemplateArgument(Node* templateArgument);
    const NodeList<Node>& TemplateArguments() const { return templateArguments; }
private:
    std::unique_ptr<Node> primary;
    NodeList<Node> templateArguments;
};

class TemplateParameterNode : public Node
{
public:
    TemplateParameterNode(const Span& span_);
    TemplateParameterNode(const Span& span_, IdentifierNode* id_, Node* defaultTemplateArgument_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const IdentifierNode* Id() const { return id.get(); }
    IdentifierNode* Id() { return id.get(); }
    const Node* DefaultTemplateArgument() const { return defaultTemplateArgument.get(); }
    Node* DefaultTemplateArgument() { return defaultTemplateArgument.get(); }
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> defaultTemplateArgument;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_TEMPLATE_INCLUDED
