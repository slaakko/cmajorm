#ifndef CmajorTokenLexerTokens_HPP
#define CmajorTokenLexerTokens_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmlexer/CmajorTokenLexer.lexer' using soulng lexer generator slg version 4.0.0

#include <sngcm/cmlexer/TokenValueParsers.hpp>
#include <sngcm/cmlexer/CmajorLexerApi.hpp>
#include <string>

namespace CmajorTokenLexerTokens
{
    const int END = 0;
    const int ASSERT = 1;
    const int ELIF = 2;
    const int ENDIF = 3;
    const int BOOL = 4;
    const int TRUE = 5;
    const int FALSE = 6;
    const int SBYTE = 7;
    const int BYTE = 8;
    const int SHORT = 9;
    const int USHORT = 10;
    const int INT = 11;
    const int UINT = 12;
    const int LONG = 13;
    const int ULONG = 14;
    const int FLOAT = 15;
    const int DOUBLE = 16;
    const int CHAR = 17;
    const int WCHAR = 18;
    const int UCHAR = 19;
    const int VOID = 20;
    const int ENUM = 21;
    const int CAST = 22;
    const int INTERFACE = 23;
    const int NAMESPACE = 24;
    const int USING = 25;
    const int STATIC = 26;
    const int EXTERN = 27;
    const int AS = 28;
    const int IS = 29;
    const int EXPLICIT = 30;
    const int DELEGATE = 31;
    const int INLINE = 32;
    const int CDECL = 33;
    const int NOTHROW = 34;
    const int PUBLIC = 35;
    const int PROTECTED = 36;
    const int PRIVATE = 37;
    const int INTERNAL = 38;
    const int VIRTUAL = 39;
    const int ABSTRACT = 40;
    const int OVERRIDE = 41;
    const int SUPPRESS = 42;
    const int WINAPI = 43;
    const int OPERATOR = 44;
    const int CLASS = 45;
    const int RETURN = 46;
    const int IF = 47;
    const int ELSE = 48;
    const int SWITCH = 49;
    const int CASE = 50;
    const int DEFAULT = 51;
    const int WHILE = 52;
    const int DO = 53;
    const int FOR = 54;
    const int BREAK = 55;
    const int CONTINUE = 56;
    const int GOTO = 57;
    const int TYPEDEF = 58;
    const int TYPENAME = 59;
    const int TYPEID = 60;
    const int CONST = 61;
    const int CONSTEXPR = 62;
    const int NULLLIT = 63;
    const int THIS = 64;
    const int BASE = 65;
    const int CONSTRUCT = 66;
    const int DESTROY = 67;
    const int NEW = 68;
    const int DELETE = 69;
    const int SIZEOF = 70;
    const int TRY = 71;
    const int CATCH = 72;
    const int THROW = 73;
    const int CONCEPT = 74;
    const int WHERE = 75;
    const int AXIOM = 76;
    const int AND = 77;
    const int OR = 78;
    const int EXCLAMATION = 79;
    const int UNIT_TEST = 80;
    const int ID = 81;
    const int FLOATINGLIT = 82;
    const int INTLIT = 83;
    const int CHARLIT = 84;
    const int STRINGLIT = 85;
    const int EQUIVALENCE = 86;
    const int IMPLICATION = 87;
    const int DISJUNCTION = 88;
    const int AMPAMP = 89;
    const int BITOR = 90;
    const int BITXOR = 91;
    const int AMP = 92;
    const int EQ = 93;
    const int NEQ = 94;
    const int LEQ = 95;
    const int GEQ = 96;
    const int SHIFTLEFT = 97;
    const int SHIFTRIGHT = 98;
    const int PLUS = 99;
    const int MINUS = 100;
    const int STAR = 101;
    const int DIV = 102;
    const int REM = 103;
    const int PLUSPLUS = 104;
    const int MINUSMINUS = 105;
    const int CPL = 106;
    const int DOT = 107;
    const int ARROW = 108;
    const int LBRACKET = 109;
    const int RBRACKET = 110;
    const int LPAREN = 111;
    const int RPAREN = 112;
    const int LANGLE = 113;
    const int RANGLE = 114;
    const int LBRACE = 115;
    const int RBRACE = 116;
    const int COMMA = 117;
    const int ASSIGN = 118;
    const int COLON = 119;
    const int SEMICOLON = 120;
    const int HASH = 121;
    const int COMMENT = 122;
    const int WS = 123;
    const int KEYWORD = 124;
    const int LINECOMMENT = 125;
    const int BLOCKCOMMENT = 126;

    SNGCM_LEXER_API void InitTokenIdMap();
    SNGCM_LEXER_API int GetTokenId(const std::u32string& tokenName);
    SNGCM_LEXER_API const char* GetTokenName(int tokenId);
    SNGCM_LEXER_API const char* GetTokenInfo(int tokenId);
}
#endif
