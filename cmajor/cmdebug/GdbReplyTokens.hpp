#ifndef GdbReplyTokens_HPP
#define GdbReplyTokens_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/GdbReply.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugApi.hpp>
#include <string>

namespace GdbReplyTokens
{
    const int END = 0;
    const int GDB = 1;
    const int DONE = 2;
    const int RUNNING = 3;
    const int CONNECTED = 4;
    const int ERROR = 5;
    const int EXIT = 6;
    const int STOPPED = 7;
    const int IDENTIFIER = 8;
    const int STRING = 9;
    const int LPAREN = 10;
    const int RPAREN = 11;
    const int EQ = 12;
    const int COMMA = 13;
    const int TILDE = 14;
    const int AT = 15;
    const int AMP = 16;
    const int AST = 17;
    const int CARET = 18;
    const int LBRACE = 19;
    const int RBRACE = 20;
    const int LBRACKET = 21;
    const int RBRACKET = 22;

    DEBUG_API void InitTokenIdMap();
    DEBUG_API int GetTokenId(const std::u32string& tokenName);
    DEBUG_API const char* GetTokenName(int tokenId);
    DEBUG_API const char* GetTokenInfo(int tokenId);
}
#endif
