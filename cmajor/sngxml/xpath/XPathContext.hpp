// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_XPATH_CONTEXT
#define SNGXML_XPATH_XPATH_CONTEXT
#include <sngxml/xpath/XPathApi.hpp>
#include <sngxml/dom/Node.hpp>

namespace sngxml { namespace xpath {

class SNGXML_XPATH_API XPathContext
{
public:
    XPathContext(sngxml::dom::Node* node_, int position_, int size_);
    sngxml::dom::Node* Node() const { return node; }
    int Position() const { return position; }
    int Size() const { return size; }
private:
    sngxml::dom::Node* node;
    int position;
    int size;
};

} } // namespace sngxml::xpath

#endif // SNGXML_XPATH_XPATH_CONTEXT
