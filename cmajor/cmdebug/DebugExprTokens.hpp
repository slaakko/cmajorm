#ifndef DebugExprTokens_HPP
#define DebugExprTokens_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebugExpr.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugApi.hpp>
#include <string>

namespace DebugExprTokens
{
    const int END = 0;
    const int RANGE = 1;
    const int CAST = 2;
    const int TYPEID = 3;
    const int BASE = 4;
    const int ID = 5;
    const int INTEGER = 6;
    const int STRING = 7;
    const int AST = 8;
    const int AMP = 9;
    const int LPAREN = 10;
    const int RPAREN = 11;
    const int LANGLE = 12;
    const int RANGLE = 13;
    const int LBRACKET = 14;
    const int RBRACKET = 15;
    const int DOT = 16;
    const int COMMA = 17;
    const int PLUS = 18;
    const int MINUS = 19;

    DEBUG_API void InitTokenIdMap();
    DEBUG_API int GetTokenId(const std::u32string& tokenName);
    DEBUG_API const char* GetTokenName(int tokenId);
    DEBUG_API const char* GetTokenInfo(int tokenId);
}
#endif
