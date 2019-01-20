#include <cmajor/util/InitDone.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/xpath/InitDone.hpp>
#include <cmajor/xpath/XPathEvaluate.hpp>
#include <cmajor/xpath/XPathDebug.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/util/CodeFormatter.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

struct InitDone
{
    InitDone()
    {
        cmajor::util::Init();
        cmajor::parsing::Init();
        cmajor::xpath::Init();
    }
    ~InitDone()
    {
        cmajor::xpath::Done();
        cmajor::parsing::Done();
        cmajor::util::Done();
    }
};

using namespace cmajor::unicode;

void Execute(const std::string& query, const std::string& xmlFilePath, std::chrono::nanoseconds& parseDuration)
{
    std::chrono::time_point<std::chrono::steady_clock> startParse = std::chrono::steady_clock::now();
    std::unique_ptr<cmajor::dom::Document> document = cmajor::dom::ReadDocument(xmlFilePath);
    std::chrono::time_point<std::chrono::steady_clock> endParse = std::chrono::steady_clock::now();
    parseDuration = endParse - startParse;
    std::unique_ptr<cmajor::xpath::XPathObject> result = cmajor::xpath::Evaluate(ToUtf32(query), document.get());
    std::unique_ptr<cmajor::dom::Node> queryDom = cmajor::xpath::GetXPathQueryDom();
    std::unique_ptr<cmajor::dom::Element> xpathElement(new cmajor::dom::Element(U"xpath"));
    cmajor::dom::Element* queryElement = new cmajor::dom::Element(U"query");
    queryElement->AppendChild(std::move(queryDom));
    xpathElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(queryElement));
    cmajor::dom::Element* resultElement = new cmajor::dom::Element(U"result");
    resultElement->AppendChild(result->ToDom());
    xpathElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(resultElement));
    CodeFormatter formatter(std::cout);
    formatter.SetIndentSize(2);
    xpathElement->Write(formatter);
}

int main(int argc, const char** argv)
{
    try
    {
        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
        InitDone initDone;
        cmajor::xpath::SetXPathDebugQuery();
        bool first = true;
        std::string query;
        std::string xmlFilePath;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (first)
            {
                first = false;
                query = arg;
            }
            else if (xmlFilePath.empty())
            {
                xmlFilePath = arg;
            }
            else
            {
                throw std::runtime_error("too many arguments");
            }
        }
        if (query.empty())
        {
            throw std::runtime_error("no query given");
        }
        if (xmlFilePath.empty())
        {
            throw std::runtime_error("no xml file path given");
        }
        std::chrono::nanoseconds parseDuration;
        Execute(query, xmlFilePath, parseDuration);
        std::chrono::nanoseconds queryDuration = cmajor::xpath::XPathQueryDuration();
        std::chrono::nanoseconds executeDuration = cmajor::xpath::XPathExecuteDuration();
        std::cout << "parsing XML took " << std::chrono::duration_cast<std::chrono::milliseconds>(parseDuration).count() << " ms" << std::endl;
        std::cout << "parsing query took " << std::chrono::duration_cast<std::chrono::milliseconds>(queryDuration).count() << " ms" << std::endl;
        std::cout << "executing query took " << std::chrono::duration_cast<std::chrono::milliseconds>(executeDuration).count() << " ms" << std::endl;
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        std::cout << "duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
