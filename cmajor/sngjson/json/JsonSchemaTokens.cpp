
// this file has been automatically generated from 'D:/work/soulng-project/sngjson/json/JsonSchemaLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonSchemaTokens.hpp>
#include <map>

namespace JsonSchemaTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"API"] = 8;
        tokenIdMap[U"BOOL"] = 7;
        tokenIdMap[U"DOUBLE"] = 6;
        tokenIdMap[U"FILEPATH"] = 18;
        tokenIdMap[U"FLOAT"] = 5;
        tokenIdMap[U"HASH"] = 17;
        tokenIdMap[U"ID"] = 9;
        tokenIdMap[U"INCLUDE"] = 19;
        tokenIdMap[U"INT"] = 3;
        tokenIdMap[U"LBRACE"] = 11;
        tokenIdMap[U"LBRACKET"] = 13;
        tokenIdMap[U"LONG"] = 4;
        tokenIdMap[U"LPAREN"] = 15;
        tokenIdMap[U"RBRACE"] = 12;
        tokenIdMap[U"RBRACKET"] = 14;
        tokenIdMap[U"RPAREN"] = 16;
        tokenIdMap[U"SEMICOLON"] = 10;
        tokenIdMap[U"STRING"] = 2;
        tokenIdMap[U"STRUCT"] = 1;
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
        "STRUCT",
        "STRING",
        "INT",
        "LONG",
        "FLOAT",
        "DOUBLE",
        "BOOL",
        "API",
        "ID",
        "SEMICOLON",
        "LBRACE",
        "RBRACE",
        "LBRACKET",
        "RBRACKET",
        "LPAREN",
        "RPAREN",
        "HASH",
        "FILEPATH",
        "INCLUDE"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'struct'",
        "'string'",
        "'int'",
        "'long'",
        "'float'",
        "'double'",
        "'bool'",
        "'api'",
        "'id'",
        "';'",
        "'{'",
        "'}'",
        "'['",
        "']'",
        "'('",
        "')'",
        "'#'",
        "'filepath'",
        "'include'"
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
