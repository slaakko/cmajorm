
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/assembler/AssemblyLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/assembler/AssemblyTokens.hpp>
#include <map>

namespace AssemblyTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"AMP"] = 13;
        tokenIdMap[U"AST"] = 14;
        tokenIdMap[U"AT"] = 8;
        tokenIdMap[U"BAR"] = 20;
        tokenIdMap[U"CARET"] = 21;
        tokenIdMap[U"CHAR"] = 29;
        tokenIdMap[U"CHAR_CONSTANT"] = 3;
        tokenIdMap[U"CLSID_CONSTANT"] = 5;
        tokenIdMap[U"COMMA"] = 28;
        tokenIdMap[U"DECIMAL_CONSTANT"] = 1;
        tokenIdMap[U"DOLLAR"] = 12;
        tokenIdMap[U"DOT"] = 22;
        tokenIdMap[U"HEX_CONSTANT"] = 2;
        tokenIdMap[U"LOCAL_SYMBOL"] = 6;
        tokenIdMap[U"LPAREN"] = 25;
        tokenIdMap[U"MINUS"] = 10;
        tokenIdMap[U"NEWLINE"] = 24;
        tokenIdMap[U"PERCENT"] = 17;
        tokenIdMap[U"PLUS"] = 9;
        tokenIdMap[U"RPAREN"] = 26;
        tokenIdMap[U"SEMICOLON"] = 27;
        tokenIdMap[U"SHIFT_LEFT"] = 18;
        tokenIdMap[U"SHIFT_RIGHT"] = 19;
        tokenIdMap[U"SLASH"] = 16;
        tokenIdMap[U"SLASHSLASH"] = 15;
        tokenIdMap[U"SPACE"] = 23;
        tokenIdMap[U"STRING_CONSTANT"] = 4;
        tokenIdMap[U"SYMBOL"] = 7;
        tokenIdMap[U"TILDE"] = 11;
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
        "DECIMAL_CONSTANT",
        "HEX_CONSTANT",
        "CHAR_CONSTANT",
        "STRING_CONSTANT",
        "CLSID_CONSTANT",
        "LOCAL_SYMBOL",
        "SYMBOL",
        "AT",
        "PLUS",
        "MINUS",
        "TILDE",
        "DOLLAR",
        "AMP",
        "AST",
        "SLASHSLASH",
        "SLASH",
        "PERCENT",
        "SHIFT_LEFT",
        "SHIFT_RIGHT",
        "BAR",
        "CARET",
        "DOT",
        "SPACE",
        "NEWLINE",
        "LPAREN",
        "RPAREN",
        "SEMICOLON",
        "COMMA",
        "CHAR"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'decimal constant'",
        "'hex constant'",
        "'char constant'",
        "'string constant'",
        "'clsid constant'",
        "'local symbol'",
        "'symbol'",
        "'at'",
        "'plus'",
        "'minus'",
        "'tilde'",
        "'$'",
        "'amp'",
        "'ast'",
        "'slashslash'",
        "'slash'",
        "'percent'",
        "'shift left'",
        "'shift right'",
        "'bar'",
        "'caret'",
        "'dot'",
        "'space'",
        "'newline'",
        "'('",
        "')'",
        "';'",
        "','",
        "'char'"
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
