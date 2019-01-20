// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DOM_ELEMENT_INCLUDED
#define CMAJOR_DOM_ELEMENT_INCLUDED
#include <cmajor/dom/Node.hpp>
#include <map>

namespace cmajor { namespace dom {

class Attr : public Node
{
public:
    Attr();
    Attr(const std::u32string& name_, const std::u32string& value_);
    Attr(const Attr&) = delete;
    Attr& operator=(const Attr&) = delete;
    Attr(Attr&&) = delete;
    Attr& operator=(Attr&&) = delete;
    std::unique_ptr<Node> CloneNode(bool deep) override;
    void Write(CodeFormatter& formatter) override;
    const std::u32string& Value() const { return value; }
    std::u32string& Value() { return value; }
private:
    std::u32string value;
};

class Element : public ParentNode
{
public:
    Element(const std::u32string& name_);
    Element(const std::u32string& name_, std::map<std::u32string, std::unique_ptr<Attr>>&& attributeMap_);
    Element(const Element&) = delete;
    Element& operator=(const Element&) = delete;
    Element(Element&&) = delete;
    Element& operator=(Element&&) = delete;
    std::unique_ptr<Node> CloneNode(bool deep) override;
    const std::u32string& TagName() const { return Name(); }
    bool HasAttributes() const override;
    void Write(CodeFormatter& formatter) override;
    std::u32string GetAttribute(const std::u32string& attrName) const;
    void AddAttribute(std::unique_ptr<Attr>&& attr);
    void SetAttribute(const std::u32string& attrName, const std::u32string& attrValue);
    void RemoveAttribute(const std::u32string& attrName);
    void WalkAttribute(NodeOp& nodeOp) override;
    NodeList GetElementsByTagName(const std::u32string& tagName);
    void Accept(Visitor& visitor) override;
private:
    std::map<std::u32string, std::unique_ptr<Attr>> attributeMap;
    void WriteAttributes(CodeFormatter& formatter);
    bool HasMultilineContent();
};

} } // namespace cmajor::dom

#endif // CMAJOR_DOM_ELEMENT_INCLUDED
