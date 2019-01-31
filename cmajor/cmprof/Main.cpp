#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/ast/InitDone.hpp>
#include <cmajor/ast/Project.hpp>
#include <cmajor/ast/Solution.hpp>
#include <cmajor/parser/Project.hpp>
#include <cmajor/xpath/InitDone.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/util/InitDone.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Time.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/BinaryReader.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/xpath/XPathEvaluate.hpp>
#include <cmajor/build/Build.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <set>
#include <chrono>
#include <limits>

struct InitDone
{
    InitDone()
    {
        cmajor::ast::Init();
        cmajor::symbols::Init();
        cmajor::parsing::Init();
        cmajor::util::Init();
        cmajor::xpath::Init();
    }
    ~InitDone()
    {
        cmajor::xpath::Done();
        cmajor::util::Done();
        cmajor::parsing::Done();
        cmajor::symbols::Done();
        cmajor::ast::Done();
    }
};

using namespace cmajor::ast;
using namespace cmajor::util;
using namespace cmajor::unicode;
using namespace cmajor::symbols;
using namespace cmajor::parser;
using namespace cmajor::parsing;
using namespace cmajor::build;
using namespace cmajor::xpath;
using namespace cmajor::binder;

enum class Report : uint8_t
{
    none = 0,
    inclusive = 1 << 0,
    exclusive = 1 << 1,
    count = 1 << 2,
    all = inclusive | exclusive | count
};

inline Report operator|(Report left, Report right)
{
    return Report(uint8_t(left) | uint8_t(right));
}

inline Report operator&(Report left, Report right)
{
    return Report(uint8_t(left) & uint8_t(right));
}

struct ProfiledFunction
{
    ProfiledFunction() : functionId(boost::uuids::nil_generator()()), functionName(), recursionCount(0), count(0), elapsedInclusive(0), elapsedExclusive(0), start(), childCalled(false) {}
    boost::uuids::uuid functionId;
    std::u32string functionName;
    int recursionCount;
    int count;
    int64_t elapsedInclusive;
    int64_t elapsedExclusive;
    std::chrono::high_resolution_clock::time_point start;
    bool childCalled;
    void Start(std::chrono::high_resolution_clock::time_point start_)
    {
        if (recursionCount == 0)
        {
            start = start_;
        }
        ++recursionCount;
    }
    void Stop(std::chrono::high_resolution_clock::time_point stop)
    {
        --recursionCount;
        if (recursionCount == 0)
        {
            int64_t elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
            elapsedInclusive += elapsed;
        }
    }
};

struct ProfiledFunctionByName
{
    bool operator()(ProfiledFunction* left, ProfiledFunction* right) const
    {
        if (left->functionName.empty())
        {
            if (right->functionName.empty())
            {
                return left->functionId < right->functionId;
            }
            else
            {
                return false;
            }
        }
        else if (right->functionName.empty())
        {
            if (left->functionName.empty())
            {
                return left->functionId < right->functionId;
            }
            else
            {
                return true;
            }
        }
        else if (left->functionName < right->functionName)
        {
            return true;
        }
        else if (left->functionName > right->functionName)
        {
            return false;
        }
        else
        {
            return left->functionId < right->functionId;
        }
    }
};

struct ProfiledFunctionInclusive
{
    bool operator()(ProfiledFunction* left, ProfiledFunction* right) const
    {
        if (left->elapsedInclusive > right->elapsedInclusive)
        {
            return true;
        }
        else if (left->elapsedInclusive < right->elapsedInclusive)
        {
            return false;
        }
        else if (left->elapsedExclusive > right->elapsedExclusive)
        {
            return true;
        }
        else if (left->elapsedExclusive < right->elapsedExclusive)
        {
            return false;
        }
        else if (left->count > right->count)
        {
            return true;
        }
        else if (left->count < right->count)
        {
            return false;
        }
        else
        {
            return ProfiledFunctionByName()(left, right);
        }
    }
};

struct ProfiledFunctionExclusive
{
    bool operator()(ProfiledFunction* left, ProfiledFunction* right) const
    {
        if (left->elapsedExclusive > right->elapsedExclusive)
        {
            return true;
        }
        else if (left->elapsedExclusive < right->elapsedExclusive)
        {
            return false;
        }
        else if (left->elapsedInclusive > right->elapsedInclusive)
        {
            return true;
        }
        else if (left->elapsedInclusive < right->elapsedInclusive)
        {
            return false;
        }
        else if (left->count > right->count)
        {
            return true;
        }
        else if (left->count < right->count)
        {
            return false;
        }
        else
        {
            return ProfiledFunctionByName()(left, right);
        }
    }
};

