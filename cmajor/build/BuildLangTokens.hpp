#ifndef BuildLangTokens_HPP
#define BuildLangTokens_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLang.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/build/BuildApi.hpp>
#include <string>

namespace BuildLangTokens
{
    const int END = 0;
    const int ADD = 1;
    const int REMOVE = 2;
    const int RUN = 3;
    const int HOST = 4;
    const int PORT = 5;
    const int CONFIGURATION = 6;
    const int DEFAULT = 7;
    const int TOOLCHAIN = 8;
    const int CLANG = 9;
    const int GCC = 10;
    const int VS = 11;
    const int PUSH = 12;
    const int BUILD = 13;
    const int SHOW = 14;
    const int DEBUG = 15;
    const int RELEASE = 16;
    const int INSTALL = 17;
    const int TO = 18;
    const int FROM = 19;
    const int USING = 20;
    const int SERVER = 21;
    const int ID = 22;
    const int FILEPATH = 23;
    const int INTEGER = 24;
    const int VERBOSE = 25;
    const int MESSAGES = 26;
    const int FORCE = 27;
    const int ONLY = 28;
    const int ALL = 29;
    const int REBUILD = 30;
    const int CONFIG = 31;
    const int LBRACKET = 32;
    const int RBRACKET = 33;
    const int EQ = 34;
    const int DOT = 35;

    BUILD_API void InitTokenIdMap();
    BUILD_API int GetTokenId(const std::u32string& tokenName);
    BUILD_API const char* GetTokenName(int tokenId);
    BUILD_API const char* GetTokenInfo(int tokenId);
}
#endif
