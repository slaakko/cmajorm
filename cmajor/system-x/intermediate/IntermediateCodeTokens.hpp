#ifndef IntermediateCodeTokens_HPP
#define IntermediateCodeTokens_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/intermediate/IntermediateCodeLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/intermediate/Api.hpp>
#include <string>

namespace IntermediateCodeTokens
{
    const int END = 0;
    const int CU = 1;
    const int ID = 2;
    const int TYPES = 3;
    const int DATA = 4;
    const int FUNCTION = 5;
    const int ONCE = 6;
    const int STORE = 7;
    const int ARG = 8;
    const int JMP = 9;
    const int BRANCH = 10;
    const int CALL = 11;
    const int RET = 12;
    const int SWITCH = 13;
    const int NOP = 14;
    const int SAVE = 15;
    const int NOT = 16;
    const int NEG = 17;
    const int SIGNEXTEND = 18;
    const int ZEROEXTEND = 19;
    const int TRUNCATE = 20;
    const int BITCAST = 21;
    const int INTTOFLOAT = 22;
    const int FLOATTOINT = 23;
    const int INTTOPTR = 24;
    const int PTRTOINT = 25;
    const int ADD = 26;
    const int SUB = 27;
    const int MUL = 28;
    const int DIV = 29;
    const int MOD = 30;
    const int AND = 31;
    const int OR = 32;
    const int XOR = 33;
    const int SHL = 34;
    const int SHR = 35;
    const int EQUAL = 36;
    const int LESS = 37;
    const int PARAM = 38;
    const int LOCAL = 39;
    const int LOAD = 40;
    const int ELEMADDR = 41;
    const int PTROFFSET = 42;
    const int PTRDIFF = 43;
    const int TRAP = 44;
    const int PHI = 45;
    const int TYPE = 46;
    const int OFFSET = 47;
    const int SIZE = 48;
    const int ALIGNMENT = 49;
    const int VOID = 50;
    const int BOOL = 51;
    const int SBYTE = 52;
    const int BYTE = 53;
    const int SHORT = 54;
    const int USHORT = 55;
    const int INT = 56;
    const int UINT = 57;
    const int LONG = 58;
    const int ULONG = 59;
    const int FLOAT = 60;
    const int DOUBLE = 61;
    const int TRUE = 62;
    const int FALSE = 63;
    const int NULL_TOKEN = 64;
    const int CONV = 65;
    const int METADATA = 66;
    const int LPAREN = 67;
    const int RPAREN = 68;
    const int COMMA = 69;
    const int LBRACE = 70;
    const int RBRACE = 71;
    const int AST = 72;
    const int COLON = 73;
    const int SEMICOLON = 74;
    const int LBRACKET = 75;
    const int RBRACKET = 76;
    const int ASSIGN = 77;
    const int AT = 78;
    const int DOLLAR = 79;
    const int EXCLAMATION = 80;
    const int TYPEID = 81;
    const int CLSID = 82;
    const int STRING = 83;
    const int NUMBER = 84;
    const int HEXNUM = 85;

    CMSX_INTERMEDIATE_API void InitTokenIdMap();
    CMSX_INTERMEDIATE_API int GetTokenId(const std::u32string& tokenName);
    CMSX_INTERMEDIATE_API const char* GetTokenName(int tokenId);
    CMSX_INTERMEDIATE_API const char* GetTokenInfo(int tokenId);
}
#endif
