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
    const int PORT = 4;
    const int DEFAULT = 5;
    const int TOOLCHAIN = 6;
    const int CLANG = 7;
    const int GCC = 8;
    const int VS = 9;
    const int PUSH = 10;
    const int BUILD = 11;
    const int DEBUG = 12;
    const int RELEASE = 13;
    const int INSTALL = 14;
    const int TO = 15;
    const int FROM = 16;
    const int USING = 17;
    const int SERVER = 18;
    const int ID = 19;
    const int FILEPATH = 20;
    const int INTEGER = 21;
    const int VERBOSE = 22;
    const int FORCE = 23;
    const int ONLY = 24;
    const int ALL = 25;
    const int CONFIG = 26;
    const int EQ = 27;
    const int LBRACKET = 28;
    const int RBRACKET = 29;

    BUILD_API void InitTokenIdMap();
    BUILD_API int GetTokenId(const std::u32string& tokenName);
    BUILD_API const char* GetTokenName(int tokenId);
    BUILD_API const char* GetTokenInfo(int tokenId);
}
#endif