struct ProfiledFunctionByCount
{
    bool operator()(ProfiledFunction* left, ProfiledFunction* right) const
    {
        if (left->count > right->count)
        {
            return true;
        }
        else if (left->count < right->count)
        {
            return false;
        }
        else if (left->elapsedInclusive > right->elapsedInclusive)
        {
            return true;
        }
        else if (left->elapsedInclusive < right->elapsedInclusive)
        {
            return false;
        }
        else if (left->elapsedExclusive > right->elapsedExclusive)
        {
            return true;
        }
        else if (left->elapsedExclusive < right->elapsedExclusive)
        {
            return false;
        }
        else
        {
            return ProfiledFunctionByName()(left, right);
        }
    }
};

std::unique_ptr<cmajor::dom::Document> GenerateReport(Module& module, std::vector<ProfiledFunction*>& profiledFunctions, Report report, int top, int64_t totalInclusive, int64_t totalExclusive)
{
    std::unique_ptr<cmajor::dom::Document> reportDoc(new cmajor::dom::Document());
    std::unique_ptr<cmajor::dom::Element> htmlElement(new cmajor::dom::Element(U"html"));
    std::unique_ptr<cmajor::dom::Element> headElement(new cmajor::dom::Element(U"head"));
    std::unique_ptr<cmajor::dom::Element> titleElement(new cmajor::dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Profile Report"))); 
    std::unique_ptr<cmajor::dom::Element> styleElement(new cmajor::dom::Element(U"style"));
    std::u32string style =
        U"body { max-width: 800px; } h1, h2, h3, h4, h5, h6 { color: #005ab4; font-family: sans-serif; } table { boder-collapse: collapse; } table, th, td { text-align: left; border: 1px solid #dddddd; padding: 8px; }";
    styleElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(style)));
    std::unique_ptr<cmajor::dom::Element> bodyElement(new cmajor::dom::Element(U"body"));
    std::unique_ptr<cmajor::dom::Element> h1Element(new cmajor::dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Profile Report")));
    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h1Element.release()));

    std::unique_ptr<cmajor::dom::Element> paramTableElement(new cmajor::dom::Element(U"table"));

    std::unique_ptr<cmajor::dom::Element> trProjectElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thProjectElement(new cmajor::dom::Element(U"th"));
    thProjectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"project")));
    trProjectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thProjectElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdProjectElement(new cmajor::dom::Element(U"td"));
    tdProjectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(module.Name())));
    trProjectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdProjectElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trProjectElement.release()));

    std::unique_ptr<cmajor::dom::Element> trTopElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thTopElement(new cmajor::dom::Element(U"th"));
    thTopElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"top")));
    trTopElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thTopElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdTopElement(new cmajor::dom::Element(U"td"));
    std::u32string topStr = U"*";
    if (top != 0)
    {
        topStr = ToUtf32(std::to_string(top));
    }
    tdTopElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(topStr)));
    trTopElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdTopElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trTopElement.release()));

    std::unique_ptr<cmajor::dom::Element> trElapsedElement(new cmajor::dom::Element(U"tr"));
    std::unique_ptr<cmajor::dom::Element> thElapsedElement(new cmajor::dom::Element(U"th"));
    thElapsedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"elapsed")));
    trElapsedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thElapsedElement.release()));
    std::unique_ptr<cmajor::dom::Element> tdElapsedElement(new cmajor::dom::Element(U"td"));
    std::u32string elapsedStr = ToUtf32(FormatTimeMs(totalInclusive / 1000000));
    tdElapsedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(elapsedStr)));
    trElapsedElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdElapsedElement.release()));
    paramTableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trElapsedElement.release()));

    bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(paramTableElement.release()));

    if ((report & Report::inclusive) != Report::none)
    {
        std::unique_ptr<cmajor::dom::Element> h2Element(new cmajor::dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Elapsed Time Inclusive")));
        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h2Element.release()));
        std::unique_ptr<cmajor::dom::Element> tableElement(new cmajor::dom::Element(U"table"));

        std::unique_ptr<cmajor::dom::Element> trTitlesElement(new cmajor::dom::Element(U"tr"));
        std::unique_ptr<cmajor::dom::Element> thRankElement(new cmajor::dom::Element(U"th"));
        thRankElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"rank")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thRankElement.release()));
        std::unique_ptr<cmajor::dom::Element> thFunctionElement(new cmajor::dom::Element(U"th"));
        thFunctionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"function")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thFunctionElement.release()));
        std::unique_ptr<cmajor::dom::Element> thInclusiveElement(new cmajor::dom::Element(U"th"));
        thInclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"elapsed time inclusive")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thInclusiveElement.release()));
        std::unique_ptr<cmajor::dom::Element> thInclusivePercentElement(new cmajor::dom::Element(U"th"));
        thInclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"inclusive-%")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thInclusivePercentElement.release()));
        std::unique_ptr<cmajor::dom::Element> thExclusiveElement(new cmajor::dom::Element(U"th"));
        thExclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"elapsed time exclusive")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExclusiveElement.release()));
        std::unique_ptr<cmajor::dom::Element> thExclusivePercentElement(new cmajor::dom::Element(U"th"));
        thExclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"exclusive-%")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExclusivePercentElement.release()));
        std::unique_ptr<cmajor::dom::Element> thCountElement(new cmajor::dom::Element(U"th"));
        thCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"execution count")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thCountElement.release()));
        tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trTitlesElement.release()));

        std::sort(profiledFunctions.begin(), profiledFunctions.end(), ProfiledFunctionInclusive());
        int n = std::min(top, int(profiledFunctions.size()));
        if (n == 0)
        {
            n = profiledFunctions.size();
        }
        for (int i = 0; i < n; ++i)
        {
            ProfiledFunction* fun = profiledFunctions[i];
            std::unique_ptr<cmajor::dom::Element> trElement(new cmajor::dom::Element(U"tr"));
            std::unique_ptr<cmajor::dom::Element> tdRanklement(new cmajor::dom::Element(U"td"));
            tdRanklement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(i + 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdRanklement.release()));
            std::unique_ptr<cmajor::dom::Element> tdFunctionElement(new cmajor::dom::Element(U"td"));
            tdFunctionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(fun->functionName)));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdFunctionElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdInclusiveElement(new cmajor::dom::Element(U"td"));
            tdInclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(FormatTimeMs(fun->elapsedInclusive / 1000000)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdInclusiveElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdInclusivePercentElement(new cmajor::dom::Element(U"td"));
            tdInclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(ToString(100.0 * fun->elapsedInclusive / totalInclusive, 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdInclusivePercentElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdExclusiveElement(new cmajor::dom::Element(U"td"));
            tdExclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(FormatTimeMs(fun->elapsedExclusive / 1000000)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExclusiveElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdExclusivePercentElement(new cmajor::dom::Element(U"td"));
            tdExclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(ToString(100.0 * fun->elapsedExclusive / totalExclusive, 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExclusivePercentElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdCountElement(new cmajor::dom::Element(U"td"));
            tdCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(fun->count)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdCountElement.release()));
            tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tableElement.release()));
    }

    if ((report & Report::exclusive) != Report::none)
    {
        std::unique_ptr<cmajor::dom::Element> h2Element(new cmajor::dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Elapsed Time Exclusive")));
        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h2Element.release()));
        std::unique_ptr<cmajor::dom::Element> tableElement(new cmajor::dom::Element(U"table"));

        std::unique_ptr<cmajor::dom::Element> trTitlesElement(new cmajor::dom::Element(U"tr"));
        std::unique_ptr<cmajor::dom::Element> thRankElement(new cmajor::dom::Element(U"th"));
        thRankElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"rank")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thRankElement.release()));
        std::unique_ptr<cmajor::dom::Element> thFunctionElement(new cmajor::dom::Element(U"th"));
        thFunctionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"function")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thFunctionElement.release()));
        std::unique_ptr<cmajor::dom::Element> thExclusiveElement(new cmajor::dom::Element(U"th"));
        thExclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"elapsed time exclusive")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExclusiveElement.release()));
        std::unique_ptr<cmajor::dom::Element> thExclusivePercentElement(new cmajor::dom::Element(U"th"));
        thExclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"exclusive-%")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExclusivePercentElement.release()));
        std::unique_ptr<cmajor::dom::Element> thInclusiveElement(new cmajor::dom::Element(U"th"));
        thInclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"elapsed time inclusive")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thInclusiveElement.release()));
        std::unique_ptr<cmajor::dom::Element> thInclusivePercentElement(new cmajor::dom::Element(U"th"));
        thInclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"inclusive-%")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thInclusivePercentElement.release()));
        std::unique_ptr<cmajor::dom::Element> thCountElement(new cmajor::dom::Element(U"th"));
        thCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"execution count")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thCountElement.release()));
        tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trTitlesElement.release()));

        std::sort(profiledFunctions.begin(), profiledFunctions.end(), ProfiledFunctionExclusive());
        int n = std::min(top, int(profiledFunctions.size()));
        if (n == 0)
        {
            n = profiledFunctions.size();
        }
        for (int i = 0; i < n; ++i)
        {
            ProfiledFunction* fun = profiledFunctions[i];
            std::unique_ptr<cmajor::dom::Element> trElement(new cmajor::dom::Element(U"tr"));
            std::unique_ptr<cmajor::dom::Element> tdRanklement(new cmajor::dom::Element(U"td"));
            tdRanklement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(i + 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdRanklement.release()));
            std::unique_ptr<cmajor::dom::Element> tdFunctionElement(new cmajor::dom::Element(U"td"));
            tdFunctionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(fun->functionName)));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdFunctionElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdExclusiveElement(new cmajor::dom::Element(U"td"));
            tdExclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(FormatTimeMs(fun->elapsedExclusive / 1000000)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExclusiveElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdExclusivePercentElement(new cmajor::dom::Element(U"td"));
            tdExclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(ToString(100.0 * fun->elapsedExclusive / totalExclusive, 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExclusivePercentElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdInclusiveElement(new cmajor::dom::Element(U"td"));
            tdInclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(FormatTimeMs(fun->elapsedInclusive / 1000000)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdInclusiveElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdInclusivePercentElement(new cmajor::dom::Element(U"td"));
            tdInclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(ToString(100.0 * fun->elapsedInclusive / totalInclusive, 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdInclusivePercentElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdCountElement(new cmajor::dom::Element(U"td"));
            tdCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(fun->count)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdCountElement.release()));
            tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tableElement.release()));
    }
    if ((report & Report::count) != Report::none)
    {
        std::unique_ptr<cmajor::dom::Element> h2Element(new cmajor::dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"Execution Count")));
        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(h2Element.release()));
        std::unique_ptr<cmajor::dom::Element> tableElement(new cmajor::dom::Element(U"table"));

        std::unique_ptr<cmajor::dom::Element> trTitlesElement(new cmajor::dom::Element(U"tr"));
        std::unique_ptr<cmajor::dom::Element> thRankElement(new cmajor::dom::Element(U"th"));
        thRankElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"rank")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thRankElement.release()));
        std::unique_ptr<cmajor::dom::Element> thFunctionElement(new cmajor::dom::Element(U"th"));
        thFunctionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"function")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thFunctionElement.release()));
        std::unique_ptr<cmajor::dom::Element> thCountElement(new cmajor::dom::Element(U"th"));
        thCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"execution count")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thCountElement.release()));
        std::unique_ptr<cmajor::dom::Element> thInclusiveElement(new cmajor::dom::Element(U"th"));
        thInclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"elapsed time inclusive")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thInclusiveElement.release()));
        std::unique_ptr<cmajor::dom::Element> thInclusivePercentElement(new cmajor::dom::Element(U"th"));
        thInclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"inclusive-%")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thInclusivePercentElement.release()));
        std::unique_ptr<cmajor::dom::Element> thExclusiveElement(new cmajor::dom::Element(U"th"));
        thExclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"elapsed time exclusive")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExclusiveElement.release()));
        std::unique_ptr<cmajor::dom::Element> thExclusivePercentElement(new cmajor::dom::Element(U"th"));
        thExclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(U"exclusive-%")));
        trTitlesElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(thExclusivePercentElement.release()));
        tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trTitlesElement.release()));

        std::sort(profiledFunctions.begin(), profiledFunctions.end(), ProfiledFunctionByCount());
        int n = std::min(top, int(profiledFunctions.size()));
        if (n == 0)
        {
            n = profiledFunctions.size();
        }
        for (int i = 0; i < n; ++i)
        {
            ProfiledFunction* fun = profiledFunctions[i];
            std::unique_ptr<cmajor::dom::Element> trElement(new cmajor::dom::Element(U"tr"));
            std::unique_ptr<cmajor::dom::Element> tdRanklement(new cmajor::dom::Element(U"td"));
            tdRanklement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(i + 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdRanklement.release()));
            std::unique_ptr<cmajor::dom::Element> tdFunctionElement(new cmajor::dom::Element(U"td"));
            tdFunctionElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(fun->functionName)));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdFunctionElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdCountElement(new cmajor::dom::Element(U"td"));
            tdCountElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(std::to_string(fun->count)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdCountElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdInclusiveElement(new cmajor::dom::Element(U"td"));
            tdInclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(FormatTimeMs(fun->elapsedInclusive / 1000000)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdInclusiveElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdInclusivePercentElement(new cmajor::dom::Element(U"td"));
            tdInclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(ToString(100.0 * fun->elapsedInclusive / totalInclusive, 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdInclusivePercentElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdExclusiveElement(new cmajor::dom::Element(U"td"));
            tdExclusiveElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(FormatTimeMs(fun->elapsedExclusive / 1000000)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExclusiveElement.release()));
            std::unique_ptr<cmajor::dom::Element> tdExclusivePercentElement(new cmajor::dom::Element(U"td"));
            tdExclusivePercentElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(new cmajor::dom::Text(ToUtf32(ToString(100.0 * fun->elapsedExclusive / totalExclusive, 1)))));
            trElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tdExclusivePercentElement.release()));
            tableElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(tableElement.release()));
    }
    headElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(titleElement.release()));
    headElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(styleElement.release()));
    htmlElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(headElement.release()));
    htmlElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(bodyElement.release()));
    reportDoc->AppendChild(std::unique_ptr<cmajor::dom::Node>(htmlElement.release()));
    return reportDoc;
}

