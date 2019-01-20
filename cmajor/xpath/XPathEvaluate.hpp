// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_XPATH_XPATH_EVALUATE
#define CMAJOR_XPATH_XPATH_EVALUATE
#include <cmajor/xpath/XPathObject.hpp>

namespace cmajor { namespace xpath {

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, cmajor::dom::Node* node);
std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, cmajor::dom::Document* document);

} } // namespace cmajor::xpath

#endif // CMAJOR_XPATH_XPATH_EVALUATE
