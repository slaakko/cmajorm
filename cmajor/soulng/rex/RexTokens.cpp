
// this file has been automatically generated from 'D:/work/soulng-project/soulng/rex/RexLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <soulng/rex/RexTokens.hpp>
#include <map>

namespace RexTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ALT"] = 5;
        tokenIdMap[U"CHAR"] = 13;
        tokenIdMap[U"DOT"] = 9;
        tokenIdMap[U"ESCAPE"] = 10;
        tokenIdMap[U"INVERSE"] = 11;
        tokenIdMap[U"LBRACKET"] = 3;
        tokenIdMap[U"LPAREN"] = 1;
        tokenIdMap[U"MINUS"] = 12;
        tokenIdMap[U"PLUS"] = 7;
        tokenIdMap[U"QUEST"] = 8;
        tokenIdMap[U"RBRACKET"] = 4;
        tokenIdMap[U"RPAREN"] = 2;
        tokenIdMap[U"STAR"] = 6;
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
        "LPAREN",
        "RPAREN",
        "LBRACKET",
        "RBRACKET",
        "ALT",
        "STAR",
        "PLUS",
        "QUEST",
        "DOT",
        "ESCAPE",
        "INVERSE",
        "MINUS",
        "CHAR"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'('",
        "')'",
        "'['",
        "']'",
        "'|'",
        "'*'",
        "'+'",
        "'?'",
        "'.'",
        "escape",
        "'^'",
        "'-'",
        "character"
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
