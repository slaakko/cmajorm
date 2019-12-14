// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Exception.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Error.hpp>

namespace sngxml { namespace dom {

using namespace soulng::unicode;

Document::Document() : ParentNode(NodeType::documentNode, U"document"), documentElement(nullptr), docType(nullptr), indexValid(false), xmlStandalone(false)
{
}

void Document::Write(CodeFormatter& formatter)
{
    if (!xmlVersion.empty() && !xmlEncoding.empty())
    {
        formatter.WriteLine("<?xml version=\"" + ToUtf8(xmlVersion) + "\" encoding=\"" + ToUtf8(xmlEncoding) + "\"?>");
    }
    ParentNode::Write(formatter);
}

std::unique_ptr<Node> Document::CloneNode(bool deep)
{
    std::unique_ptr<Node> clonedDocument = std::unique_ptr<Node>(new Document());
    if (deep)
    {
        ParentNode* parentNode = static_cast<ParentNode*>(clonedDocument.get());
        CloneChildrenTo(parentNode);
    }
    return clonedDocument;
}

Node* Document::InsertBefore(std::unique_ptr<Node>&& newChild, Node* refChild)
{
    CheckValidInsert(newChild.get(), refChild);
    if (newChild->GetNodeType() == NodeType::elementNode)
    {
        Assert(documentElement == nullptr, "document element is not null");
        documentElement = static_cast<Element*>(newChild.get());
    }
    return ParentNode::InsertBefore(std::move(newChild), refChild);
}

std::unique_ptr<Node> Document::ReplaceChild(std::unique_ptr<Node>&& newChild, Node* oldChild)
{
    if (!oldChild)
    {
        throw DomException("could not replace node: given old child is null");
    }
    if (oldChild->Parent() != this)
    {
        throw DomException("could not replace node: given old child is not child of this node");
    }
    CheckValidInsert(newChild.get(), nullptr);
    if (newChild->GetNodeType() == NodeType::elementNode)
    {
        std::unique_ptr<Node> removed = RemoveChild(oldChild);
        AppendChild(std::move(newChild));
        return removed;
    }
    else
    {
        return ParentNode::ReplaceChild(std::move(newChild), oldChild);
    }
}

std::unique_ptr<Node> Document::RemoveChild(Node* oldChild)
{
    if (!oldChild)
    {
        throw DomException("could not remove node: given old child is null");
    }
    if (oldChild->Parent() != this)
    {
        throw DomException("could not remove node: given old child is not child of this node");
    }
    if (oldChild->GetNodeType() == NodeType::elementNode)
    {
        documentElement = nullptr;
    }
    else if (oldChild->GetNodeType() == NodeType::documentTypeNode)
    {
        docType = nullptr;
    }
    return ParentNode::RemoveChild(oldChild);
}

Node* Document::AppendChild(std::unique_ptr<Node>&& newChild)
{
    CheckValidInsert(newChild.get(), nullptr);
    if (newChild->GetNodeType() == NodeType::elementNode)
    {
        Assert(documentElement == nullptr, "document element is not null");
        documentElement = static_cast<Element*>(newChild.get());
    }
    return ParentNode::AppendChild(std::move(newChild));
}

void Document::InternalInvalidateIndex()
{
    indexValid = false;
}

void Document::Accept(Visitor& visitor)
{
    visitor.BeginVisit(this);
    ParentNode::Accept(visitor);
    visitor.EndVisit(this);
}

class BuildIndexVisitor : public Visitor
{
public:
    BuildIndexVisitor(std::unordered_map<std::u32string, Element*>& elementsByIdMap_);
    void BeginVisit(Element* element) override;
private:
    std::unordered_map<std::u32string, Element*>& elementsByIdMap;
};

BuildIndexVisitor::BuildIndexVisitor(std::unordered_map<std::u32string, Element*>& elementsByIdMap_) : elementsByIdMap(elementsByIdMap_)
{
}

void BuildIndexVisitor::BeginVisit(Element* element)
{
    const std::u32string& id = element->GetAttribute(U"id");
    if (!id.empty())
    {
        elementsByIdMap[id] = element;
    }
}

Element* Document::GetElementById(const std::u32string& elementId)
{
    if (!indexValid)
    {
        elementsByIdMap.clear();
        BuildIndexVisitor visitor(elementsByIdMap);
        Accept(visitor);
        indexValid = true;
    }
    auto it = elementsByIdMap.find(elementId);
    if (it != elementsByIdMap.cend())
    {
        Element* element = it->second;
        return element;
    }
    return nullptr;
}

void Document::CheckValidInsert(Node* node, Node* refNode)
{
    if (node->GetNodeType() == NodeType::elementNode)
    {
        if (refNode != nullptr || documentElement != nullptr)
        {
            throw DomException("attempt to insert a second element to a document");
        }
    }
    else if (node->GetNodeType() == NodeType::documentTypeNode)
    {
        if (refNode != nullptr || docType != nullptr)
        {
            throw DomException("attempt to insert a second document type node to a document");
        }
    }
}

} } // namespace sngxml::dom
