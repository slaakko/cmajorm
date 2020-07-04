
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebuggerCommand.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebuggerCommandTokens.hpp>
#include <map>

namespace DebuggerCommandTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"B"] = 19;
        tokenIdMap[U"BREAK"] = 18;
        tokenIdMap[U"BREAKPOINTS"] = 25;
        tokenIdMap[U"C"] = 13;
        tokenIdMap[U"COLON"] = 31;
        tokenIdMap[U"CONTINUE"] = 12;
        tokenIdMap[U"D"] = 21;
        tokenIdMap[U"DELETE"] = 20;
        tokenIdMap[U"DEPTH"] = 22;
        tokenIdMap[U"E"] = 3;
        tokenIdMap[U"EXIT"] = 2;
        tokenIdMap[U"F"] = 15;
        tokenIdMap[U"FINISH"] = 14;
        tokenIdMap[U"FRAMES"] = 23;
        tokenIdMap[U"H"] = 7;
        tokenIdMap[U"HELP"] = 6;
        tokenIdMap[U"ID"] = 28;
        tokenIdMap[U"INTEGER"] = 30;
        tokenIdMap[U"L"] = 27;
        tokenIdMap[U"LIST"] = 26;
        tokenIdMap[U"N"] = 9;
        tokenIdMap[U"NEXT"] = 8;
        tokenIdMap[U"NL"] = 1;
        tokenIdMap[U"PATH"] = 29;
        tokenIdMap[U"Q"] = 5;
        tokenIdMap[U"QUIT"] = 4;
        tokenIdMap[U"S"] = 11;
        tokenIdMap[U"SHOW"] = 24;
        tokenIdMap[U"STEP"] = 10;
        tokenIdMap[U"U"] = 17;
        tokenIdMap[U"UNTIL"] = 16;
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
        "NL",
        "EXIT",
        "E",
        "QUIT",
        "Q",
        "HELP",
        "H",
        "NEXT",
        "N",
        "STEP",
        "S",
        "CONTINUE",
        "C",
        "FINISH",
        "F",
        "UNTIL",
        "U",
        "BREAK",
        "B",
        "DELETE",
        "D",
        "DEPTH",
        "FRAMES",
        "SHOW",
        "BREAKPOINTS",
        "LIST",
        "L",
        "ID",
        "PATH",
        "INTEGER",
        "COLON"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'newline'",
        "'exit'",
        "'e'",
        "'quit'",
        "'q'",
        "'help'",
        "'h",
        "'next'",
        "'n'",
        "'step'",
        "'s'",
        "'continue'",
        "'c'",
        "'finish'",
        "'f'",
        "'until'",
        "'u'",
        "'break'",
        "'b'",
        "'delete'",
        "'d'",
        "'depth'",
        "'frames'",
        "'show'",
        "'breakpoints'",
        "'list'",
        "'l'",
        "id",
        "'path'",
        "'integer'",
        "':'"
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
