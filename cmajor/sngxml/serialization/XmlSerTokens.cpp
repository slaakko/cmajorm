
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngxml/serialization/XmlSerLexer.lexer' using soulng lexer generator slg version 4.0.0

#include <sngxml/serialization/XmlSerTokens.hpp>
#include <map>

namespace XmlSerTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"API"] = 1;
        tokenIdMap[U"ASSIGN"] = 39;
        tokenIdMap[U"AST"] = 35;
        tokenIdMap[U"BASE"] = 5;
        tokenIdMap[U"BOOL"] = 6;
        tokenIdMap[U"BYTE"] = 8;
        tokenIdMap[U"CHAR"] = 17;
        tokenIdMap[U"CLASS"] = 3;
        tokenIdMap[U"COLON"] = 34;
        tokenIdMap[U"COLONCOLON"] = 44;
        tokenIdMap[U"COMMA"] = 38;
        tokenIdMap[U"DATE"] = 24;
        tokenIdMap[U"DATETIME"] = 25;
        tokenIdMap[U"DOUBLE"] = 16;
        tokenIdMap[U"DURATION"] = 28;
        tokenIdMap[U"ENDCPP"] = 41;
        tokenIdMap[U"ENUM"] = 4;
        tokenIdMap[U"FILEPATH"] = 47;
        tokenIdMap[U"FLOAT"] = 15;
        tokenIdMap[U"HASH"] = 45;
        tokenIdMap[U"ID"] = 2;
        tokenIdMap[U"INCLUDE"] = 46;
        tokenIdMap[U"INT"] = 11;
        tokenIdMap[U"LANGLE"] = 42;
        tokenIdMap[U"LBRACE"] = 31;
        tokenIdMap[U"LBRACKET"] = 29;
        tokenIdMap[U"LONG"] = 13;
        tokenIdMap[U"LPAREN"] = 36;
        tokenIdMap[U"NAMESPACE"] = 48;
        tokenIdMap[U"RANGLE"] = 43;
        tokenIdMap[U"RBRACE"] = 32;
        tokenIdMap[U"RBRACKET"] = 30;
        tokenIdMap[U"RPAREN"] = 37;
        tokenIdMap[U"SBYTE"] = 7;
        tokenIdMap[U"SEMICOLON"] = 33;
        tokenIdMap[U"SHORT"] = 9;
        tokenIdMap[U"STARTCPP"] = 40;
        tokenIdMap[U"STRING"] = 21;
        tokenIdMap[U"TIMESTAMP"] = 26;
        tokenIdMap[U"TIME_POINT"] = 27;
        tokenIdMap[U"UCHAR"] = 19;
        tokenIdMap[U"UINT"] = 12;
        tokenIdMap[U"ULONG"] = 14;
        tokenIdMap[U"USHORT"] = 10;
        tokenIdMap[U"USING"] = 49;
        tokenIdMap[U"USTRING"] = 23;
        tokenIdMap[U"UUID"] = 20;
        tokenIdMap[U"WCHAR"] = 18;
        tokenIdMap[U"WSTRING"] = 22;
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
        "ENUM",
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
        "TIMESTAMP",
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
        "COMMA",
        "ASSIGN",
        "STARTCPP",
        "ENDCPP",
        "LANGLE",
        "RANGLE",
        "COLONCOLON",
        "HASH",
        "INCLUDE",
        "FILEPATH",
        "NAMESPACE",
        "USING"
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
        "'enum'",
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
        "'timestamp'",
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
        "','",
        "'='",
        "'<%'",
        "'%>'",
        "'<'",
        "'>'",
        "'::'",
        "'#'",
        "'include'",
        "'filepath'",
        "'namespace'",
        "'using'"
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
