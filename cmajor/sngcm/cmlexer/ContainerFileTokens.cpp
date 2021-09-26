
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 4.0.0

#include <sngcm/cmlexer/ContainerFileTokens.hpp>
#include <map>

namespace ContainerFileTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ACTION"] = 7;
        tokenIdMap[U"ACTIVEPROJECT"] = 15;
        tokenIdMap[U"ASSIGN"] = 18;
        tokenIdMap[U"DOT"] = 20;
        tokenIdMap[U"FILEPATH"] = 17;
        tokenIdMap[U"ID"] = 16;
        tokenIdMap[U"LIBRARY"] = 12;
        tokenIdMap[U"PROGRAM"] = 9;
        tokenIdMap[U"PROJECT"] = 1;
        tokenIdMap[U"REFERENCE"] = 3;
        tokenIdMap[U"RESOURCE"] = 5;
        tokenIdMap[U"SEMICOLON"] = 19;
        tokenIdMap[U"SOLUTION"] = 2;
        tokenIdMap[U"SOURCE"] = 4;
        tokenIdMap[U"TARGET"] = 8;
        tokenIdMap[U"TEXT"] = 6;
        tokenIdMap[U"UNITTEST"] = 14;
        tokenIdMap[U"WINAPP"] = 11;
        tokenIdMap[U"WINGUIAPP"] = 10;
        tokenIdMap[U"WINLIB"] = 13;
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
        "ACTION",
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
        "'action'",
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
