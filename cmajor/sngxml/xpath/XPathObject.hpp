// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_XPATH_OBJECT
#define SNGXML_XPATH_XPATH_OBJECT
#include <sngxml/xpath/XPathApi.hpp>
#include <sngxml/dom/Node.hpp>

namespace sngxml { namespace xpath {

enum class XPathObjectType
{
    nodeSet, boolean, number, string
};

class SNGXML_XPATH_API XPathObject
{
public:
    XPathObject(XPathObjectType type_);
    XPathObjectType Type() const { return type; }
    virtual ~XPathObject();
    virtual std::unique_ptr<dom::Node> ToDom() const = 0;
private:
    XPathObjectType type;
};

class SNGXML_XPATH_API XPathNodeSet : public XPathObject
{
public:
    XPathNodeSet();
    sngxml::dom::Node* operator[](int index) const { return nodes[index]; }
    int Length() const { return nodes.Length(); }
    void Add(sngxml::dom::Node* node);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    sngxml::dom::NodeList nodes;
};

class SNGXML_XPATH_API XPathBoolean : public XPathObject
{
public:
    XPathBoolean(bool value_);
    bool Value() const { return value; }
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    bool value;
};

class SNGXML_XPATH_API XPathNumber : public XPathObject
{
public:
    XPathNumber(double value_);
    double Value() const { return value; }
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    double value;
};

class SNGXML_XPATH_API XPathString : public XPathObject
{
public:
    XPathString(const std::u32string& value_);
    const std::u32string& Value() const { return value; }
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string value;
};

} } // namespace sngxml::xpath

#endif // SNGXML_XPATH_XPATH_OBJECT
