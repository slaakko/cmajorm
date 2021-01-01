// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/XPathObject.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngxml { namespace xpath {

using namespace soulng::unicode;

XPathObject::XPathObject(XPathObjectType type_) : type(type_)
{
}

XPathObject::~XPathObject()
{
}

XPathNodeSet::XPathNodeSet() : XPathObject(XPathObjectType::nodeSet)
{
}

void XPathNodeSet::Add(sngxml::dom::Node* node)
{
    nodes.InternalAddNode(node);
}

std::unique_ptr<dom::Node> XPathNodeSet::ToDom() const
{
    std::unique_ptr<dom::Element> result(new dom::Element(U"nodeset"));
    result->SetAttribute(U"length", ToUtf32(std::to_string(nodes.Length())));
    int n = nodes.Length();
    for (int i = 0; i < n; ++i)
    {
        dom::Node* node = nodes[i];
        if (node->GetNodeType() == dom::NodeType::attributeNode)
        {
            std::unique_ptr<dom::Element> element(new dom::Element(U"attribute"));
            std::unique_ptr<dom::Node> clonedAttrNode = node->CloneNode(false);
            std::unique_ptr<dom::Attr> clonedAttr(static_cast<dom::Attr*>(clonedAttrNode.get()));
            element->AddAttribute(std::move(clonedAttr));
            result->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        }
        else
        {
            std::u32string nodeName;
            switch (node->GetNodeType())
            {
            case dom::NodeType::documentNode:
            {
                nodeName = U"document";
                break;
            }
            case dom::NodeType::elementNode:
            {
                nodeName = U"element";
                break;
            }
            case dom::NodeType::textNode:
            {
                nodeName = U"text";
                break;
            }
            case dom::NodeType::cdataSectionNode:
            {
                nodeName = U"cdataSection";
                break;
            }
            case dom::NodeType::commentNode:
            {
                nodeName = U"comment";
                break;
            }
            case dom::NodeType::processingInstructionNode:
            {
                nodeName = U"processingInstruction";
                break;
            }
            }
            std::unique_ptr<dom::Element> element(new dom::Element(nodeName));
            element->AppendChild(node->CloneNode(true));
            result->AppendChild(std::unique_ptr<dom::Node>(element.release()));
        }
    }
    return std::unique_ptr<dom::Node>(result.release());
}

XPathBoolean::XPathBoolean(bool value_) : XPathObject(XPathObjectType::boolean), value(value_)
{
}

std::unique_ptr<dom::Node> XPathBoolean::ToDom() const
{
    std::unique_ptr<dom::Element> result(new dom::Element(U"boolean"));
    std::u32string val = U"true";
    if (!value)
    {
        val = U"false";
    }
    result->SetAttribute(U"value", val);
    return std::unique_ptr<dom::Node>(result.release());
}

XPathNumber::XPathNumber(double value_) : XPathObject(XPathObjectType::number), value(value_)
{
}

std::unique_ptr<dom::Node> XPathNumber::ToDom() const
{
    std::unique_ptr<dom::Element> result(new dom::Element(U"number"));
    result->SetAttribute(U"value", ToUtf32(std::to_string(value)));
    return std::unique_ptr<dom::Node>(result.release());
}

XPathString::XPathString(const std::u32string& value_) : XPathObject(XPathObjectType::string), value(value_)
{
}

std::unique_ptr<dom::Node> XPathString::ToDom() const
{
    std::unique_ptr<dom::Element> result(new dom::Element(U"string"));
    result->SetAttribute(U"value", value);
    return std::unique_ptr<dom::Node>(result.release());
}

} } // namespace sngxml::xpath
