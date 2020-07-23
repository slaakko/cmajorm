
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebugExpr.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugExprKeywords.hpp>
#include <cmajor/cmdebug/DebugExprTokens.hpp>

namespace DebugExprKeywords
{
    using namespace DebugExprTokens;

    Keyword keywords[] =
    {
        {U"cast", CAST}, 
        {U"typeid", TYPEID}, 
        {U"base", BASE},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