const uint8_t startEvent = 0;
const uint8_t endEvent = 1;

std::unique_ptr<cmajor::dom::Document> AnalyzeProfileData(const std::string& profileDataFileName, Module& module,
    std::unordered_map<boost::uuids::uuid, ProfiledFunction, boost::hash<boost::uuids::uuid>>& functionProfileMap, std::vector<ProfiledFunction*>& profiledFunctions, int64_t& totalInclusive, int64_t& totalExclusive)
{
    std::unique_ptr<cmajor::dom::Document> analyzedProfileDataDoc(new cmajor::dom::Document());
    std::vector<ProfiledFunction*> functionPath;
    std::chrono::high_resolution_clock::time_point prevTimePoint;
    std::chrono::high_resolution_clock::time_point start { std::chrono::high_resolution_clock::duration{ std::numeric_limits<int64_t>::max() } };
    std::chrono::high_resolution_clock::time_point end { std::chrono::high_resolution_clock::duration{ std::numeric_limits<int64_t>::min() } };
    bool prevWasEnd = false;
    BinaryReader reader(profileDataFileName);
    uint64_t n = reader.ReadULong();
    for (uint64_t i = 0; i < n; ++i)
    {
        boost::uuids::uuid functionId;
        reader.ReadUuid(functionId);
        uint64_t timePointCount = reader.ReadULong();
        uint8_t eventKind = reader.ReadByte();
        std::chrono::high_resolution_clock::time_point timePoint{ std::chrono::high_resolution_clock::duration{ timePointCount } };
        if (timePoint < start)
        {
            start = timePoint;
        }
        if (timePoint > end)
        {
            end = timePoint;
        }
        if (i == 0 || prevTimePoint > timePoint)
        {
            prevTimePoint = timePoint;
        }
        ProfiledFunction& fun = functionProfileMap[functionId];
        if (fun.functionId.is_nil())
        {
            fun.functionId = functionId;
            fun.functionName = module.GetSymbolTable().GetProfiledFunctionName(functionId);
            if (fun.functionName.empty())
            {
                //fun.functionName = ToUtf32(std::to_string(functionId));
                fun.functionName = ToUtf32(boost::uuids::to_string(functionId));
            }
        }
        if (eventKind == startEvent)
        {
            if (!functionPath.empty())
            {
                ProfiledFunction* parentFun = functionPath.back();
                parentFun->elapsedExclusive = std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint - prevTimePoint).count();
                parentFun->childCalled = true;
            }
            fun.Start(timePoint);
            ++fun.count;
            functionPath.push_back(&fun);
            prevWasEnd = false;
        }
        else if (eventKind == endEvent)
        {
            if (!functionPath.empty() && prevWasEnd)
            {
                ProfiledFunction* parentFun = functionPath.back();
                parentFun->elapsedExclusive = std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint - prevTimePoint).count();
            }
            fun.Stop(timePoint);
            functionPath.pop_back();
            if (!fun.childCalled)
            {
                fun.elapsedExclusive += std::chrono::duration_cast<std::chrono::nanoseconds>(timePoint - prevTimePoint).count();
            }
            prevWasEnd = true;
        }
        else
        {
            throw std::runtime_error("'start' or 'end' event expected");
        }
        prevTimePoint = timePoint;
    }
    for (auto& p : functionProfileMap)
    {
        profiledFunctions.push_back(&p.second);
    }
    std::sort(profiledFunctions.begin(), profiledFunctions.end(), ProfiledFunctionByName());
    totalInclusive = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    totalExclusive = 0;
    for (ProfiledFunction* fun : profiledFunctions)
    {
        totalExclusive += fun->elapsedExclusive;
    }
    cmajor::dom::Element* profileElement = new cmajor::dom::Element(U"profile");
    profileElement->SetAttribute(U"project", module.Name());
    profileElement->SetAttribute(U"elapsed", ToUtf32(std::to_string(totalInclusive)));
    for (ProfiledFunction* fun : profiledFunctions)
    {
        cmajor::dom::Element* functionElement = new cmajor::dom::Element(U"function");
        functionElement->SetAttribute(U"id", ToUtf32(boost::uuids::to_string(fun->functionId)));
        functionElement->SetAttribute(U"name", fun->functionName);
        functionElement->SetAttribute(U"count", ToUtf32(std::to_string(fun->count)));
        functionElement->SetAttribute(U"elapsedInclusive", ToUtf32(std::to_string(fun->elapsedInclusive)));
        functionElement->SetAttribute(U"elapsedInclusivePercent", ToUtf32(ToString(100.0 * fun->elapsedInclusive / totalInclusive, 1)));
        functionElement->SetAttribute(U"elapsedExclusive", ToUtf32(std::to_string(fun->elapsedExclusive)));
        functionElement->SetAttribute(U"elapsedExclusivePercent", ToUtf32(ToString(100.0 * fun->elapsedExclusive / totalExclusive, 1)));
        profileElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(functionElement));
    }
    analyzedProfileDataDoc->AppendChild(std::unique_ptr<cmajor::dom::Node>(profileElement));
    return analyzedProfileDataDoc;
}

