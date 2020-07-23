
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebuggerCommand.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebuggerCommandKeywords.hpp>
#include <cmajor/cmdebug/DebuggerCommandTokens.hpp>

namespace DebuggerCommandKeywords
{
    using namespace DebuggerCommandTokens;

    Keyword keywords[] =
    {
        {U"exit", EXIT}, 
        {U"e", EXIT}, 
        {U"quit", EXIT}, 
        {U"q", EXIT}, 
        {U"help", HELP}, 
        {U"h", HELP}, 
        {U"next", NEXT}, 
        {U"n", NEXT}, 
        {U"step", STEP}, 
        {U"s", STEP}, 
        {U"continue", CONTINUE}, 
        {U"c", CONTINUE}, 
        {U"finish", FINISH}, 
        {U"f", FINISH}, 
        {U"until", UNTIL}, 
        {U"u", UNTIL}, 
        {U"break", BREAK}, 
        {U"b", BREAK}, 
        {U"delete", DELETE}, 
        {U"d", DELETE}, 
        {U"print", PRINT}, 
        {U"p", PRINT}, 
        {U"depth", DEPTH}, 
        {U"frames", FRAMES}, 
        {U"list", LIST}, 
        {U"l", LIST}, 
        {U"show", SHOW}, 
        {U"breakpoint", BREAKPOINT}, 
        {U"breakpoints", BREAKPOINTS}, 
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
