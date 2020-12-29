
// this file has been automatically generated from 'C:/work/soulng/sngxml/serialization/XmlSerLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngxml/serialization/XmlSerTokens.hpp>
#include <map>

namespace XmlSerTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"API"] = 1;
        tokenIdMap[U"AST"] = 33;
        tokenIdMap[U"BASE"] = 4;
        tokenIdMap[U"BOOL"] = 5;
        tokenIdMap[U"BYTE"] = 7;
        tokenIdMap[U"CHAR"] = 16;
        tokenIdMap[U"CLASS"] = 3;
        tokenIdMap[U"COLON"] = 32;
        tokenIdMap[U"DATE"] = 23;
        tokenIdMap[U"DATETIME"] = 24;
        tokenIdMap[U"DOUBLE"] = 15;
        tokenIdMap[U"DURATION"] = 26;
        tokenIdMap[U"FILEPATH"] = 38;
        tokenIdMap[U"FLOAT"] = 14;
        tokenIdMap[U"HASH"] = 36;
        tokenIdMap[U"ID"] = 2;
        tokenIdMap[U"INCLUDE"] = 37;
        tokenIdMap[U"INT"] = 10;
        tokenIdMap[U"LBRACE"] = 29;
        tokenIdMap[U"LBRACKET"] = 27;
        tokenIdMap[U"LONG"] = 12;
        tokenIdMap[U"LPAREN"] = 34;
        tokenIdMap[U"RBRACE"] = 30;
        tokenIdMap[U"RBRACKET"] = 28;
        tokenIdMap[U"RPAREN"] = 35;
        tokenIdMap[U"SBYTE"] = 6;
        tokenIdMap[U"SEMICOLON"] = 31;
        tokenIdMap[U"SHORT"] = 8;
        tokenIdMap[U"STRING"] = 20;
        tokenIdMap[U"TIME_POINT"] = 25;
        tokenIdMap[U"UCHAR"] = 18;
        tokenIdMap[U"UINT"] = 11;
        tokenIdMap[U"ULONG"] = 13;
        tokenIdMap[U"USHORT"] = 9;
        tokenIdMap[U"USTRING"] = 22;
        tokenIdMap[U"UUID"] = 19;
        tokenIdMap[U"WCHAR"] = 17;
        tokenIdMap[U"WSTRING"] = 21;
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
        "API",
        "ID",
        "CLASS",
        "BASE",
        "BOOL",
        "SBYTE",
        "BYTE",
        "SHORT",
        "USHORT",
        "INT",
        "UINT",
        "LONG",
        "ULONG",
        "FLOAT",
        "DOUBLE",
        "CHAR",
        "WCHAR",
        "UCHAR",
        "UUID",
        "STRING",
        "WSTRING",
        "USTRING",
        "DATE",
        "DATETIME",
        "TIME_POINT",
        "DURATION",
        "LBRACKET",
        "RBRACKET",
        "LBRACE",
        "RBRACE",
        "SEMICOLON",
        "COLON",
        "AST",
        "LPAREN",
        "RPAREN",
        "HASH",
        "INCLUDE",
        "FILEPATH"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'api'",
        "'identifier'",
        "'class'",
        "'base'",
        "'bool'",
        "'sbyte'",
        "'byte'",
        "'short'",
        "'ushort'",
        "'int'",
        "'uint'",
        "'long'",
        "'ulong'",
        "'float'",
        "'double'",
        "'char'",
        "'wchar'",
        "'uchar'",
        "'uuid'",
        "'string'",
        "'wstring'",
        "'ustring'",
        "'date'",
        "'datetime'",
        "'time_point'",
        "'duration'",
        "'['",
        "']'",
        "'{'",
        "'}'",
        "';'",
        "':'",
        "'*'",
        "'('",
        "')'",
        "'#'",
        "'include'",
        "'filepath'"
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
