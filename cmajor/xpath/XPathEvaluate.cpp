// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/xpath/XPathEvaluate.hpp>
#include <cmajor/xpath/XPathContext.hpp>
#include <cmajor/xpath/XPath.hpp>
#include <cmajor/xpath/XPathDebug.hpp>
#include <cmajor/dom/Document.hpp>
#include <iostream>
#include <chrono>

namespace cmajor { namespace xpath {

XPath* xpathGrammar = nullptr;

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, cmajor::dom::Node* node)
{
    if (!xpathGrammar)
    {
        xpathGrammar = XPath::Create();
    }
    if (XPathDebugParsing())
    {
        xpathGrammar->SetLog(&std::cout);
    }
    std::chrono::time_point<std::chrono::steady_clock> startQuery = std::chrono::steady_clock::now();
    std::unique_ptr<XPathExpr> xpathExpr(xpathGrammar->Parse(&xpathExpression[0], &xpathExpression[0] + xpathExpression.length(), 0, ""));
    std::chrono::time_point<std::chrono::steady_clock> endQuery = std::chrono::steady_clock::now();
    if (XPathDebugQuery())
    {
        std::unique_ptr<dom::Node> queryDom = xpathExpr->ToDom();
        SetXPathQueryDom(std::move(queryDom));
        SetXPathQueryDuration(std::chrono::nanoseconds(endQuery - startQuery));
    }
    std::chrono::time_point<std::chrono::steady_clock> startEvaluate = std::chrono::steady_clock::now();
    XPathContext context(node, 1, 1);
    std::unique_ptr<XPathObject> result = xpathExpr->Evaluate(context);
    std::chrono::time_point<std::chrono::steady_clock> endEvaluate = std::chrono::steady_clock::now();
    if (XPathDebugQuery())
    {
        SetXPathExecuteDuration(endEvaluate - startEvaluate);
    }
    return result;
}

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, cmajor::dom::Document* document)
{
    return Evaluate(xpathExpression, static_cast<cmajor::dom::Node*>(document));
}

} } // namespace cmajor::xpath
