
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebuggerCommand.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebuggerCommandTokens.hpp>
#include <map>

namespace DebuggerCommandTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"AMP"] = 40;
        tokenIdMap[U"AST"] = 39;
        tokenIdMap[U"B"] = 19;
        tokenIdMap[U"BASE"] = 33;
        tokenIdMap[U"BREAK"] = 18;
        tokenIdMap[U"BREAKPOINT"] = 29;
        tokenIdMap[U"BREAKPOINTS"] = 30;
        tokenIdMap[U"C"] = 13;
        tokenIdMap[U"CAST"] = 31;
        tokenIdMap[U"COLON"] = 38;
        tokenIdMap[U"COMMA"] = 48;
        tokenIdMap[U"CONTINUE"] = 12;
        tokenIdMap[U"D"] = 21;
        tokenIdMap[U"DELETE"] = 20;
        tokenIdMap[U"DEPTH"] = 24;
        tokenIdMap[U"DOT"] = 47;
        tokenIdMap[U"E"] = 3;
        tokenIdMap[U"EXIT"] = 2;
        tokenIdMap[U"F"] = 15;
        tokenIdMap[U"FINISH"] = 14;
        tokenIdMap[U"FRAMES"] = 25;
        tokenIdMap[U"H"] = 7;
        tokenIdMap[U"HELP"] = 6;
        tokenIdMap[U"ID"] = 34;
        tokenIdMap[U"INTEGER"] = 36;
        tokenIdMap[U"L"] = 27;
        tokenIdMap[U"LANGLE"] = 43;
        tokenIdMap[U"LBRACKET"] = 45;
        tokenIdMap[U"LIST"] = 26;
        tokenIdMap[U"LPAREN"] = 41;
        tokenIdMap[U"MINUS"] = 50;
        tokenIdMap[U"N"] = 9;
        tokenIdMap[U"NEXT"] = 8;
        tokenIdMap[U"NL"] = 1;
        tokenIdMap[U"P"] = 23;
        tokenIdMap[U"PATH"] = 35;
        tokenIdMap[U"PLUS"] = 49;
        tokenIdMap[U"PRINT"] = 22;
        tokenIdMap[U"Q"] = 5;
        tokenIdMap[U"QUIT"] = 4;
        tokenIdMap[U"RANGLE"] = 44;
        tokenIdMap[U"RBRACKET"] = 46;
        tokenIdMap[U"RPAREN"] = 42;
        tokenIdMap[U"S"] = 11;
        tokenIdMap[U"SHOW"] = 28;
        tokenIdMap[U"STEP"] = 10;
        tokenIdMap[U"STRING"] = 37;
        tokenIdMap[U"TYPEID"] = 32;
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
        "PRINT",
        "P",
        "DEPTH",
        "FRAMES",
        "LIST",
        "L",
        "SHOW",
        "BREAKPOINT",
        "BREAKPOINTS",
        "CAST",
        "TYPEID",
        "BASE",
        "ID",
        "PATH",
        "INTEGER",
        "STRING",
        "COLON",
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
        "'print'",
        "'p'",
        "'depth'",
        "'frames'",
        "'list'",
        "'l'",
        "'show'",
        "'breakpoint'",
        "'breakpoints'",
        "'cast'",
        "'typeid'",
        "'base'",
        "id",
        "'path'",
        "'integer'",
        "'string'",
        "':'",
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
