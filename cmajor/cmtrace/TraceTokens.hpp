#ifndef TraceTokens_HPP
#define TraceTokens_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/cmtrace/TraceLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <string>

namespace TraceTokens
{
    const int END = 0;
    const int NAME = 1;
    const int THREAD = 2;
    const int DURATION = 3;
    const int TIME = 4;
    const int PARENT = 5;
    const int LEVEL = 6;
    const int MESSAGE = 7;
    const int AND = 8;
    const int OR = 9;
    const int TRUE = 10;
    const int FALSE = 11;
    const int S = 12;
    const int MS = 13;
    const int US = 14;
    const int NS = 15;
    const int LPAREN = 16;
    const int RPAREN = 17;
    const int MATCH = 18;
    const int EQ = 19;
    const int NEQ = 20;
    const int LESSEQ = 21;
    const int GREATEREQ = 22;
    const int LESS = 23;
    const int GREATER = 24;
    const int ID = 25;
    const int PATTERN = 26;
    const int STRING = 27;
    const int INTEGER = 28;

    void InitTokenIdMap();
    int GetTokenId(const std::u32string& tokenName);
    const char* GetTokenName(int tokenId);
    const char* GetTokenInfo(int tokenId);
}
#endif
