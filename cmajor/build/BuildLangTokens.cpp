
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLang.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/build/BuildLangTokens.hpp>
#include <map>

namespace BuildLangTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ADD"] = 1;
        tokenIdMap[U"ALL"] = 29;
        tokenIdMap[U"BUILD"] = 13;
        tokenIdMap[U"CLANG"] = 9;
        tokenIdMap[U"CONFIG"] = 31;
        tokenIdMap[U"CONFIGURATION"] = 6;
        tokenIdMap[U"DEBUG"] = 15;
        tokenIdMap[U"DEFAULT"] = 7;
        tokenIdMap[U"DOT"] = 35;
        tokenIdMap[U"EQ"] = 34;
        tokenIdMap[U"FILEPATH"] = 23;
        tokenIdMap[U"FORCE"] = 27;
        tokenIdMap[U"FROM"] = 19;
        tokenIdMap[U"GCC"] = 10;
        tokenIdMap[U"HOST"] = 4;
        tokenIdMap[U"ID"] = 22;
        tokenIdMap[U"INSTALL"] = 17;
        tokenIdMap[U"INTEGER"] = 24;
        tokenIdMap[U"LBRACKET"] = 32;
        tokenIdMap[U"MESSAGES"] = 26;
        tokenIdMap[U"ONLY"] = 28;
        tokenIdMap[U"PORT"] = 5;
        tokenIdMap[U"PUSH"] = 12;
        tokenIdMap[U"RBRACKET"] = 33;
        tokenIdMap[U"REBUILD"] = 30;
        tokenIdMap[U"RELEASE"] = 16;
        tokenIdMap[U"REMOVE"] = 2;
        tokenIdMap[U"RUN"] = 3;
        tokenIdMap[U"SERVER"] = 21;
        tokenIdMap[U"SHOW"] = 14;
        tokenIdMap[U"TO"] = 18;
        tokenIdMap[U"TOOLCHAIN"] = 8;
        tokenIdMap[U"USING"] = 20;
        tokenIdMap[U"VERBOSE"] = 25;
        tokenIdMap[U"VS"] = 11;
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
        "HOST",
        "PORT",
        "CONFIGURATION",
        "DEFAULT",
        "TOOLCHAIN",
        "CLANG",
        "GCC",
        "VS",
        "PUSH",
        "BUILD",
        "SHOW",
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
        "MESSAGES",
        "FORCE",
        "ONLY",
        "ALL",
        "REBUILD",
        "CONFIG",
        "LBRACKET",
        "RBRACKET",
        "EQ",
        "DOT"
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
        "'host'",
        "'port'",
        "'configuration'",
        "'default'",
        "'toolchain'",
        "'clang'",
        "'gcc'",
        "'vs'",
        "'push'",
        "'build'",
        "'show'",
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
        "'messages'",
        "'force'",
        "'only",
        "'all'",
        "'rebuild'",
        "'config'",
        "'['",
        "']'",
        "'='",
        "'.'"
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
