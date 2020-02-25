
// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/ContainerFileTokens.hpp>
#include <map>

namespace ContainerFileTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ACTIVEPROJECT"] = 14;
        tokenIdMap[U"ASSIGN"] = 17;
        tokenIdMap[U"DOT"] = 19;
        tokenIdMap[U"FILEPATH"] = 16;
        tokenIdMap[U"ID"] = 15;
        tokenIdMap[U"LIBRARY"] = 11;
        tokenIdMap[U"PROGRAM"] = 8;
        tokenIdMap[U"PROJECT"] = 1;
        tokenIdMap[U"REFERENCE"] = 3;
        tokenIdMap[U"RESOURCE"] = 5;
        tokenIdMap[U"SEMICOLON"] = 18;
        tokenIdMap[U"SOLUTION"] = 2;
        tokenIdMap[U"SOURCE"] = 4;
        tokenIdMap[U"TARGET"] = 7;
        tokenIdMap[U"TEXT"] = 6;
        tokenIdMap[U"UNITTEST"] = 13;
        tokenIdMap[U"WINAPP"] = 10;
        tokenIdMap[U"WINGUIAPP"] = 9;
        tokenIdMap[U"WINLIB"] = 12;
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
        "RESOURCE",
        "TEXT",
        "TARGET",
        "PROGRAM",
        "WINGUIAPP",
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
        "'resource'",
        "'text'",
        "'target'",
        "'program'",
        "'winguiapp'",
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
