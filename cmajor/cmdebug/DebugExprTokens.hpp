#ifndef DebugExprTokens_HPP
#define DebugExprTokens_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebugExpr.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugApi.hpp>
#include <string>

namespace DebugExprTokens
{
    const int END = 0;
    const int CAST = 1;
    const int TYPEID = 2;
    const int BASE = 3;
    const int ID = 4;
    const int INTEGER = 5;
    const int STRING = 6;
    const int ADDRESS = 7;
    const int AST = 8;
    const int AMP = 9;
    const int DOLLAR = 10;
    const int LPAREN = 11;
    const int RPAREN = 12;
    const int LANGLE = 13;
    const int RANGLE = 14;
    const int LBRACKET = 15;
    const int RBRACKET = 16;
    const int DOT = 17;
    const int COMMA = 18;
    const int PLUS = 19;
    const int MINUS = 20;

    DEBUG_API void InitTokenIdMap();
    DEBUG_API int GetTokenId(const std::u32string& tokenName);
    DEBUG_API const char* GetTokenName(int tokenId);
    DEBUG_API const char* GetTokenInfo(int tokenId);
}
#endif