cmajor::parser::Project* projectGrammar = nullptr;

void ReadProject(const std::string& projectFilePath, cmajor::ast::Solution& solution, bool requireProgram, std::set<std::u32string>& readProjects)
{
    if (!projectGrammar)
    {
        projectGrammar = cmajor::parser::Project::Create();
    }
    MappedInputFile projectFile(projectFilePath);
    std::u32string p(ToUtf32(std::string(projectFile.Begin(), projectFile.End())));
    std::string config = "profile";
    std::unique_ptr<cmajor::ast::Project> project(projectGrammar->Parse(&p[0], &p[0] + p.length(), 0, projectFilePath, config, cmajor::ast::BackEnd::llvm));
    if (!IsSystemModule(project->Name()))
    {
        std::string systemProjectFilePath = Path::Combine(Path::Combine(Path::Combine(CmajorRootDir(), "system"), "System"), "System.cmp");
        project->AddDeclaration(new ReferenceDeclaration(systemProjectFilePath));
    }
    else
    {
        project->SetSystemProject();
    }
    project->ResolveDeclarations();
    if (requireProgram)
    {
        if (project->GetTarget() != Target::program)
        {
            throw std::runtime_error("project '" + ToUtf8(project->Name()) + "' is not a program project");
        }
    }
    cmajor::ast::Project* proj = project.get();
    if (readProjects.find(proj->Name()) == readProjects.cend())
    {
        solution.AddProject(std::move(project));
    }
    for (const std::string& projectReference : proj->ReferencedProjectFilePaths())
    {
        ReadProject(projectReference, solution, false, readProjects);
    }
}

