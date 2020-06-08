
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLang.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/build/BuildLangTokens.hpp>
#include <map>

namespace BuildLangTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ADD"] = 1;
        tokenIdMap[U"ALL"] = 25;
        tokenIdMap[U"BUILD"] = 11;
        tokenIdMap[U"CLANG"] = 7;
        tokenIdMap[U"CONFIG"] = 26;
        tokenIdMap[U"DEBUG"] = 12;
        tokenIdMap[U"DEFAULT"] = 5;
        tokenIdMap[U"EQ"] = 27;
        tokenIdMap[U"FILEPATH"] = 20;
        tokenIdMap[U"FORCE"] = 23;
        tokenIdMap[U"FROM"] = 16;
        tokenIdMap[U"GCC"] = 8;
        tokenIdMap[U"ID"] = 19;
        tokenIdMap[U"INSTALL"] = 14;
        tokenIdMap[U"INTEGER"] = 21;
        tokenIdMap[U"LBRACKET"] = 28;
        tokenIdMap[U"ONLY"] = 24;
        tokenIdMap[U"PORT"] = 4;
        tokenIdMap[U"PUSH"] = 10;
        tokenIdMap[U"RBRACKET"] = 29;
        tokenIdMap[U"RELEASE"] = 13;
        tokenIdMap[U"REMOVE"] = 2;
        tokenIdMap[U"RUN"] = 3;
        tokenIdMap[U"SERVER"] = 18;
        tokenIdMap[U"TO"] = 15;
        tokenIdMap[U"TOOLCHAIN"] = 6;
        tokenIdMap[U"USING"] = 17;
        tokenIdMap[U"VERBOSE"] = 22;
        tokenIdMap[U"VS"] = 9;
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
        "ADD",
        "REMOVE",
        "RUN",
        "PORT",
        "DEFAULT",
        "TOOLCHAIN",
        "CLANG",
        "GCC",
        "VS",
        "PUSH",
        "BUILD",
        "DEBUG",
        "RELEASE",
        "INSTALL",
        "TO",
        "FROM",
        "USING",
        "SERVER",
        "ID",
        "FILEPATH",
        "INTEGER",
        "VERBOSE",
        "FORCE",
        "ONLY",
        "ALL",
        "CONFIG",
        "EQ",
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
        "'add'",
        "'remove'",
        "'run'",
        "'port'",
        "'default'",
        "'toolchain'",
        "'clang'",
        "'gcc'",
        "'vs'",
        "'push'",
        "'build'",
        "'debug'",
        "'release'",
        "'install'",
        "'to'",
        "'from'",
        "'using'",
        "'server'",
        "'id'",
        "'filepath'",
        "'integer'",
        "'verbose'",
        "'force'",
        "'only",
        "'all'",
        "'config'",
        "'='",
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
