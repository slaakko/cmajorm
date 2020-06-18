#ifndef JsonTokens_HPP
#define JsonTokens_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngjson/json/JsonLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonApi.hpp>
#include <string>

namespace JsonTokens
{
    const int END = 0;
    const int STRING = 1;
    const int NUMBER = 2;
    const int LBRACKET = 3;
    const int RBRACKET = 4;
    const int LBRACE = 5;
    const int RBRACE = 6;
    const int COMMA = 7;
    const int COLON = 8;
    const int TRUE = 9;
    const int FALSE = 10;
    const int NULLTOKEN = 11;

    SNGJSON_JSON_API void InitTokenIdMap();
    SNGJSON_JSON_API int GetTokenId(const std::u32string& tokenName);
    SNGJSON_JSON_API const char* GetTokenName(int tokenId);
    SNGJSON_JSON_API const char* GetTokenInfo(int tokenId);
}
#endif
