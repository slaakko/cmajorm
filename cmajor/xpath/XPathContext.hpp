// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_XPATH_XPATH_CONTEXT
#define CMAJOR_XPATH_XPATH_CONTEXT
#include <cmajor/dom/Node.hpp>

namespace cmajor { namespace xpath {

class XPathContext
{
public:
    XPathContext(cmajor::dom::Node* node_, int position_, int size_);
    cmajor::dom::Node* Node() const { return node; }
    int Position() const { return position; }
    int Size() const { return size; }
private:
    cmajor::dom::Node* node;
    int position;
    int size;
};

} } // namespace cmajor::xpath

#endif // CMAJOR_XPATH_XPATH_CONTEXT
