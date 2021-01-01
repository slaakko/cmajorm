// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_TEMPLATE_INCLUDED
#define SNGCM_AST_TEMPLATE_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/NodeList.hpp>

namespace sngcm { namespace ast {

class IdentifierNode;

class SNGCM_AST_API TemplateIdNode : public Node
{
public:
    TemplateIdNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    TemplateIdNode(const Span& span_, const boost::uuids::uuid& moduleId_, Node* primary_);
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

class SNGCM_AST_API TemplateParameterNode : public Node
{
public:
    TemplateParameterNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    TemplateParameterNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* id_, Node* defaultTemplateArgument_);
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

} } // namespace sngcm::ast

#endif // SNGCM_AST_TEMPLATE_INCLUDED
