// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/xpath/XPathContext.hpp>

namespace cmajor { namespace xpath {

XPathContext::XPathContext(cmajor::dom::Node* node_, int position_, int size_) : node(node_), position(position_), size(size_)
{
}

} } // namespace cmajor::xpath
