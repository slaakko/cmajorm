
// this file has been automatically generated from 'D:/work/soulng-project/sngjson/json/JsonSchemaLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonSchemaKeywords.hpp>
#include <sngjson/json/JsonSchemaTokens.hpp>

namespace JsonSchemaKeywords
{
    using namespace JsonSchemaTokens;

    Keyword keywords[] =
    {
        {U"struct", STRUCT}, 
        {U"string", STRING}, 
        {U"int", INT}, 
        {U"double", DOUBLE}, 
        {U"bool", BOOL}, 
        {U"api", API}, 
        {U"include", INCLUDE},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
