
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/cmtrace/TraceLexer.lexer' using soulng lexer generator slg version 3.0.0

#include "TraceTokens.hpp"
#include <map>

namespace TraceTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"AND"] = 8;
        tokenIdMap[U"DURATION"] = 3;
        tokenIdMap[U"EQ"] = 19;
        tokenIdMap[U"FALSE"] = 11;
        tokenIdMap[U"GREATER"] = 24;
        tokenIdMap[U"GREATEREQ"] = 22;
        tokenIdMap[U"ID"] = 25;
        tokenIdMap[U"INTEGER"] = 28;
        tokenIdMap[U"LESS"] = 23;
        tokenIdMap[U"LESSEQ"] = 21;
        tokenIdMap[U"LEVEL"] = 6;
        tokenIdMap[U"LPAREN"] = 16;
        tokenIdMap[U"MATCH"] = 18;
        tokenIdMap[U"MESSAGE"] = 7;
        tokenIdMap[U"MS"] = 13;
        tokenIdMap[U"MUS"] = 14;
        tokenIdMap[U"NAME"] = 1;
        tokenIdMap[U"NEQ"] = 20;
        tokenIdMap[U"NS"] = 15;
        tokenIdMap[U"OR"] = 9;
        tokenIdMap[U"PARENT"] = 5;
        tokenIdMap[U"PATTERN"] = 26;
        tokenIdMap[U"RPAREN"] = 17;
        tokenIdMap[U"S"] = 12;
        tokenIdMap[U"STRING"] = 27;
        tokenIdMap[U"THREAD"] = 2;
        tokenIdMap[U"TIME"] = 4;
        tokenIdMap[U"TRUE"] = 10;
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
        "NAME",
        "THREAD",
        "DURATION",
        "TIME",
        "PARENT",
        "LEVEL",
        "MESSAGE",
        "AND",
        "OR",
        "TRUE",
        "FALSE",
        "S",
        "MS",
        "MUS",
        "NS",
        "LPAREN",
        "RPAREN",
        "MATCH",
        "EQ",
        "NEQ",
        "LESSEQ",
        "GREATEREQ",
        "LESS",
        "GREATER",
        "ID",
        "PATTERN",
        "STRING",
        "INTEGER"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'name'",
        "'thread'",
        "'duration'",
        "'time'",
        "'parent'",
        "'level'",
        "'message'",
        "'and'",
        "'or'",
        "'true'",
        "'false'",
        "'s'",
        "'ms'",
        "'mus'",
        "'ns'",
        "'('",
        "')'",
        "'match'",
        "'='",
        "'<>'",
        "'<='",
        "'>='",
        "'<",
        "'>'",
        "'id'",
        "'pattern'",
        "'string'",
        "'integer'"
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
