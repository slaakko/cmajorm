// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/XPathContext.hpp>

namespace sngxml { namespace xpath {

XPathContext::XPathContext(sngxml::dom::Node* node_, int position_, int size_) : node(node_), position(position_), size(size_)
{
}

} } // namespace sngxml::xpath