void ProfileProject(const std::string& projectFilePath, bool rebuildSys, bool rebuildApp, int top, Report report, std::string& outFile, const std::string& args, std::unique_ptr<Module>& rootModule)
{
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Building projects..." << std::endl;
    }
    Solution solution(U"solution", "solution.cms");
    bool requireProgram = true;
    std::set<std::u32string> readProjects;
    ReadProject(projectFilePath, solution, requireProgram, readProjects);
    std::vector<cmajor::ast::Project*> projects = solution.CreateBuildOrder();
    std::string systemModuleFilePath;
    for (cmajor::ast::Project* project : projects)
    {
        if (project->IsSystemProject())
        {
            if (rebuildSys || !project->IsUpToDate(systemModuleFilePath))
            {
                rebuildSys = true;
                bool stop = false;
                BuildProject(project, rootModule, stop, false);
            }
            else if (GetGlobalFlag(GlobalFlags::verbose))
            {
                std::cout << "project '" << ToUtf8(project->Name()) << "' is up-to-date" << std::endl;
            }
            if (project->Name() == U"System")
            {
                systemModuleFilePath = project->ModuleFilePath();
            }
        }
        else
        {
            if (rebuildApp || rebuildSys || !project->IsUpToDate(systemModuleFilePath))
            {
                rebuildApp = true;
                bool stop = false;
                BuildProject(project, rootModule, stop, false);
            }
            else if (GetGlobalFlag(GlobalFlags::verbose))
            {
                std::cout << "project '" << ToUtf8(project->Name()) << "' is up-to-date" << std::endl;
            }
        }
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Finished building projects." << std::endl;
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Reading main module..." << std::endl;
    }
    cmajor::ast::Project* mainProject = projects.back();
    std::string moduleFilePath = mainProject->ModuleFilePath();
    rootModule.reset(new Module(moduleFilePath));
    rootModule->SetRootModule();
    SetRootModuleForCurrentThread(rootModule.get());
    CompileUnitNode compileUnit(Span(), "foo");
    AttributeBinder attributeBinder(rootModule.get());
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Finished reading main module." << std::endl;
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Profiling '" << mainProject->ExecutableFilePath() << "'..." << std::endl;
    }
    std::string commandLine = mainProject->ExecutableFilePath();
    if (!args.empty())
    {
        commandLine.append(1, ' ').append(args);
    }
    int exitCode = system(commandLine.c_str());
    if (exitCode != 0)
    {
        std::cout << mainProject->ExecutableFilePath() << " returned exit code " << exitCode << std::endl;
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Finished profiling." << std::endl;
    }
    std::string profileDataFileName = Path::Combine(Path::GetDirectoryName(mainProject->ExecutableFilePath()), "cmprof.bin");
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Analyzing profile data..." << std::endl;
    }
    std::unordered_map<boost::uuids::uuid, ProfiledFunction, boost::hash<boost::uuids::uuid>> functionProfileMap;
    std::vector<ProfiledFunction*> profiledFunctions;
    int64_t totalInclusive = 0;
    int64_t totalExclusive = 0;
    std::unique_ptr<cmajor::dom::Document> analyzedProfileDataDoc = AnalyzeProfileData(profileDataFileName, *rootModule, functionProfileMap, profiledFunctions, totalInclusive, totalExclusive);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Finished analyzing profile data." << std::endl;
    }
    if (outFile.empty())
    {
        outFile = "cmprof";
        outFile.append("-").append(GetCurrentDateTime().ToString(true, true, false, false));
    }
    boost::filesystem::path xmlFilePath = boost::filesystem::path(outFile).replace_extension(".xml");
    std::string cmProfFileName = GetFullPath(xmlFilePath.generic_string());
    std::ofstream cmProfXmlFile(cmProfFileName);
    cmajor::util::CodeFormatter xmlFormatter(cmProfXmlFile);
    xmlFormatter.SetIndentSize(2);
    analyzedProfileDataDoc->Write(xmlFormatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "=> " << cmProfFileName << std::endl;
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Generating report..." << std::endl;
    }
    boost::filesystem::path htmlFilePath = boost::filesystem::path(outFile).replace_extension(".html");
    std::unique_ptr<cmajor::dom::Document> reportHtmlDoc = GenerateReport(*rootModule, profiledFunctions, report, top, totalInclusive, totalExclusive);
    std::string reportHtmlFileName = GetFullPath(htmlFilePath.generic_string());
    std::ofstream reportHtmlFile(reportHtmlFileName);
    cmajor::util::CodeFormatter htmlFormatter(reportHtmlFile);
    htmlFormatter.SetIndentSize(2);
    reportHtmlDoc->Write(htmlFormatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "=> " << reportHtmlFileName << std::endl;
    }
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "Finished generating report." << std::endl;
    }
}

