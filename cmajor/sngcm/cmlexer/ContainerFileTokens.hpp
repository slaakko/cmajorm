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
    const int WINGUIAPP = 8;
    const int WINAPP = 9;
    const int LIBRARY = 10;
    const int WINLIB = 11;
    const int UNITTEST = 12;
    const int ACTIVEPROJECT = 13;
    const int ID = 14;
    const int FILEPATH = 15;
    const int ASSIGN = 16;
    const int SEMICOLON = 17;
    const int DOT = 18;

    SNGCM_LEXER_API void InitTokenIdMap();
    SNGCM_LEXER_API int GetTokenId(const std::u32string& tokenName);
    SNGCM_LEXER_API const char* GetTokenName(int tokenId);
    SNGCM_LEXER_API const char* GetTokenInfo(int tokenId);
}
#endif
