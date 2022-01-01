// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_NAMESPACE_INCLUDED
#define SNGCM_AST_NAMESPACE_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/NodeList.hpp>

namespace sngcm { namespace ast {

class IdentifierNode;

enum class NsFlags : int8_t
{
    none = 0, isUnnamedNs = 1 << 0, hasUnnamedNs = 1 << 1
};

inline constexpr NsFlags operator|(NsFlags left, NsFlags right)
{
    return static_cast<NsFlags>(static_cast<int8_t>(left) | static_cast<int8_t>(right));
}

inline constexpr NsFlags operator&(NsFlags left, NsFlags right)
{
    return static_cast<NsFlags>(static_cast<int8_t>(left) & static_cast<int8_t>(right));
}

class SNGCM_AST_API NamespaceNode : public Node
{
public:
    NamespaceNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    NamespaceNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddMember(Node* member);
    IdentifierNode* Id() const;
    NodeList<Node>& Members() { return members; }
    const NodeList<Node>& Members() const { return members; }
    bool IsUnnamedNs() const { return (flags & NsFlags::isUnnamedNs) != NsFlags::none; }
    void SetUnnamedNs() { flags = flags | NsFlags::isUnnamedNs; }
    bool HasUnnamedNs() const { return (flags & NsFlags::hasUnnamedNs) != NsFlags::none; }
    void SetHasUnnamedNs() { flags = flags | NsFlags::hasUnnamedNs; }
private:
    std::unique_ptr<IdentifierNode> id;
    NodeList<Node> members;
    NsFlags flags;
};

class SNGCM_AST_API AliasNode : public Node
{
public:
    AliasNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AliasNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* id_, IdentifierNode* qid_);
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
    NamespaceImportNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    NamespaceImportNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* ns_);
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
