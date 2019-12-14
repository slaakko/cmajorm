// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_XPATH_EVALUATE
#define SNGXML_XPATH_XPATH_EVALUATE
#include <sngxml/xpath/XPathObject.hpp>

namespace sngxml { namespace xpath {

SNGXML_XPATH_API std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, sngxml::dom::Node* node);
SNGXML_XPATH_API std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, sngxml::dom::Document* document);

} } // namespace sngxml::xpath

#endif // SNGXML_XPATH_XPATH_EVALUATE
