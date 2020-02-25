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
    const int RESOURCE = 5;
    const int TEXT = 6;
    const int TARGET = 7;
    const int PROGRAM = 8;
    const int WINGUIAPP = 9;
    const int WINAPP = 10;
    const int LIBRARY = 11;
    const int WINLIB = 12;
    const int UNITTEST = 13;
    const int ACTIVEPROJECT = 14;
    const int ID = 15;
    const int FILEPATH = 16;
    const int ASSIGN = 17;
    const int SEMICOLON = 18;
    const int DOT = 19;

    SNGCM_LEXER_API void InitTokenIdMap();
    SNGCM_LEXER_API int GetTokenId(const std::u32string& tokenName);
    SNGCM_LEXER_API const char* GetTokenName(int tokenId);
    SNGCM_LEXER_API const char* GetTokenInfo(int tokenId);
}
#endif