const char* version = "3.3.0";

void PrintHelp()
{
    std::cout << "Cmajor Profiler version " << version << std::endl;
    std::cout << "Usage: cmprof [options] { project.cmp }" << std::endl;
    std::cout << "Profiles given Cmajor program projects." << std::endl;
    std::cout << "Options:\n" <<
        "--help (-h)\n" <<
        "   print this help message\n" <<
        "--verbose (-v)\n" <<
        "   print verbose messages\n" <<
        "--rebuild-sys (-rs)\n" <<
        "   rebuild system libraries\n" <<
        "--rebuild-app (-ra)\n" <<
        "   rebuild application libraries & program\n" <<
        "--inclusive (-i)\n"
        "   report elapsed inclusive time\n" <<
        "--exclusive (-x)\n"
        "   report elapsed exclusive time\n" <<
        "--count (-c)\n" <<
        "   report execution count\n" <<
        "--all (-a)\n" <<
        "   report all\n" <<
        "--top=N (-t=N)\n" <<
        "   report top N functions (default=all)\n" <<
        "--out=FILE (-o=FILE)\n" <<
        "   report to file FILE\n" <<
        "--args=\"ARGUMENTS\"\n" <<
        "   set program arguments\n" <<
        "--emit-llvm (-l)\n" <<
        "   emit intermediate LLVM code to file.ll files\n" <<
        "--emit-opt-llvm (-p)\n" <<
        "   emit optimized intermediate LLVM code to file.opt.ll files\n" <<
        std::endl;
}

