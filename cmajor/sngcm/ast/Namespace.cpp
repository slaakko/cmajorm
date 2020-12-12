// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Namespace.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/Function.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Sha1.hpp>
#include <boost/uuid/random_generator.hpp>

namespace sngcm { namespace ast {

using namespace soulng::unicode;
using namespace soulng::util;

NamespaceNode::NamespaceNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::namespaceNode, span_, moduleId_), id(), flags()
{
}

NamespaceNode::NamespaceNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* id_) : Node(NodeType::namespaceNode, span_, moduleId_), id(id_), flags()
{
    if (id == nullptr)
    {
        SetUnnamedNs();
        Sha1 sha1;
        boost::uuids::uuid randomUuid = boost::uuids::random_generator()();
        for (uint8_t x : randomUuid)
        {
            sha1.Process(x);
        }
        id.reset(new IdentifierNode(span_, moduleId_, U"unnamed_ns_" + ToUtf32(sha1.GetDigest())));
    }
    id->SetParent(this);
}

IdentifierNode* NamespaceNode::Id() const
{
    return id.get();
}

Node* NamespaceNode::Clone(CloneContext& cloneContext) const
{
    NamespaceNode* clone = nullptr;
    if (IsUnnamedNs())
    {
        clone = new NamespaceNode(GetSpan(), ModuleId(), nullptr);
    }
    else
    {
        clone = new NamespaceNode(GetSpan(), ModuleId(), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    }
    clone->flags = flags;
    int n = members.Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = members[i];
        if (cloneContext.MakeTestUnits() && member->GetNodeType() == NodeType::functionNode && (static_cast<FunctionNode*>(member)->GetSpecifiers() & Specifiers::unit_test_) != Specifiers::none)
        {
            FunctionNode* unitTestFunction = static_cast<FunctionNode*>(member->Clone(cloneContext));
            unitTestFunction->SetParent(const_cast<NamespaceNode*>(this));
            cloneContext.AddUnitTestFunction(unitTestFunction);
        }
        else
        {
            clone->AddMember(member->Clone(cloneContext));
        }
    }
    return clone;
}

void NamespaceNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void NamespaceNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(id.get());
    writer.GetBinaryWriter().Write(static_cast<int8_t>(flags));
    members.Write(writer);
}

void NamespaceNode::Read(AstReader& reader)
{
    Node::Read(reader);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    flags = static_cast<NsFlags>(reader.GetBinaryReader().ReadSByte());
    members.Read(reader);
    members.SetParent(this);
}

void NamespaceNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
    if (member->GetNodeType() == NodeType::namespaceNode)
    {
        NamespaceNode* ns = static_cast<NamespaceNode*>(member);
        if (ns->IsUnnamedNs() || ns->HasUnnamedNs())
        {
            SetHasUnnamedNs();
        }
    }
}

AliasNode::AliasNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::aliasNode, span_, moduleId_), id(), qid()
{
}

AliasNode::AliasNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* id_, IdentifierNode* qid_) : Node(NodeType::aliasNode, span_, moduleId_), id(id_), qid(qid_)
{
    id->SetParent(this);
    qid->SetParent(this);
}

Node* AliasNode::Clone(CloneContext& cloneContext) const
{
    AliasNode* clone = new AliasNode(GetSpan(), ModuleId(), static_cast<IdentifierNode*>(id->Clone(cloneContext)), static_cast<IdentifierNode*>(qid->Clone(cloneContext)));
    return clone;
}

void AliasNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void AliasNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(id.get());
    writer.Write(qid.get());
}

void AliasNode::Read(AstReader& reader)
{
    Node::Read(reader);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    qid.reset(reader.ReadIdentifierNode());
    qid->SetParent(this);
}

IdentifierNode* AliasNode::Id() const
{
    return id.get();
}

IdentifierNode* AliasNode::Qid() const
{
    return qid.get();
}

NamespaceImportNode::NamespaceImportNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::namespaceImportNode, span_, moduleId_), ns()
{
}

NamespaceImportNode::NamespaceImportNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* ns_) : Node(NodeType::namespaceImportNode, span_, moduleId_), ns(ns_)
{
    ns->SetParent(this);
}

Node* NamespaceImportNode::Clone(CloneContext& cloneContext) const
{
    NamespaceImportNode* clone = new NamespaceImportNode(GetSpan(), ModuleId(), static_cast<IdentifierNode*>(ns->Clone(cloneContext)));
    return clone;
}

void NamespaceImportNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void NamespaceImportNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(ns.get());
}

void NamespaceImportNode::Read(AstReader& reader)
{
    Node::Read(reader);
    ns.reset(reader.ReadIdentifierNode());
    ns->SetParent(this);
}

IdentifierNode* NamespaceImportNode::Ns() const
{
    return ns.get();
}

} } // namespace sngcm::ast
