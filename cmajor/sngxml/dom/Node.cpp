// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/Node.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/DocumentFragment.hpp>
#include <sngxml/dom/Exception.hpp>
#include <algorithm>

namespace sngxml { namespace dom {

NodeOp::~NodeOp()
{
}

void NodeOp::Apply(Node* node)
{
}

std::u32string AxisName(Axis axis)
{
    switch (axis)
    {
    case Axis::child: return U"child";
    case Axis::descendant: return U"descendant";
    case Axis::parent: return U"parent";
    case Axis::ancestor: return U"ancestor";
    case Axis::followingSibling: return U"followingSibling";
    case Axis::precedingSibling: return U"precedingSibling";
    case Axis::following: return U"following";
    case Axis::preceding: return U"preceding";
    case Axis::attribute: return U"attribute";
    case Axis::ns: return U"namespace";
    case Axis::self: return U"self";
    case Axis::descendantOrSelf: return U"descendantOrSelf";
    case Axis::ancestorOrSelf: return U"ancestorOrSelf";
    }
    return std::u32string();
}

Node::Node(NodeType nodeType_, const std::u32string& name_) : nodeType(nodeType_), name(name_), parent(nullptr), previousSibling(nullptr), nextSibling(nullptr), ownerDocument(nullptr)
{
}

Node::~Node()
{
}

std::u32string Node::Prefix() const
{
    if (nodeType == NodeType::elementNode || nodeType == NodeType::attributeNode)
    {
        auto colonPos = name.find(':');
        if (colonPos != std::u32string::npos)
        {
            return name.substr(0, colonPos);
        }
    }
    return std::u32string();
}

void Node::SetPrefix(const std::u32string& prefix)
{
    if (nodeType == NodeType::elementNode || nodeType == NodeType::attributeNode)
    {
        auto colonPos = name.find(':');
        if (prefix.empty())
        {
            if (colonPos != std::u32string::npos)
            {
                name = name.substr(colonPos + 1);
            }
        }
        else
        {
            if (colonPos != std::u32string::npos)
            {
                name = prefix + U":" + name.substr(colonPos + 1);
            }
            else
            {
                name = prefix + U":" + name;
            }
        }
    }
    else
    {
        throw DomException("cannot set prefix for this node type");
    }
}

std::u32string Node::LocalName() const
{
    if (nodeType == NodeType::elementNode || nodeType == NodeType::attributeNode)
    {
        auto colonPos = name.find(':');
        if (colonPos != std::u32string::npos)
        {
            return name.substr(colonPos + 1);
        }
        else
        {
            return name;
        }
    }
    else
    {
        return std::u32string();
    }
}

void Node::Walk(NodeOp& nodeOp, Axis axis)
{
    switch (axis)
    {
        case Axis::child: WalkChildren(nodeOp); break;
        case Axis::descendant: WalkDescendant(nodeOp); break;
        case Axis::descendantOrSelf: WalkDescendantOrSelf(nodeOp); break;
        case Axis::parent: WalkParent(nodeOp); break;
        case Axis::ancestor: WalkAncestor(nodeOp); break;
        case Axis::ancestorOrSelf: WalkAncestorOrSelf(nodeOp); break;
        case Axis::followingSibling: WalkFollowingSibling(nodeOp); break;
        case Axis::precedingSibling: WalkPrecedingSibling(nodeOp); break;
        case Axis::following: WalkFollowing(nodeOp); break;
        case Axis::preceding: WalkPreceding(nodeOp); break;
        case Axis::attribute: WalkAttribute(nodeOp); break;
        case Axis::self: nodeOp.Apply(this); break;
    }
}

void Node::WalkChildren(NodeOp& nodeOp)
{
}

void Node::WalkDescendant(NodeOp& nodeOp)
{
}

void Node::WalkDescendantOrSelf(NodeOp& nodeOp)
{
    nodeOp.Apply(this);
}

void Node::WalkParent(NodeOp& nodeOp)
{
    if (parent)
    {
        nodeOp.Apply(parent);
    }
}

void Node::WalkFollowing(NodeOp& nodeOp)
{
    Node* ns = nextSibling;
    if (ns)
    {
        ns->WalkDescendantOrSelf(nodeOp);
        ns = ns->nextSibling;
    }
}

void Node::WalkPreceding(NodeOp& nodeOp)
{
}

void Node::WalkPrecedingOrSelf(NodeOp& nodeOp)
{
    nodeOp.Apply(this);
}

void Node::WalkAncestor(NodeOp& nodeOp)
{
    if (parent)
    {
        parent->WalkAncestorOrSelf(nodeOp);
    }
}

void Node::WalkAncestorOrSelf(NodeOp& nodeOp)
{
    nodeOp.Apply(this);
    if (parent)
    {
        parent->WalkAncestorOrSelf(nodeOp);
    }
}

void Node::WalkFollowingSibling(NodeOp& nodeOp)
{
    Node* ns = nextSibling;
    if (ns)
    {
        nodeOp.Apply(ns);
        ns = ns->nextSibling;
    }
}

void Node::WalkPrecedingSibling(NodeOp& nodeOp)
{
    Node* ps = previousSibling;
    if (ps)
    {
        nodeOp.Apply(ps);
        ps = ps->previousSibling;
    }
}

void Node::WalkAttribute(NodeOp& nodeOp)
{
}

void Node::InternalLinkBefore(Node* newNode)
{
    newNode->previousSibling = previousSibling;
    newNode->nextSibling = this;
    previousSibling = newNode;
}

void Node::InternalLinkAfter(Node* newNode)
{
    newNode->previousSibling = this;
    newNode->nextSibling = nextSibling;
    nextSibling = newNode;
}

void Node::InternalUnlink()
{
    if (previousSibling)
    {
        previousSibling->nextSibling = nextSibling;
    }
    if (nextSibling)
    {
        nextSibling->previousSibling = previousSibling;
    }
}

void Node::InternalSetNamespaceUri(const std::u32string& namespaceUri_)
{
    namespaceUri = namespaceUri_;
}

ParentNode::ParentNode(NodeType nodeType_, const std::u32string& name_) : Node(nodeType_, name_), firstChild(nullptr), lastChild(nullptr)
{
}

ParentNode::~ParentNode()
{
    Node* child = firstChild;
    while (child != nullptr)
    {
        Node* toDel = child;
        child = child->NextSibling();
        delete toDel;
    }
}

NodeList ParentNode::ChildNodes() const
{
    NodeList result;
    Node* child = firstChild;
    while (child != nullptr)
    {
        result.InternalAddNode(child);
        child = child->NextSibling();
    }
    return result;
}

void ParentNode::Write(CodeFormatter& formatter)
{
    Node* child = firstChild;
    while (child != nullptr)
    {
        child->Write(formatter);
        child = child->NextSibling();
    }
}

void ParentNode::CloneChildrenTo(ParentNode* clone) const
{
    Node* child = firstChild;
    while (child != nullptr)
    {
        clone->AppendChild(child->CloneNode(true));
        child = child->NextSibling();
    }
}

Node* ParentNode::InsertBefore(std::unique_ptr<Node>&& newChild, Node* refChild)
{
    if (refChild == nullptr)
    {
        return AppendChild(std::move(newChild));
    }
    else
    {
        if (OwnerDocument())
        {
            OwnerDocument()->InternalInvalidateIndex();
        }
        if (newChild->GetNodeType() == NodeType::documentFragmentNode)
        {
            DocumentFragment* documentFragment = static_cast<DocumentFragment*>(newChild.get());
            Node* docFragChild = documentFragment->FirstChild();
            Node* nodeInserted = nullptr;
            while (docFragChild != nullptr)
            {
                std::unique_ptr<Node> docFragChildRemoved = documentFragment->RemoveChild(docFragChild);
                nodeInserted = InsertBefore(std::move(docFragChildRemoved), refChild);
                docFragChild = documentFragment->FirstChild();
            }
            return nodeInserted;
        }
        else
        {
            if (newChild->Parent())
            {
                newChild.reset(newChild->Parent()->RemoveChild(newChild.get()).release());
            }
            if (firstChild == refChild)
            {
                firstChild = newChild.get();
            }
            newChild->InternalSetParent(this);
            newChild->InternalSetOwnerDocument(OwnerDocument());
            Node* nodeInserted = newChild.get();
            refChild->InternalLinkBefore(newChild.release());
            return nodeInserted;
        }
    }
}

std::unique_ptr<Node> ParentNode::RemoveChild(Node* oldChild)
{
    if (!oldChild)
    {
        throw DomException("could not remove node: given old child is null");
    }
    if (oldChild->Parent() != this)
    {
        throw DomException("could not remove node: given old child is not child of this node");
    }
    if (OwnerDocument())
    {
        OwnerDocument()->InternalInvalidateIndex();
    }
    oldChild->InternalUnlink();
    if (oldChild == firstChild)
    {
        firstChild = oldChild->NextSibling();
    }
    if (oldChild == lastChild)
    {
        lastChild = oldChild->PreviousSibling();
    }
    std::unique_ptr<Node> removedNode = std::unique_ptr<Node>(oldChild);
    removedNode->InternalSetOwnerDocument(nullptr);
    removedNode->InternalSetParent(nullptr);
    return removedNode;
}

std::unique_ptr<Node> ParentNode::ReplaceChild(std::unique_ptr<Node>&& newChild, Node* oldChild)
{
    if (!oldChild)
    {
        throw DomException("could not replace node: given old child is null");
    }
    if (oldChild->Parent() != this)
    {
        throw DomException("could not replace node: given old child is not child of this node");
    }
    if (OwnerDocument())
    {
        OwnerDocument()->InternalInvalidateIndex();
    }
    if (newChild->Parent())
    {
        newChild.reset(newChild->Parent()->RemoveChild(newChild.get()).release());
    }
    InsertBefore(std::move(newChild), oldChild);
    return RemoveChild(oldChild);
}

Node* ParentNode::AppendChild(std::unique_ptr<Node>&& newChild)
{
    if (OwnerDocument())
    {
        OwnerDocument()->InternalInvalidateIndex();
    }
    if (newChild->Parent())
    {
        newChild.reset(newChild->Parent()->RemoveChild(newChild.get()).release());
    }
    if (newChild->GetNodeType() == NodeType::documentFragmentNode)
    {
        DocumentFragment* documentFragment = static_cast<DocumentFragment*>(newChild.get());
        Node* docFragChild = documentFragment->FirstChild();
        Node* nodeInserted = nullptr;
        while (docFragChild != nullptr)
        {
            std::unique_ptr<Node> docFragChildRemoved = documentFragment->RemoveChild(docFragChild);
            nodeInserted = AppendChild(std::move(docFragChildRemoved));
            docFragChild = documentFragment->FirstChild();
        }
        return nodeInserted;
    }
    else
    {
        if (lastChild)
        {
            lastChild->InternalLinkAfter(newChild.get());
        }
        if (!firstChild)
        {
            firstChild = newChild.get();
        }
        newChild->InternalSetOwnerDocument(OwnerDocument());
        newChild->InternalSetParent(this);
        lastChild = newChild.release();
        return lastChild;
    }
}

void ParentNode::Accept(Visitor& visitor)
{
    Node* child = firstChild;
    while (child != nullptr)
    {
        child->Accept(visitor);
        child = child->NextSibling();
    }
}

void ParentNode::WalkChildren(NodeOp& nodeOp)
{
    Node* child = firstChild;
    while (child != nullptr)
    {
        nodeOp.Apply(child);
        child = child->NextSibling();
    }
}

void ParentNode::WalkDescendant(NodeOp& nodeOp)
{
    Node* child = firstChild;
    while (child != nullptr)
    {
        child->WalkDescendantOrSelf(nodeOp);
        child = child->NextSibling();
    }
}

void ParentNode::WalkDescendantOrSelf(NodeOp& nodeOp)
{
    Node::WalkDescendantOrSelf(nodeOp);
    Node* child = firstChild;
    while (child != nullptr)
    {
        child->WalkDescendantOrSelf(nodeOp);
        child = child->NextSibling();
    }
}

void ParentNode::WalkPrecedingOrSelf(NodeOp& nodeOp)
{
    Node::WalkPrecedingOrSelf(nodeOp);
    Node* child = lastChild;
    while (child != nullptr)
    {
        child->WalkPrecedingOrSelf(nodeOp);
        child = child->PreviousSibling();
    }
}

void ParentNode::WalkPreceding(NodeOp& nodeOp)
{
    Node* preceding = PreviousSibling();
    if (preceding)
    {
        preceding->WalkPrecedingOrSelf(nodeOp);
    }
}

void NodeList::InternalAddNode(Node* node)
{
    if (std::find(nodes.begin(), nodes.end(), node) == nodes.end())
    {
        nodes.push_back(node);
    }
}

} } // namespace sngxml::dom
