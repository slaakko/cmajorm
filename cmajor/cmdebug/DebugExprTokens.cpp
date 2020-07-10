
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebugExpr.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugExprTokens.hpp>
#include <map>

namespace DebugExprTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"AMP"] = 9;
        tokenIdMap[U"AST"] = 8;
        tokenIdMap[U"BASE"] = 4;
        tokenIdMap[U"CAST"] = 2;
        tokenIdMap[U"COMMA"] = 17;
        tokenIdMap[U"DOT"] = 16;
        tokenIdMap[U"ID"] = 5;
        tokenIdMap[U"INTEGER"] = 6;
        tokenIdMap[U"LANGLE"] = 12;
        tokenIdMap[U"LBRACKET"] = 14;
        tokenIdMap[U"LPAREN"] = 10;
        tokenIdMap[U"MINUS"] = 19;
        tokenIdMap[U"PLUS"] = 18;
        tokenIdMap[U"RANGE"] = 1;
        tokenIdMap[U"RANGLE"] = 13;
        tokenIdMap[U"RBRACKET"] = 15;
        tokenIdMap[U"RPAREN"] = 11;
        tokenIdMap[U"STRING"] = 7;
        tokenIdMap[U"TYPEID"] = 3;
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
        "RANGE",
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
        "'range'",
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
