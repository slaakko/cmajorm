
// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/ContainerFileTokens.hpp>
#include <map>

namespace ContainerFileTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ACTIVEPROJECT"] = 12;
        tokenIdMap[U"ASSIGN"] = 15;
        tokenIdMap[U"DOT"] = 17;
        tokenIdMap[U"FILEPATH"] = 14;
        tokenIdMap[U"ID"] = 13;
        tokenIdMap[U"LIBRARY"] = 9;
        tokenIdMap[U"PROGRAM"] = 7;
        tokenIdMap[U"PROJECT"] = 1;
        tokenIdMap[U"REFERENCE"] = 3;
        tokenIdMap[U"SEMICOLON"] = 16;
        tokenIdMap[U"SOLUTION"] = 2;
        tokenIdMap[U"SOURCE"] = 4;
        tokenIdMap[U"TARGET"] = 6;
        tokenIdMap[U"TEXT"] = 5;
        tokenIdMap[U"UNITTEST"] = 11;
        tokenIdMap[U"WINAPP"] = 8;
        tokenIdMap[U"WINLIB"] = 10;
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
        "PROJECT",
        "SOLUTION",
        "REFERENCE",
        "SOURCE",
        "TEXT",
        "TARGET",
        "PROGRAM",
        "WINAPP",
        "LIBRARY",
        "WINLIB",
        "UNITTEST",
        "ACTIVEPROJECT",
        "ID",
        "FILEPATH",
        "ASSIGN",
        "SEMICOLON",
        "DOT"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'project'",
        "'solution'",
        "'reference'",
        "'source'",
        "'text'",
        "'target'",
        "'program'",
        "'winapp'",
        "'library'",
        "'winlib'",
        "'unitTest'",
        "'activeProject'",
        "identifier",
        "file path",
        "'='",
        "';'",
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
