
// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/TokenLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/TokenLexerKeywords.hpp>
#include <sngcm/cmlexer/TokenLexerTokens.hpp>

namespace TokenLexerKeywords
{
    using namespace TokenLexerTokens;

    Keyword keywords[] =
    {
        {U"assert", ASSERT}, 
        {U"elif", ELIF}, 
        {U"endif", ENDIF}, 
        {U"bool", BOOL}, 
        {U"true", TRUE}, 
        {U"false", FALSE}, 
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
        {U"void", VOID}, 
        {U"enum", ENUM}, 
        {U"cast", CAST}, 
        {U"interface", INTERFACE}, 
        {U"namespace", NAMESPACE}, 
        {U"using", USING}, 
        {U"static", STATIC}, 
        {U"extern", EXTERN}, 
        {U"as", AS}, 
        {U"is", IS}, 
        {U"explicit", EXPLICIT}, 
        {U"delegate", DELEGATE}, 
        {U"inline", INLINE}, 
        {U"cdecl", CDECL}, 
        {U"nothrow", NOTHROW}, 
        {U"public", PUBLIC}, 
        {U"protected", PROTECTED}, 
        {U"private", PRIVATE}, 
        {U"internal", INTERNAL}, 
        {U"virtual", VIRTUAL}, 
        {U"abstract", ABSTRACT}, 
        {U"override", OVERRIDE}, 
        {U"suppress", SUPPRESS}, 
        {U"operator", OPERATOR}, 
        {U"class", CLASS}, 
        {U"return", RETURN}, 
        {U"if", IF}, 
        {U"else", ELSE}, 
        {U"switch", SWITCH}, 
        {U"case", CASE}, 
        {U"default", DEFAULT}, 
        {U"while", WHILE}, 
        {U"do", DO}, 
        {U"for", FOR}, 
        {U"break", BREAK}, 
        {U"continue", CONTINUE}, 
        {U"goto", GOTO}, 
        {U"typedef", TYPEDEF}, 
        {U"typename", TYPENAME}, 
        {U"typeid", TYPEID}, 
        {U"const", CONST}, 
        {U"constexpr", CONSTEXPR}, 
        {U"null", NULLLIT}, 
        {U"this", THIS}, 
        {U"base", BASE}, 
        {U"construct", CONSTRUCT}, 
        {U"destroy", DESTROY}, 
        {U"new", NEW}, 
        {U"delete", DELETE}, 
        {U"sizeof", SIZEOF}, 
        {U"try", TRY}, 
        {U"catch", CATCH}, 
        {U"throw", THROW}, 
        {U"concept", CONCEPT}, 
        {U"where", WHERE}, 
        {U"axiom", AXIOM}, 
        {U"and", AND}, 
        {U"or", OR}, 
        {U"unit_test", UNIT_TEST},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
