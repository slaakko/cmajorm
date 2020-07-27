
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebugExpr.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugExprTokens.hpp>
#include <map>

namespace DebugExprTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"AMP"] = 8;
        tokenIdMap[U"AST"] = 7;
        tokenIdMap[U"BASE"] = 3;
        tokenIdMap[U"CAST"] = 1;
        tokenIdMap[U"COMMA"] = 16;
        tokenIdMap[U"DOT"] = 15;
        tokenIdMap[U"ID"] = 4;
        tokenIdMap[U"INTEGER"] = 5;
        tokenIdMap[U"LANGLE"] = 11;
        tokenIdMap[U"LBRACKET"] = 13;
        tokenIdMap[U"LPAREN"] = 9;
        tokenIdMap[U"MINUS"] = 18;
        tokenIdMap[U"PLUS"] = 17;
        tokenIdMap[U"RANGLE"] = 12;
        tokenIdMap[U"RBRACKET"] = 14;
        tokenIdMap[U"RPAREN"] = 10;
        tokenIdMap[U"STRING"] = 6;
        tokenIdMap[U"TYPEID"] = 2;
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
        "CAST",
        "TYPEID",
        "BASE",
        "ID",
        "INTEGER",
        "STRING",
        "AST",
        "AMP",
        "LPAREN",
        "RPAREN",
        "LANGLE",
        "RANGLE",
        "LBRACKET",
        "RBRACKET",
        "DOT",
        "COMMA",
        "PLUS",
        "MINUS"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'cast'",
        "'typeid'",
        "'base'",
        "id",
        "'integer'",
        "'string'",
        "'*'",
        "'&'",
        "'('",
        "')'",
        "'<'",
        "'>'",
        "'['",
        "']'",
        "'.'",
        "','",
        "'+'",
        "'-'"
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
