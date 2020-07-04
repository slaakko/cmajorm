
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/GdbReply.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/GdbReplyTokens.hpp>
#include <map>

namespace GdbReplyTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"AMP"] = 16;
        tokenIdMap[U"AST"] = 17;
        tokenIdMap[U"AT"] = 15;
        tokenIdMap[U"CARET"] = 18;
        tokenIdMap[U"COMMA"] = 13;
        tokenIdMap[U"CONNECTED"] = 4;
        tokenIdMap[U"DONE"] = 2;
        tokenIdMap[U"EQ"] = 12;
        tokenIdMap[U"ERROR"] = 5;
        tokenIdMap[U"EXIT"] = 6;
        tokenIdMap[U"GDB"] = 1;
        tokenIdMap[U"IDENTIFIER"] = 8;
        tokenIdMap[U"LBRACE"] = 19;
        tokenIdMap[U"LBRACKET"] = 21;
        tokenIdMap[U"LPAREN"] = 10;
        tokenIdMap[U"RBRACE"] = 20;
        tokenIdMap[U"RBRACKET"] = 22;
        tokenIdMap[U"RPAREN"] = 11;
        tokenIdMap[U"RUNNING"] = 3;
        tokenIdMap[U"STOPPED"] = 7;
        tokenIdMap[U"STRING"] = 9;
        tokenIdMap[U"TILDE"] = 14;
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
        "GDB",
        "DONE",
        "RUNNING",
        "CONNECTED",
        "ERROR",
        "EXIT",
        "STOPPED",
        "IDENTIFIER",
        "STRING",
        "LPAREN",
        "RPAREN",
        "EQ",
        "COMMA",
        "TILDE",
        "AT",
        "AMP",
        "AST",
        "CARET",
        "LBRACE",
        "RBRACE",
        "LBRACKET",
        "RBRACKET"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'gdb'",
        "'done'",
        "'running'",
        "'connected'",
        "'error'",
        "'exit'",
        "'stopped'",
        "identifier",
        "'string'",
        "'('",
        "')'",
        "'='",
        "','",
        "'~'",
        "'@'",
        "'&'",
        "'*'",
        "'^'",
        "'{'",
        "'}'",
        "'['",
        "']'"
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
