
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmlexer/CmajorLexer.lexer' using soulng lexer generator slg version 3.10.0

#include <sngcm/cmlexer/CmajorTokens.hpp>
#include <map>

namespace CmajorTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ABSTRACT"] = 40;
        tokenIdMap[U"AMP"] = 92;
        tokenIdMap[U"AMPAMP"] = 89;
        tokenIdMap[U"AND"] = 77;
        tokenIdMap[U"ARROW"] = 108;
        tokenIdMap[U"AS"] = 28;
        tokenIdMap[U"ASSERT"] = 1;
        tokenIdMap[U"ASSIGN"] = 118;
        tokenIdMap[U"AXIOM"] = 76;
        tokenIdMap[U"BASE"] = 65;
        tokenIdMap[U"BITOR"] = 90;
        tokenIdMap[U"BITXOR"] = 91;
        tokenIdMap[U"BOOL"] = 4;
        tokenIdMap[U"BREAK"] = 55;
        tokenIdMap[U"BYTE"] = 8;
        tokenIdMap[U"CASE"] = 50;
        tokenIdMap[U"CAST"] = 22;
        tokenIdMap[U"CATCH"] = 72;
        tokenIdMap[U"CDECL"] = 33;
        tokenIdMap[U"CHAR"] = 17;
        tokenIdMap[U"CHARLIT"] = 84;
        tokenIdMap[U"CLASS"] = 45;
        tokenIdMap[U"COLON"] = 119;
        tokenIdMap[U"COMMA"] = 117;
        tokenIdMap[U"COMMENT"] = 122;
        tokenIdMap[U"CONCEPT"] = 74;
        tokenIdMap[U"CONST"] = 61;
        tokenIdMap[U"CONSTEXPR"] = 62;
        tokenIdMap[U"CONSTRUCT"] = 66;
        tokenIdMap[U"CONTINUE"] = 56;
        tokenIdMap[U"CPL"] = 106;
        tokenIdMap[U"DEFAULT"] = 51;
        tokenIdMap[U"DELEGATE"] = 31;
        tokenIdMap[U"DELETE"] = 69;
        tokenIdMap[U"DESTROY"] = 67;
        tokenIdMap[U"DISJUNCTION"] = 88;
        tokenIdMap[U"DIV"] = 102;
        tokenIdMap[U"DO"] = 53;
        tokenIdMap[U"DOT"] = 107;
        tokenIdMap[U"DOUBLE"] = 16;
        tokenIdMap[U"ELIF"] = 2;
        tokenIdMap[U"ELSE"] = 48;
        tokenIdMap[U"ENDIF"] = 3;
        tokenIdMap[U"ENUM"] = 21;
        tokenIdMap[U"EQ"] = 93;
        tokenIdMap[U"EQUIVALENCE"] = 86;
        tokenIdMap[U"EXCLAMATION"] = 79;
        tokenIdMap[U"EXPLICIT"] = 30;
        tokenIdMap[U"EXTERN"] = 27;
        tokenIdMap[U"FALSE"] = 6;
        tokenIdMap[U"FLOAT"] = 15;
        tokenIdMap[U"FLOATINGLIT"] = 82;
        tokenIdMap[U"FOR"] = 54;
        tokenIdMap[U"GEQ"] = 96;
        tokenIdMap[U"GOTO"] = 57;
        tokenIdMap[U"HASH"] = 121;
        tokenIdMap[U"ID"] = 81;
        tokenIdMap[U"IF"] = 47;
        tokenIdMap[U"IMPLICATION"] = 87;
        tokenIdMap[U"INLINE"] = 32;
        tokenIdMap[U"INT"] = 11;
        tokenIdMap[U"INTERFACE"] = 23;
        tokenIdMap[U"INTERNAL"] = 38;
        tokenIdMap[U"INTLIT"] = 83;
        tokenIdMap[U"IS"] = 29;
        tokenIdMap[U"LANGLE"] = 113;
        tokenIdMap[U"LBRACE"] = 115;
        tokenIdMap[U"LBRACKET"] = 109;
        tokenIdMap[U"LEQ"] = 95;
        tokenIdMap[U"LONG"] = 13;
        tokenIdMap[U"LPAREN"] = 111;
        tokenIdMap[U"MINUS"] = 100;
        tokenIdMap[U"MINUSMINUS"] = 105;
        tokenIdMap[U"NAMESPACE"] = 24;
        tokenIdMap[U"NEQ"] = 94;
        tokenIdMap[U"NEW"] = 68;
        tokenIdMap[U"NOTHROW"] = 34;
        tokenIdMap[U"NULLLIT"] = 63;
        tokenIdMap[U"OPERATOR"] = 44;
        tokenIdMap[U"OR"] = 78;
        tokenIdMap[U"OVERRIDE"] = 41;
        tokenIdMap[U"PLUS"] = 99;
        tokenIdMap[U"PLUSPLUS"] = 104;
        tokenIdMap[U"PRIVATE"] = 37;
        tokenIdMap[U"PROTECTED"] = 36;
        tokenIdMap[U"PUBLIC"] = 35;
        tokenIdMap[U"RANGLE"] = 114;
        tokenIdMap[U"RBRACE"] = 116;
        tokenIdMap[U"RBRACKET"] = 110;
        tokenIdMap[U"REM"] = 103;
        tokenIdMap[U"RETURN"] = 46;
        tokenIdMap[U"RPAREN"] = 112;
        tokenIdMap[U"SBYTE"] = 7;
        tokenIdMap[U"SEMICOLON"] = 120;
        tokenIdMap[U"SHIFTLEFT"] = 97;
        tokenIdMap[U"SHIFTRIGHT"] = 98;
        tokenIdMap[U"SHORT"] = 9;
        tokenIdMap[U"SIZEOF"] = 70;
        tokenIdMap[U"STAR"] = 101;
        tokenIdMap[U"STATIC"] = 26;
        tokenIdMap[U"STRINGLIT"] = 85;
        tokenIdMap[U"SUPPRESS"] = 42;
        tokenIdMap[U"SWITCH"] = 49;
        tokenIdMap[U"THIS"] = 64;
        tokenIdMap[U"THROW"] = 73;
        tokenIdMap[U"TRUE"] = 5;
        tokenIdMap[U"TRY"] = 71;
        tokenIdMap[U"TYPEDEF"] = 58;
        tokenIdMap[U"TYPEID"] = 60;
        tokenIdMap[U"TYPENAME"] = 59;
        tokenIdMap[U"UCHAR"] = 19;
        tokenIdMap[U"UINT"] = 12;
        tokenIdMap[U"ULONG"] = 14;
        tokenIdMap[U"UNIT_TEST"] = 80;
        tokenIdMap[U"USHORT"] = 10;
        tokenIdMap[U"USING"] = 25;
        tokenIdMap[U"VIRTUAL"] = 39;
        tokenIdMap[U"VOID"] = 20;
        tokenIdMap[U"WCHAR"] = 18;
        tokenIdMap[U"WHERE"] = 75;
        tokenIdMap[U"WHILE"] = 52;
        tokenIdMap[U"WINAPI"] = 43;
        tokenIdMap[U"WS"] = 123;
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
        "WINAPI",
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
        "HASH",
        "COMMENT",
        "WS"
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
        "'winapi'",
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
        "'#'",
        "comment",
        "ws"
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
