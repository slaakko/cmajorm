
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/GdbReply.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/GdbReplyKeywords.hpp>
#include <cmajor/cmdebug/GdbReplyTokens.hpp>

namespace GdbReplyKeywords
{
    using namespace GdbReplyTokens;

    Keyword keywords[] =
    {
        {U"gdb", GDB}, 
        {U"done", DONE}, 
        {U"running", RUNNING}, 
        {U"connected", CONNECTED}, 
        {U"error", ERROR}, 
        {U"exit", EXIT}, 
        {U"stopped", STOPPED},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
