// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_XPATH_DEBUG
#define SNGXML_XPATH_XPATH_DEBUG
#include <sngxml/xpath/XPathApi.hpp>
#include <sngxml/dom/Node.hpp>
#include <chrono>

namespace sngxml { namespace xpath {

SNGXML_XPATH_API bool XPathDebugParsing();
SNGXML_XPATH_API void SetXPathDebugParsing();
SNGXML_XPATH_API bool XPathDebugQuery();
SNGXML_XPATH_API void SetXPathDebugQuery();
SNGXML_XPATH_API void SetXPathQueryDom(std::unique_ptr<sngxml::dom::Node>&& queryDom);
SNGXML_XPATH_API std::unique_ptr<sngxml::dom::Node> GetXPathQueryDom();
SNGXML_XPATH_API void SetXPathQueryDuration(std::chrono::nanoseconds duration);
SNGXML_XPATH_API std::chrono::nanoseconds XPathQueryDuration();
SNGXML_XPATH_API void SetXPathExecuteDuration(std::chrono::nanoseconds duration);
SNGXML_XPATH_API std::chrono::nanoseconds XPathExecuteDuration();
SNGXML_XPATH_API void InitDebug();
SNGXML_XPATH_API void DoneDebug();

} } // namespace sngxml::xpath

#endif // SNGXML_XPATH_XPATH_DEBUG
