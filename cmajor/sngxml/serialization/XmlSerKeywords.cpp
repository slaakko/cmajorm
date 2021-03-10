
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngxml/serialization/XmlSerLexer.lexer' using soulng lexer generator slg version 3.10.0

#include <sngxml/serialization/XmlSerKeywords.hpp>
#include <sngxml/serialization/XmlSerTokens.hpp>

namespace XmlSerKeywords
{
    using namespace XmlSerTokens;

    Keyword keywords[] =
    {
        {U"api", API}, 
        {U"class", CLASS}, 
        {U"base", BASE}, 
        {U"bool", BOOL}, 
        {U"sbyte", SBYTE}, 
        {U"byte", BYTE}, 
        {U"short", SHORT}, 
        {U"ushort", USHORT}, 
        {U"int", INT}, 
        {U"uint", UINT}, 
        {U"long", LONG}, 
        {U"ulong", ULONG}, 
        {U"float", FLOAT}, 
        {U"double", DOUBLE}, 
        {U"char", CHAR}, 
        {U"wchar", WCHAR}, 
        {U"uchar", UCHAR}, 
        {U"uuid", UUID}, 
        {U"string", STRING}, 
        {U"wstring", WSTRING}, 
        {U"ustring", USTRING}, 
        {U"date", DATE}, 
        {U"datetime", DATETIME}, 
        {U"timestamp", TIMESTAMP}, 
        {U"time_point", TIME_POINT}, 
        {U"duration", DURATION}, 
        {U"include", INCLUDE}, 
        {U"namespace", NAMESPACE},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
