// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/Namespace.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/Function.hpp>

namespace cmajor { namespace ast {

NamespaceNode::NamespaceNode(const Span& span_) : Node(NodeType::namespaceNode, span_), id()
{
}

NamespaceNode::NamespaceNode(const Span& span_, IdentifierNode* id_) : Node(NodeType::namespaceNode, span_), id(id_)
{
    id->SetParent(this);
}

Node* NamespaceNode::Clone(CloneContext& cloneContext) const
{
    NamespaceNode* clone = new NamespaceNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone(cloneContext)));
    int n = members.Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = members[i];
        if (cloneContext.MakeTestUnits() && member->GetNodeType() == NodeType::functionNode && (static_cast<FunctionNode*>(member)->GetSpecifiers() & Specifiers::unit_test_) != Specifiers::none)
        {
            FunctionNode* unitTestFunction = static_cast<FunctionNode*>(member->Clone(cloneContext));
            unitTestFunction->SetParent(const_cast<NamespaceNode*>(this));
            cloneContext.AddUnitTestFunction(std::unique_ptr<FunctionNode>(unitTestFunction));
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
    members.Write(writer);
}

void NamespaceNode::Read(AstReader& reader)
{
    Node::Read(reader);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    members.Read(reader);
    members.SetParent(this);
}

void NamespaceNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
}

AliasNode::AliasNode(const Span& span_) : Node(NodeType::aliasNode, span_), id(), qid()
{
}

AliasNode::AliasNode(const Span& span_, IdentifierNode* id_, IdentifierNode* qid_) : Node(NodeType::aliasNode, span_), id(id_), qid(qid_)
{
    id->SetParent(this);
    qid->SetParent(this);
}

Node* AliasNode::Clone(CloneContext& cloneContext) const
{
    return new AliasNode(GetSpan(), static_cast<IdentifierNode*>(id->Clone(cloneContext)), static_cast<IdentifierNode*>(qid->Clone(cloneContext)));
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

NamespaceImportNode::NamespaceImportNode(const Span& span_) : Node(NodeType::namespaceImportNode, span_), ns()
{
}

NamespaceImportNode::NamespaceImportNode(const Span& span_, IdentifierNode* ns_) : Node(NodeType::namespaceImportNode, span_), ns(ns_)
{
    ns->SetParent(this);
}

Node* NamespaceImportNode::Clone(CloneContext& cloneContext) const
{
    return new NamespaceImportNode(GetSpan(), static_cast<IdentifierNode*>(ns->Clone(cloneContext)));
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

} } // namespace cmajor::ast
