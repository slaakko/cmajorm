
// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/CmajorLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/CmajorTokens.hpp>
#include <map>

namespace CmajorTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ABSTRACT"] = 40;
        tokenIdMap[U"AMP"] = 91;
        tokenIdMap[U"AMPAMP"] = 88;
        tokenIdMap[U"AND"] = 76;
        tokenIdMap[U"ARROW"] = 107;
        tokenIdMap[U"AS"] = 28;
        tokenIdMap[U"ASSERT"] = 1;
        tokenIdMap[U"ASSIGN"] = 117;
        tokenIdMap[U"AXIOM"] = 75;
        tokenIdMap[U"BASE"] = 64;
        tokenIdMap[U"BITOR"] = 89;
        tokenIdMap[U"BITXOR"] = 90;
        tokenIdMap[U"BOOL"] = 4;
        tokenIdMap[U"BREAK"] = 54;
        tokenIdMap[U"BYTE"] = 8;
        tokenIdMap[U"CASE"] = 49;
        tokenIdMap[U"CAST"] = 22;
        tokenIdMap[U"CATCH"] = 71;
        tokenIdMap[U"CDECL"] = 33;
        tokenIdMap[U"CHAR"] = 17;
        tokenIdMap[U"CHARLIT"] = 83;
        tokenIdMap[U"CLASS"] = 44;
        tokenIdMap[U"COLON"] = 118;
        tokenIdMap[U"COMMA"] = 116;
        tokenIdMap[U"CONCEPT"] = 73;
        tokenIdMap[U"CONST"] = 60;
        tokenIdMap[U"CONSTEXPR"] = 61;
        tokenIdMap[U"CONSTRUCT"] = 65;
        tokenIdMap[U"CONTINUE"] = 55;
        tokenIdMap[U"CPL"] = 105;
        tokenIdMap[U"DEFAULT"] = 50;
        tokenIdMap[U"DELEGATE"] = 31;
        tokenIdMap[U"DELETE"] = 68;
        tokenIdMap[U"DESTROY"] = 66;
        tokenIdMap[U"DISJUNCTION"] = 87;
        tokenIdMap[U"DIV"] = 101;
        tokenIdMap[U"DO"] = 52;
        tokenIdMap[U"DOT"] = 106;
        tokenIdMap[U"DOUBLE"] = 16;
        tokenIdMap[U"ELIF"] = 2;
        tokenIdMap[U"ELSE"] = 47;
        tokenIdMap[U"ENDIF"] = 3;
        tokenIdMap[U"ENUM"] = 21;
        tokenIdMap[U"EQ"] = 92;
        tokenIdMap[U"EQUIVALENCE"] = 85;
        tokenIdMap[U"EXCLAMATION"] = 78;
        tokenIdMap[U"EXPLICIT"] = 30;
        tokenIdMap[U"EXTERN"] = 27;
        tokenIdMap[U"FALSE"] = 6;
        tokenIdMap[U"FLOAT"] = 15;
        tokenIdMap[U"FLOATINGLIT"] = 81;
        tokenIdMap[U"FOR"] = 53;
        tokenIdMap[U"GEQ"] = 95;
        tokenIdMap[U"GOTO"] = 56;
        tokenIdMap[U"HASH"] = 120;
        tokenIdMap[U"ID"] = 80;
        tokenIdMap[U"IF"] = 46;
        tokenIdMap[U"IMPLICATION"] = 86;
        tokenIdMap[U"INLINE"] = 32;
        tokenIdMap[U"INT"] = 11;
        tokenIdMap[U"INTERFACE"] = 23;
        tokenIdMap[U"INTERNAL"] = 38;
        tokenIdMap[U"INTLIT"] = 82;
        tokenIdMap[U"IS"] = 29;
        tokenIdMap[U"LANGLE"] = 112;
        tokenIdMap[U"LBRACE"] = 114;
        tokenIdMap[U"LBRACKET"] = 108;
        tokenIdMap[U"LEQ"] = 94;
        tokenIdMap[U"LONG"] = 13;
        tokenIdMap[U"LPAREN"] = 110;
        tokenIdMap[U"MINUS"] = 99;
        tokenIdMap[U"MINUSMINUS"] = 104;
        tokenIdMap[U"NAMESPACE"] = 24;
        tokenIdMap[U"NEQ"] = 93;
        tokenIdMap[U"NEW"] = 67;
        tokenIdMap[U"NOTHROW"] = 34;
        tokenIdMap[U"NULLLIT"] = 62;
        tokenIdMap[U"OPERATOR"] = 43;
        tokenIdMap[U"OR"] = 77;
        tokenIdMap[U"OVERRIDE"] = 41;
        tokenIdMap[U"PLUS"] = 98;
        tokenIdMap[U"PLUSPLUS"] = 103;
        tokenIdMap[U"PRIVATE"] = 37;
        tokenIdMap[U"PROTECTED"] = 36;
        tokenIdMap[U"PUBLIC"] = 35;
        tokenIdMap[U"RANGLE"] = 113;
        tokenIdMap[U"RBRACE"] = 115;
        tokenIdMap[U"RBRACKET"] = 109;
        tokenIdMap[U"REM"] = 102;
        tokenIdMap[U"RETURN"] = 45;
        tokenIdMap[U"RPAREN"] = 111;
        tokenIdMap[U"SBYTE"] = 7;
        tokenIdMap[U"SEMICOLON"] = 119;
        tokenIdMap[U"SHIFTLEFT"] = 96;
        tokenIdMap[U"SHIFTRIGHT"] = 97;
        tokenIdMap[U"SHORT"] = 9;
        tokenIdMap[U"SIZEOF"] = 69;
        tokenIdMap[U"STAR"] = 100;
        tokenIdMap[U"STATIC"] = 26;
        tokenIdMap[U"STRINGLIT"] = 84;
        tokenIdMap[U"SUPPRESS"] = 42;
        tokenIdMap[U"SWITCH"] = 48;
        tokenIdMap[U"THIS"] = 63;
        tokenIdMap[U"THROW"] = 72;
        tokenIdMap[U"TRUE"] = 5;
        tokenIdMap[U"TRY"] = 70;
        tokenIdMap[U"TYPEDEF"] = 57;
        tokenIdMap[U"TYPEID"] = 59;
        tokenIdMap[U"TYPENAME"] = 58;
        tokenIdMap[U"UCHAR"] = 19;
        tokenIdMap[U"UINT"] = 12;
        tokenIdMap[U"ULONG"] = 14;
        tokenIdMap[U"UNIT_TEST"] = 79;
        tokenIdMap[U"USHORT"] = 10;
        tokenIdMap[U"USING"] = 25;
        tokenIdMap[U"VIRTUAL"] = 39;
        tokenIdMap[U"VOID"] = 20;
        tokenIdMap[U"WCHAR"] = 18;
        tokenIdMap[U"WHERE"] = 74;
        tokenIdMap[U"WHILE"] = 51;
    }

    int GetTokenId(const std::u32string& tokenName)
    {
        auto it = tokenIdMap.find(tokenName);
        if (it != tokenIdMap.cend())
        {
            return it->second;
        }
        else
        {
            return -1;
        }
    }

    const char* tokenName[] =
    {
        "END",
        "ASSERT",
        "ELIF",
        "ENDIF",
        "BOOL",
        "TRUE",
        "FALSE",
        "SBYTE",
        "BYTE",
        "SHORT",
        "USHORT",
        "INT",
        "UINT",
        "LONG",
        "ULONG",
        "FLOAT",
        "DOUBLE",
        "CHAR",
        "WCHAR",
        "UCHAR",
        "VOID",
        "ENUM",
        "CAST",
        "INTERFACE",
        "NAMESPACE",
        "USING",
        "STATIC",
        "EXTERN",
        "AS",
        "IS",
        "EXPLICIT",
        "DELEGATE",
        "INLINE",
        "CDECL",
        "NOTHROW",
        "PUBLIC",
        "PROTECTED",
        "PRIVATE",
        "INTERNAL",
        "VIRTUAL",
        "ABSTRACT",
        "OVERRIDE",
        "SUPPRESS",
        "OPERATOR",
        "CLASS",
        "RETURN",
        "IF",
        "ELSE",
        "SWITCH",
        "CASE",
        "DEFAULT",
        "WHILE",
        "DO",
        "FOR",
        "BREAK",
        "CONTINUE",
        "GOTO",
        "TYPEDEF",
        "TYPENAME",
        "TYPEID",
        "CONST",
        "CONSTEXPR",
        "NULLLIT",
        "THIS",
        "BASE",
        "CONSTRUCT",
        "DESTROY",
        "NEW",
        "DELETE",
        "SIZEOF",
        "TRY",
        "CATCH",
        "THROW",
        "CONCEPT",
        "WHERE",
        "AXIOM",
        "AND",
        "OR",
        "EXCLAMATION",
        "UNIT_TEST",
        "ID",
        "FLOATINGLIT",
        "INTLIT",
        "CHARLIT",
        "STRINGLIT",
        "EQUIVALENCE",
        "IMPLICATION",
        "DISJUNCTION",
        "AMPAMP",
        "BITOR",
        "BITXOR",
        "AMP",
        "EQ",
        "NEQ",
        "LEQ",
        "GEQ",
        "SHIFTLEFT",
        "SHIFTRIGHT",
        "PLUS",
        "MINUS",
        "STAR",
        "DIV",
        "REM",
        "PLUSPLUS",
        "MINUSMINUS",
        "CPL",
        "DOT",
        "ARROW",
        "LBRACKET",
        "RBRACKET",
        "LPAREN",
        "RPAREN",
        "LANGLE",
        "RANGLE",
        "LBRACE",
        "RBRACE",
        "COMMA",
        "ASSIGN",
        "COLON",
        "SEMICOLON",
        "HASH"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'assert'",
        "'elif'",
        "'endif'",
        "'bool'",
        "'true'",
        "'false'",
        "'sbyte'",
        "'byte'",
        "'short'",
        "'ushort'",
        "'int'",
        "'uint'",
        "'long'",
        "'ulong'",
        "'float'",
        "'double'",
        "'char'",
        "'wchar'",
        "'uchar'",
        "'void'",
        "'enum'",
        "'cast'",
        "'interface'",
        "'namespace'",
        "'using'",
        "'static'",
        "'extern'",
        "'as'",
        "'is'",
        "'explicit'",
        "'delegate'",
        "'inline'",
        "'cdecl'",
        "'nothrow'",
        "'public'",
        "'protected'",
        "'private'",
        "'internal'",
        "'virtual'",
        "'abstract'",
        "'override'",
        "'suppress'",
        "'operator'",
        "'class'",
        "'return'",
        "'if'",
        "'else'",
        "'switch'",
        "'case'",
        "'default'",
        "'while'",
        "'do'",
        "'for'",
        "'break'",
        "'continue'",
        "'goto'",
        "'typedef'",
        "'typename'",
        "'typeid'",
        "'const'",
        "'constexpr'",
        "'null'",
        "'this'",
        "'base'",
        "'construct'",
        "'destroy'",
        "'new'",
        "'delete'",
        "'sizeof'",
        "'try'",
        "'catch'",
        "'throw'",
        "'concept'",
        "'where'",
        "'axiom'",
        "'and'",
        "'or'",
        "'!'",
        "'unit_test'",
        "identifier",
        "floating literal",
        "integer literal",
        "character literal",
        "string literal",
        "'<=>'",
        "'=>'",
        "'||'",
        "'&&'",
        "'|'",
        "'^'",
        "'&'",
        "'=='",
        "'!='",
        "'<='",
        "'>='",
        "'<<'",
        "'>>'",
        "'+'",
        "'-'",
        "'*'",
        "'/'",
        "'%'",
        "'++'",
        "'--'",
        "'~'",
        "'.'",
        "'->'",
        "'['",
        "']'",
        "'('",
        "')'",
        "'<'",
        "'>'",
        "'{'",
        "'}'",
        "','",
        "'='",
        "':'",
        "';'",
        "'#'"
    };

    const char* GetTokenInfo(int tokenId)
    {
        return tokenInfo[tokenId];
    }

    struct Initializer
    {
        Initializer() { InitTokenIdMap(); }
    };

    Initializer initializer;
}