int main(int argc, const char** argv)
{
    std::unique_ptr<Module> rootModule;
    try
    { 
        InitDone initDone;
        SetCompilerVersion(version);
        SetGlobalFlag(GlobalFlags::release);
        SetGlobalFlag(GlobalFlags::profile);
        std::vector<std::string> projects;
        bool rebuildSys = false;
        bool rebuildApp = false;
        int top = 0;
        Report report = Report::none;
        std::string outFile;
        std::string args;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!arg.empty() && arg[0] == '-')
            {
                if (arg == "--help" || arg == "-h")
                {
                    PrintHelp();
                    return 0;
                }
                else if (arg == "--verbose" || arg == "-v")
                {
                    SetGlobalFlag(GlobalFlags::verbose);
                }
                else if (arg == "--rebuild-sys" || arg == "-rs")
                {
                    rebuildSys = true;
                }
                else if (arg == "--rebuild-app" || arg == "-ra")
                {
                    rebuildApp = true;
                }
                else if (arg == "--inclusive" || arg == "-i")
                {
                    report = report | Report::inclusive;
                }
                else if (arg == "--exclusive" || arg == "-x")
                {
                    report = report | Report::exclusive;
                }
                else if (arg == "--count" || arg == "-c")
                {
                    report = report | Report::count;
                }
                else if (arg == "--all" || arg == "-a")
                {
                    report = report | Report::all;
                }
                else if (arg == "--emit-llvm" || arg == "-l")
                {
                    SetGlobalFlag(GlobalFlags::emitLlvm);
                }
                else if (arg == "--emit-opt-llvm" || arg == "-p")
                {
                    SetGlobalFlag(GlobalFlags::emitLlvm);
                    SetGlobalFlag(GlobalFlags::emitOptLlvm);
                }
                else if (arg == "--link-with-debug-runtime" || arg == "-d")
                {
                    SetGlobalFlag(GlobalFlags::linkWithDebugRuntime);
                }
                else if (arg == "--link-using-ms-link" || arg == "-m")
                {
                    SetGlobalFlag(GlobalFlags::linkUsingMsLink);
                }
                else if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(arg, '=');
                    if (components.size() == 2)
                    {
                        if (components[0] == "--top" || components[0] == "-t")
                        {
                            if (components[1] != "*")
                            {
                                top = boost::lexical_cast<int>(components[1]);
                            }
                        }
                        else if (components[0] == "--out" || components[0] == "-o")
                        {
                            outFile = components[1];
                        }
                        else if (components[0] == "--args")
                        {
                            args = components[1];
                        }
                        else
                        {
                            throw std::runtime_error("unknown option '" + arg + "'");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else
            {
                projects.push_back(arg);
            }
        }
        if (projects.empty())
        {
            PrintHelp();
            throw std::runtime_error("no projects and/or solutions given");
        }
        if (report == Report::none)
        {
            report = Report::all;
        }
        if (GetGlobalFlag(GlobalFlags::verbose))
        {
            std::cout << "Cmajor profiler version " << version << std::endl;
        }
        for (const std::string& project : projects)
        {
            boost::filesystem::path fp(project);
            if (fp.extension() == ".cmp")
            {
                if (!boost::filesystem::exists(fp))
                {
                    throw std::runtime_error("project file '" + fp.generic_string() + " not found");
                }
                else
                {
                    ProfileProject(GetFullPath(project), rebuildSys, rebuildApp, top, report, outFile, args, rootModule);
                }
            }
            else
            {
                throw std::runtime_error("Argument '" + fp.generic_string() + "' has invalid extension. Not a Cmajor project (.cmp) file.");
            }
        }
    }
    catch (const ParsingException& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const Exception& ex)
    {
        std::cerr << ex.What() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
