#ifndef JsonSchemaTokens_HPP
#define JsonSchemaTokens_HPP

// this file has been automatically generated from 'D:/work/soulng-project/sngjson/json/JsonSchemaLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonApi.hpp>
#include <string>

namespace JsonSchemaTokens
{
    const int END = 0;
    const int STRUCT = 1;
    const int STRING = 2;
    const int INT = 3;
    const int LONG = 4;
    const int FLOAT = 5;
    const int DOUBLE = 6;
    const int BOOL = 7;
    const int API = 8;
    const int ID = 9;
    const int SEMICOLON = 10;
    const int LBRACE = 11;
    const int RBRACE = 12;
    const int LBRACKET = 13;
    const int RBRACKET = 14;
    const int LPAREN = 15;
    const int RPAREN = 16;
    const int HASH = 17;
    const int FILEPATH = 18;
    const int INCLUDE = 19;

    SNGJSON_JSON_API void InitTokenIdMap();
    SNGJSON_JSON_API int GetTokenId(const std::u32string& tokenName);
    SNGJSON_JSON_API const char* GetTokenName(int tokenId);
    SNGJSON_JSON_API const char* GetTokenInfo(int tokenId);
}
#endif
