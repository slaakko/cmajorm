// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/xpath/XPathContext.hpp>
#include <sngxml/xpath/XPathParser.hpp>
#include <sngxml/xpath/XPathLexer.hpp>
#include <sngxml/xpath/XPathDebug.hpp>
#include <sngxml/dom/Document.hpp>
#include <soulng/lexer/XmlParsingLog.hpp>
#include <iostream>
#include <chrono>

namespace sngxml { namespace xpath {

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, sngxml::dom::Node* node)
{
    soulng::lexer::XmlParsingLog debugLog(std::cerr);
    std::chrono::time_point<std::chrono::steady_clock> startQuery = std::chrono::steady_clock::now();
    XPathLexer xpathLexer(xpathExpression, "", 0);
    if (XPathDebugParsing())
    {
        xpathLexer.SetLog(&debugLog);
    }
    std::unique_ptr<XPathExpr> xpathExpr(XPathParser::Parse(xpathLexer));
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

std::unique_ptr<XPathObject> Evaluate(const std::u32string& xpathExpression, sngxml::dom::Document* document)
{
    return Evaluate(xpathExpression, static_cast<sngxml::dom::Node*>(document));
}

} } // namespace sngxml::xpath
