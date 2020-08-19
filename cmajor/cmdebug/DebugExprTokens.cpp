
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
        tokenIdMap[U"BITOR"] = 21;
        tokenIdMap[U"BITXOR"] = 22;
        tokenIdMap[U"CAST"] = 1;
        tokenIdMap[U"COMMA"] = 16;
        tokenIdMap[U"CONJUNCTION"] = 20;
        tokenIdMap[U"DISJUNCTION"] = 19;
        tokenIdMap[U"DOT"] = 15;
        tokenIdMap[U"EQUAL"] = 23;
        tokenIdMap[U"GREATEREQUAL"] = 26;
        tokenIdMap[U"ID"] = 4;
        tokenIdMap[U"INTEGER"] = 5;
        tokenIdMap[U"LANGLE"] = 11;
        tokenIdMap[U"LBRACKET"] = 13;
        tokenIdMap[U"LESSEQUAL"] = 25;
        tokenIdMap[U"LPAREN"] = 9;
        tokenIdMap[U"MINUS"] = 18;
        tokenIdMap[U"NOTEQUAL"] = 24;
        tokenIdMap[U"PERCENT"] = 30;
        tokenIdMap[U"PLUS"] = 17;
        tokenIdMap[U"RANGLE"] = 12;
        tokenIdMap[U"RBRACKET"] = 14;
        tokenIdMap[U"RPAREN"] = 10;
        tokenIdMap[U"SHIFTLEFT"] = 27;
        tokenIdMap[U"SHIFTRIGHT"] = 28;
        tokenIdMap[U"SLASH"] = 29;
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
        "MINUS",
        "DISJUNCTION",
        "CONJUNCTION",
        "BITOR",
        "BITXOR",
        "EQUAL",
        "NOTEQUAL",
        "LESSEQUAL",
        "GREATEREQUAL",
        "SHIFTLEFT",
        "SHIFTRIGHT",
        "SLASH",
        "PERCENT"
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
        "'-'",
        "'||'",
        "'&&'",
        "'|'",
        "'^'",
        "'=='",
        "'!='",
        "'<='",
        "'>='",
        "'<<'",
        "'>>'",
        "'/'",
        "'%'"
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
