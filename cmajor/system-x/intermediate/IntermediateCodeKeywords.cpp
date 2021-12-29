
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/intermediate/IntermediateCodeLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/intermediate/IntermediateCodeKeywords.hpp>
#include <system-x/intermediate/IntermediateCodeTokens.hpp>

namespace IntermediateCodeKeywords
{
    using namespace IntermediateCodeTokens;

    Keyword keywords[] =
    {
        {U"cu", CU}, 
        {U"types", TYPES}, 
        {U"data", DATA}, 
        {U"function", FUNCTION}, 
        {U"once", ONCE}, 
        {U"store", STORE}, 
        {U"arg", ARG}, 
        {U"jmp", JMP}, 
        {U"branch", BRANCH}, 
        {U"call", CALL}, 
        {U"ret", RET}, 
        {U"switch", SWITCH}, 
        {U"nop", NOP}, 
        {U"save", SAVE}, 
        {U"not", NOT}, 
        {U"neg", NEG}, 
        {U"signextend", SIGNEXTEND}, 
        {U"zeroextend", ZEROEXTEND}, 
        {U"truncate", TRUNCATE}, 
        {U"bitcast", BITCAST}, 
        {U"inttofloat", INTTOFLOAT}, 
        {U"floattoint", FLOATTOINT}, 
        {U"inttoptr", INTTOPTR}, 
        {U"ptrtoint", PTRTOINT}, 
        {U"add", ADD}, 
        {U"sub", SUB}, 
        {U"mul", MUL}, 
        {U"div", DIV}, 
        {U"mod", MOD}, 
        {U"and", AND}, 
        {U"or", OR}, 
        {U"xor", XOR}, 
        {U"shl", SHL}, 
        {U"shr", SHR}, 
        {U"equal", EQUAL}, 
        {U"less", LESS}, 
        {U"param", PARAM}, 
        {U"local", LOCAL}, 
        {U"load", LOAD}, 
        {U"elemaddr", ELEMADDR}, 
        {U"ptroffset", PTROFFSET}, 
        {U"ptrdiff", PTRDIFF}, 
        {U"trap", TRAP}, 
        {U"phi", PHI}, 
        {U"type", TYPE}, 
        {U"offset", OFFSET}, 
        {U"size", SIZE}, 
        {U"alignment", ALIGNMENT}, 
        {U"void", VOID}, 
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
        {U"true", TRUE}, 
        {U"false", FALSE}, 
        {U"null", NULL_TOKEN}, 
        {U"conv", CONV}, 
        {U"metadata", METADATA},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
