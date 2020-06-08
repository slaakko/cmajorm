
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLang.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/build/BuildLangKeywords.hpp>
#include <cmajor/build/BuildLangTokens.hpp>

namespace BuildLangKeywords
{
    using namespace BuildLangTokens;

    Keyword keywords[] =
    {
        {U"add", ADD}, 
        {U"remove", REMOVE}, 
        {U"run", RUN}, 
        {U"port", PORT}, 
        {U"default", DEFAULT}, 
        {U"toolchain", TOOLCHAIN}, 
        {U"clang", CLANG}, 
        {U"gcc", GCC}, 
        {U"vs", VS}, 
        {U"push", PUSH}, 
        {U"build", BUILD}, 
        {U"debug", DEBUG}, 
        {U"release", RELEASE}, 
        {U"install", INSTALL}, 
        {U"to", TO}, 
        {U"from", FROM}, 
        {U"using", USING}, 
        {U"server", SERVER}, 
        {U"verbose", VERBOSE}, 
        {U"force", FORCE}, 
        {U"only", ONLY}, 
        {U"all", ALL}, 
        {U"config", CONFIG},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
