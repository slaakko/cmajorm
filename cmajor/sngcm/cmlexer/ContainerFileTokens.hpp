#ifndef ContainerFileTokens_HPP
#define ContainerFileTokens_HPP

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/CmajorLexerApi.hpp>
#include <sngcm/cmlexer/TokenValueParsers.hpp>
#include <string>

namespace ContainerFileTokens
{
    const int END = 0;
    const int PROJECT = 1;
    const int SOLUTION = 2;
    const int REFERENCE = 3;
    const int SOURCE = 4;
    const int TEXT = 5;
    const int TARGET = 6;
    const int PROGRAM = 7;
    const int LIBRARY = 8;
    const int UNITTEST = 9;
    const int ACTIVEPROJECT = 10;
    const int ID = 11;
    const int FILEPATH = 12;
    const int ASSIGN = 13;
    const int SEMICOLON = 14;
    const int DOT = 15;

    SNGCM_LEXER_API void InitTokenIdMap();
    SNGCM_LEXER_API int GetTokenId(const std::u32string& tokenName);
    SNGCM_LEXER_API const char* GetTokenName(int tokenId);
    SNGCM_LEXER_API const char* GetTokenInfo(int tokenId);
}
#endif
