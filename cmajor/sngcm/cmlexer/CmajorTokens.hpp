#ifndef CmajorTokens_HPP
#define CmajorTokens_HPP

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/CmajorLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/TokenValueParsers.hpp>
#include <sngcm/cmlexer/CmajorLexerApi.hpp>
#include <string>

namespace CmajorTokens
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
    const int OPERATOR = 43;
    const int CLASS = 44;
    const int RETURN = 45;
    const int IF = 46;
    const int ELSE = 47;
    const int SWITCH = 48;
    const int CASE = 49;
    const int DEFAULT = 50;
    const int WHILE = 51;
    const int DO = 52;
    const int FOR = 53;
    const int BREAK = 54;
    const int CONTINUE = 55;
    const int GOTO = 56;
    const int TYPEDEF = 57;
    const int TYPENAME = 58;
    const int TYPEID = 59;
    const int CONST = 60;
    const int CONSTEXPR = 61;
    const int NULLLIT = 62;
    const int THIS = 63;
    const int BASE = 64;
    const int CONSTRUCT = 65;
    const int DESTROY = 66;
    const int NEW = 67;
    const int DELETE = 68;
    const int SIZEOF = 69;
    const int TRY = 70;
    const int CATCH = 71;
    const int THROW = 72;
    const int CONCEPT = 73;
    const int WHERE = 74;
    const int AXIOM = 75;
    const int AND = 76;
    const int OR = 77;
    const int EXCLAMATION = 78;
    const int UNIT_TEST = 79;
    const int ID = 80;
    const int FLOATINGLIT = 81;
    const int INTLIT = 82;
    const int CHARLIT = 83;
    const int STRINGLIT = 84;
    const int EQUIVALENCE = 85;
    const int IMPLICATION = 86;
    const int DISJUNCTION = 87;
    const int AMPAMP = 88;
    const int BITOR = 89;
    const int BITXOR = 90;
    const int AMP = 91;
    const int EQ = 92;
    const int NEQ = 93;
    const int LEQ = 94;
    const int GEQ = 95;
    const int SHIFTLEFT = 96;
    const int SHIFTRIGHT = 97;
    const int PLUS = 98;
    const int MINUS = 99;
    const int STAR = 100;
    const int DIV = 101;
    const int REM = 102;
    const int PLUSPLUS = 103;
    const int MINUSMINUS = 104;
    const int CPL = 105;
    const int DOT = 106;
    const int ARROW = 107;
    const int LBRACKET = 108;
    const int RBRACKET = 109;
    const int LPAREN = 110;
    const int RPAREN = 111;
    const int LANGLE = 112;
    const int RANGLE = 113;
    const int LBRACE = 114;
    const int RBRACE = 115;
    const int COMMA = 116;
    const int ASSIGN = 117;
    const int COLON = 118;
    const int SEMICOLON = 119;
    const int HASH = 120;

    SNGCM_LEXER_API void InitTokenIdMap();
    SNGCM_LEXER_API int GetTokenId(const std::u32string& tokenName);
    SNGCM_LEXER_API const char* GetTokenName(int tokenId);
    SNGCM_LEXER_API const char* GetTokenInfo(int tokenId);
}
#endif
