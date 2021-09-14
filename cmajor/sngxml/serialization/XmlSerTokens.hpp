#ifndef XmlSerTokens_HPP
#define XmlSerTokens_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngxml/serialization/XmlSerLexer.lexer' using soulng lexer generator slg version 4.0.0

#include <sngxml/serialization/XmlSerApi.hpp>
#include <string>

namespace XmlSerTokens
{
    const int END = 0;
    const int API = 1;
    const int ID = 2;
    const int CLASS = 3;
    const int ENUM = 4;
    const int BASE = 5;
    const int BOOL = 6;
    const int SBYTE = 7;
    const int BYTE = 8;
    const int SHORT = 9;
    const int USHORT = 10;
    const int INT = 11;
    const int UINT = 12;
    const int LONG = 13;
    const int ULONG = 14;
    const int FLOAT = 15;
    const int DOUBLE = 16;
    const int CHAR = 17;
    const int WCHAR = 18;
    const int UCHAR = 19;
    const int UUID = 20;
    const int STRING = 21;
    const int WSTRING = 22;
    const int USTRING = 23;
    const int DATE = 24;
    const int DATETIME = 25;
    const int TIMESTAMP = 26;
    const int TIME_POINT = 27;
    const int DURATION = 28;
    const int LBRACKET = 29;
    const int RBRACKET = 30;
    const int LBRACE = 31;
    const int RBRACE = 32;
    const int SEMICOLON = 33;
    const int COLON = 34;
    const int AST = 35;
    const int LPAREN = 36;
    const int RPAREN = 37;
    const int COMMA = 38;
    const int HASH = 39;
    const int INCLUDE = 40;
    const int FILEPATH = 41;
    const int NAMESPACE = 42;

    SNGXML_SERIALIZATION_API void InitTokenIdMap();
    SNGXML_SERIALIZATION_API int GetTokenId(const std::u32string& tokenName);
    SNGXML_SERIALIZATION_API const char* GetTokenName(int tokenId);
    SNGXML_SERIALIZATION_API const char* GetTokenInfo(int tokenId);
}
#endif
