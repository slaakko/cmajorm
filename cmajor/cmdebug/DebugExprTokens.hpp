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
    const int AST = 7;
    const int AMP = 8;
    const int LPAREN = 9;
    const int RPAREN = 10;
    const int LANGLE = 11;
    const int RANGLE = 12;
    const int LBRACKET = 13;
    const int RBRACKET = 14;
    const int DOT = 15;
    const int COMMA = 16;
    const int PLUS = 17;
    const int MINUS = 18;

    DEBUG_API void InitTokenIdMap();
    DEBUG_API int GetTokenId(const std::u32string& tokenName);
    DEBUG_API const char* GetTokenName(int tokenId);
    DEBUG_API const char* GetTokenInfo(int tokenId);
}
#endif
