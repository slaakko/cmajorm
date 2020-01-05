// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_NAMESPACE_INCLUDED
#define SNGCM_AST_NAMESPACE_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/NodeList.hpp>

namespace sngcm { namespace ast {

class IdentifierNode;

class SNGCM_AST_API NamespaceNode : public Node
{
public:
    NamespaceNode(const Span& span_);
    NamespaceNode(const Span& span_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddMember(Node* member);
    IdentifierNode* Id() const;
    NodeList<Node>& Members() { return members; }
    const NodeList<Node>& Members() const { return members; }
private:
    std::unique_ptr<IdentifierNode> id;
    NodeList<Node> members;
};

class SNGCM_AST_API AliasNode : public Node
{
public:
    AliasNode(const Span& span_);
    AliasNode(const Span& span_, IdentifierNode* id_, IdentifierNode* qid_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    IdentifierNode* Id() const;
    IdentifierNode* Qid() const;
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<IdentifierNode> qid;
};

class SNGCM_AST_API NamespaceImportNode : public Node
{
public:
    NamespaceImportNode(const Span& span_);
    NamespaceImportNode(const Span& span_, IdentifierNode* ns_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    IdentifierNode* Ns() const;
private:
    std::unique_ptr<IdentifierNode> ns;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_NAMESPACE_INCLUDED
