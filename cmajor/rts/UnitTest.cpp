// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/UnitTest.hpp>
#include <cmajor/rts/InitDone.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Unicode.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace cmajor { namespace rt {

using namespace soulng::unicode;

const int assertionResultEmpty = 0;
const int assertionResultPassed = 1;
const int assertionResulFailed = 2;

struct AssertionResult
{
    AssertionResult(int result_, int32_t line_) : result(result_), line(line_) {}
    int result;
    int32_t line;
};

class UnitTestEngine
{
public:
    static void Init();
    static void Done();
    static UnitTestEngine& Instance() { return *instance; }
    void StartUnitTest(int32_t numAssertions_, const char* unitTestFilePath_);
    void EndUnitTest(const char* testName, int32_t exitCode);
    void SetUnitTestAssertionResult(int32_t assertionIndex, bool assertionResult, int32_t line);
    void SetUnitTestException(const std::string& exceptionStr_) { exceptionStr = exceptionStr_; }
private:
    static std::unique_ptr<UnitTestEngine> instance;
    int numAssertions;
    std::string unitTestFilePath;
    std::string exceptionStr;
    std::vector<AssertionResult> assertionResults;
};

std::unique_ptr<UnitTestEngine> UnitTestEngine::instance;

void UnitTestEngine::Init()
{
    instance.reset(new UnitTestEngine());
}

void UnitTestEngine::Done()
{
    instance.reset();
}

void UnitTestEngine::StartUnitTest(int32_t numAssertions_, const char* unitTestFilePath_)
{
    numAssertions = numAssertions_;
    unitTestFilePath = unitTestFilePath_;
    for (int32_t i = 0; i < numAssertions; ++i)
    {
        assertionResults.push_back(AssertionResult(assertionResultEmpty, 0));
    }
}

void UnitTestEngine::EndUnitTest(const char* testName, int32_t exitCode)
{
    std::ofstream testXmlFile(unitTestFilePath);
    sngxml::dom::Document document;
    std::unique_ptr<sngxml::dom::Element> testElement(new sngxml::dom::Element(U"test"));
    testElement->SetAttribute(U"name", ToUtf32(testName));
    for (int32_t i = 0; i < numAssertions; ++i)
    {
        std::unique_ptr<sngxml::dom::Element> assertionElement(new sngxml::dom::Element(U"assertion"));
        assertionElement->SetAttribute(U"index", ToUtf32(std::to_string(i)));
        const AssertionResult& assertionResult = assertionResults[i];
        std::u32string assertionResultStr = U"empty";
        if (assertionResult.result == assertionResultPassed)
        {
            assertionResultStr = U"passed";
        }
        else if (assertionResult.result == assertionResulFailed)
        {
            assertionResultStr = U"failed";
        }
        assertionElement->SetAttribute(U"result", assertionResultStr);
        assertionElement->SetAttribute(U"line", ToUtf32(std::to_string(assertionResult.line)));
        testElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(assertionElement.release()));
    }
    testElement->SetAttribute(U"exitCode", ToUtf32(std::to_string(exitCode)));
    testElement->SetAttribute(U"count", ToUtf32(std::to_string(numAssertions)));
    if (!exceptionStr.empty())
    {
        testElement->SetAttribute(U"exception", ToUtf32(exceptionStr));
    }
    document.AppendChild(std::unique_ptr<sngxml::dom::Node>(testElement.release()));
    soulng::util::CodeFormatter formatter(testXmlFile);
    formatter.SetIndentSize(2);
    document.Write(formatter);
}

void UnitTestEngine::SetUnitTestAssertionResult(int32_t assertionIndex, bool assertionResult, int32_t line)
{ 
    AssertionResult& ar = assertionResults[assertionIndex];
    if (assertionResult)
    {
        if (ar.result == assertionResultEmpty || ar.result == assertionResultPassed)
        {
            assertionResults[assertionIndex] = AssertionResult(assertionResultPassed, line);
        }
        else
        {
            assertionResults[assertionIndex] = AssertionResult(assertionResulFailed, line);
        }
    }
    else
    {
        assertionResults[assertionIndex] = AssertionResult(assertionResulFailed, line);
    }
}

} } // namespace cmajor::rt

extern "C" RT_API void RtStartUnitTest(int32_t numAssertions, const char* unitTestFilePath, int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray,
    int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray)
{
    RtInit(numberOfPolymorphicClassIds, polymorphicClassIdArray, numberOfStaticClassIds, staticClassIdArray, nullptr);
    cmajor::rt::UnitTestEngine::Init();
    cmajor::rt::UnitTestEngine::Instance().StartUnitTest(numAssertions, unitTestFilePath);
}

extern "C" RT_API void RtEndUnitTest(const char* testName, int32_t exitCode)
{
    cmajor::rt::UnitTestEngine::Instance().EndUnitTest(testName, exitCode);
    cmajor::rt::UnitTestEngine::Done();
    RtDone();
    RtExit(exitCode);
}

extern "C" RT_API void RtSetUnitTestAssertionResult(int32_t assertionIndex, bool assertionResult, int32_t line)
{
    cmajor::rt::UnitTestEngine::Instance().SetUnitTestAssertionResult(assertionIndex, assertionResult, line);
}

extern "C" RT_API void RtSetUnitTestException(const char* exceptionStr)
{
    cmajor::rt::UnitTestEngine::Instance().SetUnitTestException(exceptionStr);
}
