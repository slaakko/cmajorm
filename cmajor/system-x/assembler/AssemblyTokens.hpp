#ifndef AssemblyTokens_HPP
#define AssemblyTokens_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/assembler/AssemblyLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/assembler/api.hpp>
#include <string>

namespace AssemblyTokens
{
    const int END = 0;
    const int DECIMAL_CONSTANT = 1;
    const int HEX_CONSTANT = 2;
    const int CHAR_CONSTANT = 3;
    const int STRING_CONSTANT = 4;
    const int CLSID_CONSTANT = 5;
    const int LOCAL_SYMBOL = 6;
    const int SYMBOL = 7;
    const int AT = 8;
    const int PLUS = 9;
    const int MINUS = 10;
    const int TILDE = 11;
    const int DOLLAR = 12;
    const int AMP = 13;
    const int AST = 14;
    const int SLASHSLASH = 15;
    const int SLASH = 16;
    const int PERCENT = 17;
    const int SHIFT_LEFT = 18;
    const int SHIFT_RIGHT = 19;
    const int BAR = 20;
    const int CARET = 21;
    const int DOT = 22;
    const int SPACE = 23;
    const int NEWLINE = 24;
    const int LPAREN = 25;
    const int RPAREN = 26;
    const int SEMICOLON = 27;
    const int COMMA = 28;
    const int CHAR = 29;

    CMSX_ASSEMBLER_API void InitTokenIdMap();
    CMSX_ASSEMBLER_API int GetTokenId(const std::u32string& tokenName);
    CMSX_ASSEMBLER_API const char* GetTokenName(int tokenId);
    CMSX_ASSEMBLER_API const char* GetTokenInfo(int tokenId);
}
#endif
