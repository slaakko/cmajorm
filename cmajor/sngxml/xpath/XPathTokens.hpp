#ifndef XPathTokens_HPP
#define XPathTokens_HPP

// this file has been automatically generated from 'D:/work/soulng-project/sngxml/xpath/XPathLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngxml/xpath/XPathApi.hpp>
#include <string>

namespace XPathTokens
{
    const int END = 0;
    const int OR = 1;
    const int AND = 2;
    const int EQ = 3;
    const int NEQ = 4;
    const int LEQ = 5;
    const int GEQ = 6;
    const int LESS = 7;
    const int GREATER = 8;
    const int PLUS = 9;
    const int MINUS = 10;
    const int STAR = 11;
    const int DIV = 12;
    const int MOD = 13;
    const int UNION = 14;
    const int SLASHSLASH = 15;
    const int SLASH = 16;
    const int DOTDOT = 17;
    const int DOT = 18;
    const int COLONCOLON = 19;
    const int COLON = 20;
    const int DOLLAR = 21;
    const int COMMA = 22;
    const int ANCESTOR = 23;
    const int ANCESTOR_OR_SELF = 24;
    const int ATTRIBUTE = 25;
    const int CHILD = 26;
    const int DESCENDANT = 27;
    const int DESCENDANT_OR_SELF = 28;
    const int FOLLOWING = 29;
    const int FOLLOWING_SIBLING = 30;
    const int NAMESPACE = 31;
    const int PARENT = 32;
    const int PRECEDING = 33;
    const int PRECEDING_SIBLING = 34;
    const int SELF = 35;
    const int AT = 36;
    const int LBRACKET = 37;
    const int RBRACKET = 38;
    const int PROCESSING_INSTRUCTION = 39;
    const int LPAREN = 40;
    const int RPAREN = 41;
    const int COMMENT = 42;
    const int TEXT = 43;
    const int NODE = 44;
    const int DQSTRING = 45;
    const int SQSTRING = 46;
    const int NUMBER = 47;
    const int NAME = 48;

    SNGXML_XPATH_API void InitTokenIdMap();
    SNGXML_XPATH_API int GetTokenId(const std::u32string& tokenName);
    SNGXML_XPATH_API const char* GetTokenName(int tokenId);
    SNGXML_XPATH_API const char* GetTokenInfo(int tokenId);
}
#endif
