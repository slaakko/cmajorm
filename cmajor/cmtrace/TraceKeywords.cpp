
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/cmtrace/TraceLexer.lexer' using soulng lexer generator slg version 3.0.0

#include "TraceKeywords.hpp"
#include "TraceTokens.hpp"

namespace TraceKeywords
{
    using namespace TraceTokens;

    Keyword keywords[] =
    {
        {U"name", NAME}, 
        {U"thread", THREAD}, 
        {U"duration", DURATION}, 
        {U"time", TIME}, 
        {U"parent", PARENT}, 
        {U"level", LEVEL}, 
        {U"message", MESSAGE}, 
        {U"and", AND}, 
        {U"or", OR}, 
        {U"true", TRUE}, 
        {U"false", FALSE}, 
        {U"s", S}, 
        {U"ms", MS}, 
        {U"us", US}, 
        {U"ns", NS}, 
        {U"match", MATCH},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
