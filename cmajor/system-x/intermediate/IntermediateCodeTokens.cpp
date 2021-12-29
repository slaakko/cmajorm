
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/intermediate/IntermediateCodeLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/intermediate/IntermediateCodeTokens.hpp>
#include <map>

namespace IntermediateCodeTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ADD"] = 26;
        tokenIdMap[U"ALIGNMENT"] = 49;
        tokenIdMap[U"AND"] = 31;
        tokenIdMap[U"ARG"] = 8;
        tokenIdMap[U"ASSIGN"] = 77;
        tokenIdMap[U"AST"] = 72;
        tokenIdMap[U"AT"] = 78;
        tokenIdMap[U"BITCAST"] = 21;
        tokenIdMap[U"BOOL"] = 51;
        tokenIdMap[U"BRANCH"] = 10;
        tokenIdMap[U"BYTE"] = 53;
        tokenIdMap[U"CALL"] = 11;
        tokenIdMap[U"CLSID"] = 82;
        tokenIdMap[U"COLON"] = 73;
        tokenIdMap[U"COMMA"] = 69;
        tokenIdMap[U"CONV"] = 65;
        tokenIdMap[U"CU"] = 1;
        tokenIdMap[U"DATA"] = 4;
        tokenIdMap[U"DIV"] = 29;
        tokenIdMap[U"DOLLAR"] = 79;
        tokenIdMap[U"DOUBLE"] = 61;
        tokenIdMap[U"ELEMADDR"] = 41;
        tokenIdMap[U"EQUAL"] = 36;
        tokenIdMap[U"EXCLAMATION"] = 80;
        tokenIdMap[U"FALSE"] = 63;
        tokenIdMap[U"FLOAT"] = 60;
        tokenIdMap[U"FLOATTOINT"] = 23;
        tokenIdMap[U"FUNCTION"] = 5;
        tokenIdMap[U"HEXNUM"] = 85;
        tokenIdMap[U"ID"] = 2;
        tokenIdMap[U"INT"] = 56;
        tokenIdMap[U"INTTOFLOAT"] = 22;
        tokenIdMap[U"INTTOPTR"] = 24;
        tokenIdMap[U"JMP"] = 9;
        tokenIdMap[U"LBRACE"] = 70;
        tokenIdMap[U"LBRACKET"] = 75;
        tokenIdMap[U"LESS"] = 37;
        tokenIdMap[U"LOAD"] = 40;
        tokenIdMap[U"LOCAL"] = 39;
        tokenIdMap[U"LONG"] = 58;
        tokenIdMap[U"LPAREN"] = 67;
        tokenIdMap[U"METADATA"] = 66;
        tokenIdMap[U"MOD"] = 30;
        tokenIdMap[U"MUL"] = 28;
        tokenIdMap[U"NEG"] = 17;
        tokenIdMap[U"NOP"] = 14;
        tokenIdMap[U"NOT"] = 16;
        tokenIdMap[U"NULL_TOKEN"] = 64;
        tokenIdMap[U"NUMBER"] = 84;
        tokenIdMap[U"OFFSET"] = 47;
        tokenIdMap[U"ONCE"] = 6;
        tokenIdMap[U"OR"] = 32;
        tokenIdMap[U"PARAM"] = 38;
        tokenIdMap[U"PHI"] = 45;
        tokenIdMap[U"PTRDIFF"] = 43;
        tokenIdMap[U"PTROFFSET"] = 42;
        tokenIdMap[U"PTRTOINT"] = 25;
        tokenIdMap[U"RBRACE"] = 71;
        tokenIdMap[U"RBRACKET"] = 76;
        tokenIdMap[U"RET"] = 12;
        tokenIdMap[U"RPAREN"] = 68;
        tokenIdMap[U"SAVE"] = 15;
        tokenIdMap[U"SBYTE"] = 52;
        tokenIdMap[U"SEMICOLON"] = 74;
        tokenIdMap[U"SHL"] = 34;
        tokenIdMap[U"SHORT"] = 54;
        tokenIdMap[U"SHR"] = 35;
        tokenIdMap[U"SIGNEXTEND"] = 18;
        tokenIdMap[U"SIZE"] = 48;
        tokenIdMap[U"STORE"] = 7;
        tokenIdMap[U"STRING"] = 83;
        tokenIdMap[U"SUB"] = 27;
        tokenIdMap[U"SWITCH"] = 13;
        tokenIdMap[U"TRAP"] = 44;
        tokenIdMap[U"TRUE"] = 62;
        tokenIdMap[U"TRUNCATE"] = 20;
        tokenIdMap[U"TYPE"] = 46;
        tokenIdMap[U"TYPEID"] = 81;
        tokenIdMap[U"TYPES"] = 3;
        tokenIdMap[U"UINT"] = 57;
        tokenIdMap[U"ULONG"] = 59;
        tokenIdMap[U"USHORT"] = 55;
        tokenIdMap[U"VOID"] = 50;
        tokenIdMap[U"XOR"] = 33;
        tokenIdMap[U"ZEROEXTEND"] = 19;
    }

    int GetTokenId(const std::u32string& tokenName)
    {
        auto it = tokenIdMap.find(tokenName);
        if (it != tokenIdMap.cend())
        {
            return it->second;
        }
        else
        {
            return -1;
        }
    }

    const char* tokenName[] =
    {
        "END",
        "CU",
        "ID",
        "TYPES",
        "DATA",
        "FUNCTION",
        "ONCE",
        "STORE",
        "ARG",
        "JMP",
        "BRANCH",
        "CALL",
        "RET",
        "SWITCH",
        "NOP",
        "SAVE",
        "NOT",
        "NEG",
        "SIGNEXTEND",
        "ZEROEXTEND",
        "TRUNCATE",
        "BITCAST",
        "INTTOFLOAT",
        "FLOATTOINT",
        "INTTOPTR",
        "PTRTOINT",
        "ADD",
        "SUB",
        "MUL",
        "DIV",
        "MOD",
        "AND",
        "OR",
        "XOR",
        "SHL",
        "SHR",
        "EQUAL",
        "LESS",
        "PARAM",
        "LOCAL",
        "LOAD",
        "ELEMADDR",
        "PTROFFSET",
        "PTRDIFF",
        "TRAP",
        "PHI",
        "TYPE",
        "OFFSET",
        "SIZE",
        "ALIGNMENT",
        "VOID",
        "BOOL",
        "SBYTE",
        "BYTE",
        "SHORT",
        "USHORT",
        "INT",
        "UINT",
        "LONG",
        "ULONG",
        "FLOAT",
        "DOUBLE",
        "TRUE",
        "FALSE",
        "NULL_TOKEN",
        "CONV",
        "METADATA",
        "LPAREN",
        "RPAREN",
        "COMMA",
        "LBRACE",
        "RBRACE",
        "AST",
        "COLON",
        "SEMICOLON",
        "LBRACKET",
        "RBRACKET",
        "ASSIGN",
        "AT",
        "DOLLAR",
        "EXCLAMATION",
        "TYPEID",
        "CLSID",
        "STRING",
        "NUMBER",
        "HEXNUM"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'cu'",
        "identifier",
        "'types'",
        "'data'",
        "'function'",
        "'once'",
        "'store'",
        "'arg'",
        "'jmp'",
        "'branch'",
        "'call'",
        "'ret'",
        "'switch'",
        "'nop'",
        "'save'",
        "'not'",
        "'neg'",
        "'signextend'",
        "'zeroextend'",
        "'truncate'",
        "'bitcast'",
        "'inttofloat'",
        "'floattoint'",
        "'inttoptr'",
        "'ptrtoint'",
        "'add'",
        "'sub'",
        "'mul'",
        "'div'",
        "'mod'",
        "'and'",
        "'or'",
        "'xor'",
        "'shl'",
        "'shr'",
        "'equal'",
        "'less'",
        "'param'",
        "'local'",
        "'load'",
        "'elemaddr'",
        "'ptroffset'",
        "'ptrdiff'",
        "'trap'",
        "'phi'",
        "'type'",
        "'offset'",
        "'size'",
        "'alignment'",
        "'void'",
        "'bool'",
        "'sbyte'",
        "'byte'",
        "'short'",
        "'ushort'",
        "'int'",
        "'uint'",
        "'long'",
        "'ulong'",
        "'float'",
        "'double'",
        "'true'",
        "'false'",
        "'null'",
        "'conv'",
        "'metadata'",
        "'('",
        "')'",
        "','",
        "'{'",
        "'}'",
        "'*'",
        "':'",
        "';'",
        "'['",
        "']'",
        "'='",
        "'@'",
        "'$'",
        "'!'",
        "type identifier",
        "class identifier",
        "string",
        "number",
        "hex number"
    };

    const char* GetTokenInfo(int tokenId)
    {
        return tokenInfo[tokenId];
    }

    struct Initializer
    {
        Initializer() { InitTokenIdMap(); }
    };

    Initializer initializer;
}
