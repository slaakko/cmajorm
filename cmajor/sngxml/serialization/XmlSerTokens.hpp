#ifndef XmlSerTokens_HPP
#define XmlSerTokens_HPP

// this file has been automatically generated from 'C:/work/soulng/sngxml/serialization/XmlSerLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngxml/serialization/XmlSerApi.hpp>
#include <string>

namespace XmlSerTokens
{
    const int END = 0;
    const int API = 1;
    const int ID = 2;
    const int CLASS = 3;
    const int BASE = 4;
    const int BOOL = 5;
    const int SBYTE = 6;
    const int BYTE = 7;
    const int SHORT = 8;
    const int USHORT = 9;
    const int INT = 10;
    const int UINT = 11;
    const int LONG = 12;
    const int ULONG = 13;
    const int FLOAT = 14;
    const int DOUBLE = 15;
    const int CHAR = 16;
    const int WCHAR = 17;
    const int UCHAR = 18;
    const int UUID = 19;
    const int STRING = 20;
    const int WSTRING = 21;
    const int USTRING = 22;
    const int DATE = 23;
    const int DATETIME = 24;
    const int TIME_POINT = 25;
    const int DURATION = 26;
    const int LBRACKET = 27;
    const int RBRACKET = 28;
    const int LBRACE = 29;
    const int RBRACE = 30;
    const int SEMICOLON = 31;
    const int COLON = 32;
    const int AST = 33;
    const int LPAREN = 34;
    const int RPAREN = 35;
    const int HASH = 36;
    const int INCLUDE = 37;
    const int FILEPATH = 38;

    SNGXML_SERIALIZATION_API void InitTokenIdMap();
    SNGXML_SERIALIZATION_API int GetTokenId(const std::u32string& tokenName);
    SNGXML_SERIALIZATION_API const char* GetTokenName(int tokenId);
    SNGXML_SERIALIZATION_API const char* GetTokenInfo(int tokenId);
}
#endif
