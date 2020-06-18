
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngjson/json/JsonLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonTokens.hpp>
#include <map>

namespace JsonTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"COLON"] = 8;
        tokenIdMap[U"COMMA"] = 7;
        tokenIdMap[U"FALSE"] = 10;
        tokenIdMap[U"LBRACE"] = 5;
        tokenIdMap[U"LBRACKET"] = 3;
        tokenIdMap[U"NULLTOKEN"] = 11;
        tokenIdMap[U"NUMBER"] = 2;
        tokenIdMap[U"RBRACE"] = 6;
        tokenIdMap[U"RBRACKET"] = 4;
        tokenIdMap[U"STRING"] = 1;
        tokenIdMap[U"TRUE"] = 9;
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
        "STRING",
        "NUMBER",
        "LBRACKET",
        "RBRACKET",
        "LBRACE",
        "RBRACE",
        "COMMA",
        "COLON",
        "TRUE",
        "FALSE",
        "NULLTOKEN"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'string'",
        "'number'",
        "'['",
        "']",
        "{'",
        "'}'",
        "','",
        "':'",
        "'true'",
        "'false'",
        "'null'"
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
