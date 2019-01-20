// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_XPATH_XPATH_DEBUG
#define CMAJOR_XPATH_XPATH_DEBUG
#include <cmajor/dom/Node.hpp>
#include <chrono>

namespace cmajor { namespace xpath {

bool XPathDebugParsing();
void SetXPathDebugParsing();
bool XPathDebugQuery();
void SetXPathDebugQuery();
void SetXPathQueryDom(std::unique_ptr<dom::Node>&& queryDom);
std::unique_ptr<dom::Node> GetXPathQueryDom();
void SetXPathQueryDuration(std::chrono::nanoseconds duration);
std::chrono::nanoseconds XPathQueryDuration();
void SetXPathExecuteDuration(std::chrono::nanoseconds duration);
std::chrono::nanoseconds XPathExecuteDuration();
void InitDebug();
void DoneDebug();

} } // namespace cmajor::xpath

#endif // CMAJOR_XPATH_XPATH_DEBUG
