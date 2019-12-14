// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/Element.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngxml { namespace dom {

using namespace soulng::unicode;

std::u32string AttrValueEscape(const std::u32string& attributeValue, char32_t delimiter)
{
    std::u32string result;
    for (char32_t c : attributeValue)
    {
        switch (c)
        {
        case '<': result.append(U"&lt;"); break;
        case '&': result.append(U"&amp;"); break;
        case '"': if (delimiter == '"') result.append(U"&quot;"); else result.append(1, '"'); break;
        case '\'': if (delimiter == '\'') result.append(U"&apos;"); else result.append(1, '\''); break;
        default: result.append(1, c); break;
        }
    }
    return result;
}

std::u32string MakeXmlAttrValue(const std::u32string& attributeValue)
{
    std::u32string result;
    if (attributeValue.find('"') == std::u32string::npos)
    {
        result.append(1, '"');
        result.append(AttrValueEscape(attributeValue, '"'));
        result.append(1, '"');
    }
    else if (attributeValue.find('\'') == std::u32string::npos)
    {
        result.append(1, '\'');
        result.append(AttrValueEscape(attributeValue, '\''));
        result.append(1, '\'');
    }
    else
    {
        result.append(1, '"');
        result.append(AttrValueEscape(attributeValue, '"'));
        result.append(1, '"');
    }
    return result;
}

Attr::Attr() : Node(NodeType::attributeNode, U""), value(U"")
{
}

Attr::Attr(const std::u32string& name_, const std::u32string& value_) : Node(NodeType::attributeNode, name_), value(value_)
{
}

std::unique_ptr<Node> Attr::CloneNode(bool deep)
{
    return std::unique_ptr<Node>(new Attr(Name(), value));
}

void Attr::Write(CodeFormatter& formatter)
{
    formatter.Write(" " + ToUtf8(Name()) + "=");
    formatter.Write(ToUtf8(MakeXmlAttrValue(value)));
}

Element::Element(const std::u32string& name_) : ParentNode(NodeType::elementNode, name_)
{
}

Element::Element(const std::u32string& name_, std::map<std::u32string, std::unique_ptr<Attr>>&& attributeMap_) : ParentNode(NodeType::elementNode, name_), attributeMap(std::move(attributeMap_))
{
}

std::unique_ptr<Node> Element::CloneNode(bool deep)
{
    std::unique_ptr<Node> clone(new Element(Name()));
    ParentNode* cloneAsParent = static_cast<ParentNode*>(clone.get());
    std::map<std::u32string, std::unique_ptr<Attr>> clonedAttributeMap;
    for (const auto& p : attributeMap)
    {
        std::unique_ptr<Node> clonedAttrNode = p.second->CloneNode(false);
        clonedAttrNode->InternalSetParent(cloneAsParent);
        clonedAttributeMap[p.first] = std::unique_ptr<Attr>(static_cast<Attr*>(clonedAttrNode.release()));
    }
    Element* cloneAsElement = static_cast<Element*>(clone.get());
    cloneAsElement->attributeMap = std::move(clonedAttributeMap);
    if (deep)
    {
        CloneChildrenTo(cloneAsParent);
    }
    return clone;
}

bool Element::HasAttributes() const
{
    return !attributeMap.empty();
}

void Element::Write(CodeFormatter& formatter)
{
    if (HasChildNodes())
    {
        if (attributeMap.empty())
        {
            formatter.Write("<" + ToUtf8(Name()) + ">");
        }
        else
        {
            formatter.Write("<" + ToUtf8(Name()));
            WriteAttributes(formatter);
            formatter.Write(">");
        }
        bool prevPreserveSpace = formatter.PreserveSpace();
        if (GetAttribute(U"xml:space") == U"preserve")
        {
            formatter.SetPreserveSpace(true);
        }
        bool preserveSpace = formatter.PreserveSpace() || !HasMultilineContent();
        if (!preserveSpace)
        {
            formatter.WriteLine();
            formatter.IncIndent();
        }
        ParentNode::Write(formatter);
        if (!preserveSpace)
        {
            formatter.DecIndent();
            formatter.WriteLine("</" + ToUtf8(Name()) + ">");
        }
        else if (prevPreserveSpace)
        {
            formatter.Write("</" + ToUtf8(Name()) + ">");
        }
        else
        {
            formatter.WriteLine("</" + ToUtf8(Name()) + ">");
        }
        formatter.SetPreserveSpace(prevPreserveSpace);
    }
    else
    {
        if (attributeMap.empty())
        {
            formatter.WriteLine("<" + ToUtf8(Name()) + "/>");
        }
        else
        {
            formatter.Write("<" + ToUtf8(Name()));
            WriteAttributes(formatter);
            formatter.WriteLine("/>");
        }
    }
}

void Element::WriteAttributes(CodeFormatter& formatter)
{
    for (auto& p : attributeMap)
    {
        std::unique_ptr<Attr>& attr = p.second;
        attr->Write(formatter);
    }
}

bool Element::HasMultilineContent()
{
    if (FirstChild() != LastChild()) return true;
    Node* child = FirstChild();
    if (child)
    {
        if (child->GetNodeType() == NodeType::elementNode || child->GetNodeType() == NodeType::documentNode) return true;
        if (child->ValueContainsNewLine()) return true;
    }
    return false;
}

std::u32string Element::GetAttribute(const std::u32string& attrName) const
{
    auto it = attributeMap.find(attrName);
    if (it != attributeMap.cend())
    {
        return it->second->Value();
    }
    return std::u32string();
}

void Element::AddAttribute(std::unique_ptr<Attr>&& attr)
{
    attributeMap[attr->Name()] = std::move(attr);
}

void Element::SetAttribute(const std::u32string& attrName, const std::u32string& attrValue)
{
    attributeMap[attrName] = std::unique_ptr<Attr>(new Attr(attrName, attrValue));
}

void Element::RemoveAttribute(const std::u32string& attrName)
{
    attributeMap.erase(attrName);
}

void Element::WalkAttribute(NodeOp& nodeOp)
{
    for (const auto& p : attributeMap)
    {
        Attr* attr = p.second.get();
        nodeOp.Apply(attr);
    }
}

class ElementsByTagNameVisitor : public Visitor
{
public:
    ElementsByTagNameVisitor(NodeList& elements_, const std::u32string& tagName_);
    void BeginVisit(Element* elemnt) override;
private:
    NodeList& elements;
    std::u32string tagName;
};

ElementsByTagNameVisitor::ElementsByTagNameVisitor(NodeList& elements_, const std::u32string& tagName_) : elements(elements_), tagName(tagName_)
{
}

void ElementsByTagNameVisitor::BeginVisit(Element* element)
{
    if (element->Name() == tagName)
    {
        elements.InternalAddNode(element);
    }
}

NodeList Element::GetElementsByTagName(const std::u32string& tagName)
{
    NodeList result;
    ElementsByTagNameVisitor visitor(result, tagName);
    Accept(visitor);
    return result;
}

void Element::Accept(Visitor& visitor)
{
    visitor.BeginVisit(this);
    ParentNode::Accept(visitor);
    visitor.EndVisit(this);
}

} } // namespace sngxml::dom
