#ifndef DebuggerCommandTokens_HPP
#define DebuggerCommandTokens_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebuggerCommand.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugApi.hpp>
#include <string>

namespace DebuggerCommandTokens
{
    const int END = 0;
    const int NL = 1;
    const int EXIT = 2;
    const int E = 3;
    const int QUIT = 4;
    const int Q = 5;
    const int HELP = 6;
    const int H = 7;
    const int NEXT = 8;
    const int N = 9;
    const int STEP = 10;
    const int S = 11;
    const int CONTINUE = 12;
    const int C = 13;
    const int FINISH = 14;
    const int F = 15;
    const int UNTIL = 16;
    const int U = 17;
    const int BREAK = 18;
    const int B = 19;
    const int DELETE = 20;
    const int D = 21;
    const int PRINT = 22;
    const int P = 23;
    const int DEPTH = 24;
    const int FRAMES = 25;
    const int LIST = 26;
    const int L = 27;
    const int SHOW = 28;
    const int BREAKPOINT = 29;
    const int BREAKPOINTS = 30;
    const int CAST = 31;
    const int TYPEID = 32;
    const int BASE = 33;
    const int ID = 34;
    const int PATH = 35;
    const int INTEGER = 36;
    const int STRING = 37;
    const int ADDRESS = 38;
    const int COLON = 39;
    const int AST = 40;
    const int AMP = 41;
    const int DOLLAR = 42;
    const int LPAREN = 43;
    const int RPAREN = 44;
    const int LANGLE = 45;
    const int RANGLE = 46;
    const int LBRACKET = 47;
    const int RBRACKET = 48;
    const int DOT = 49;
    const int COMMA = 50;
    const int PLUS = 51;
    const int MINUS = 52;

    DEBUG_API void InitTokenIdMap();
    DEBUG_API int GetTokenId(const std::u32string& tokenName);
    DEBUG_API const char* GetTokenName(int tokenId);
    DEBUG_API const char* GetTokenInfo(int tokenId);
}
#endif
