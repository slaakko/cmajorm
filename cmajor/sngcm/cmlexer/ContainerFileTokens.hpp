#ifndef ContainerFileTokens_HPP
#define ContainerFileTokens_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 4.0.0

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
    const int ACTION = 7;
    const int TARGET = 8;
    const int PROGRAM = 9;
    const int WINGUIAPP = 10;
    const int WINAPP = 11;
    const int LIBRARY = 12;
    const int WINLIB = 13;
    const int UNITTEST = 14;
    const int ACTIVEPROJECT = 15;
    const int ID = 16;
    const int FILEPATH = 17;
    const int ASSIGN = 18;
    const int SEMICOLON = 19;
    const int DOT = 20;

    SNGCM_LEXER_API void InitTokenIdMap();
    SNGCM_LEXER_API int GetTokenId(const std::u32string& tokenName);
    SNGCM_LEXER_API const char* GetTokenName(int tokenId);
    SNGCM_LEXER_API const char* GetTokenInfo(int tokenId);
}
#endif
