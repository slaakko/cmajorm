
// this file has been automatically generated from 'D:/work/soulng-project/sngxml/xpath/XPathLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngxml/xpath/XPathTokens.hpp>
#include <map>

namespace XPathTokens
{
    std::map<std::u32string, int> tokenIdMap;

    void InitTokenIdMap()
    {
        tokenIdMap[U"ANCESTOR"] = 23;
        tokenIdMap[U"ANCESTOR_OR_SELF"] = 24;
        tokenIdMap[U"AND"] = 2;
        tokenIdMap[U"AT"] = 36;
        tokenIdMap[U"ATTRIBUTE"] = 25;
        tokenIdMap[U"CHILD"] = 26;
        tokenIdMap[U"COLON"] = 20;
        tokenIdMap[U"COLONCOLON"] = 19;
        tokenIdMap[U"COMMA"] = 22;
        tokenIdMap[U"COMMENT"] = 42;
        tokenIdMap[U"DESCENDANT"] = 27;
        tokenIdMap[U"DESCENDANT_OR_SELF"] = 28;
        tokenIdMap[U"DIV"] = 12;
        tokenIdMap[U"DOLLAR"] = 21;
        tokenIdMap[U"DOT"] = 18;
        tokenIdMap[U"DOTDOT"] = 17;
        tokenIdMap[U"DQSTRING"] = 45;
        tokenIdMap[U"EQ"] = 3;
        tokenIdMap[U"FOLLOWING"] = 29;
        tokenIdMap[U"FOLLOWING_SIBLING"] = 30;
        tokenIdMap[U"GEQ"] = 6;
        tokenIdMap[U"GREATER"] = 8;
        tokenIdMap[U"LBRACKET"] = 37;
        tokenIdMap[U"LEQ"] = 5;
        tokenIdMap[U"LESS"] = 7;
        tokenIdMap[U"LPAREN"] = 40;
        tokenIdMap[U"MINUS"] = 10;
        tokenIdMap[U"MOD"] = 13;
        tokenIdMap[U"NAME"] = 48;
        tokenIdMap[U"NAMESPACE"] = 31;
        tokenIdMap[U"NEQ"] = 4;
        tokenIdMap[U"NODE"] = 44;
        tokenIdMap[U"NUMBER"] = 47;
        tokenIdMap[U"OR"] = 1;
        tokenIdMap[U"PARENT"] = 32;
        tokenIdMap[U"PLUS"] = 9;
        tokenIdMap[U"PRECEDING"] = 33;
        tokenIdMap[U"PRECEDING_SIBLING"] = 34;
        tokenIdMap[U"PROCESSING_INSTRUCTION"] = 39;
        tokenIdMap[U"RBRACKET"] = 38;
        tokenIdMap[U"RPAREN"] = 41;
        tokenIdMap[U"SELF"] = 35;
        tokenIdMap[U"SLASH"] = 16;
        tokenIdMap[U"SLASHSLASH"] = 15;
        tokenIdMap[U"SQSTRING"] = 46;
        tokenIdMap[U"STAR"] = 11;
        tokenIdMap[U"TEXT"] = 43;
        tokenIdMap[U"UNION"] = 14;
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
        "OR",
        "AND",
        "EQ",
        "NEQ",
        "LEQ",
        "GEQ",
        "LESS",
        "GREATER",
        "PLUS",
        "MINUS",
        "STAR",
        "DIV",
        "MOD",
        "UNION",
        "SLASHSLASH",
        "SLASH",
        "DOTDOT",
        "DOT",
        "COLONCOLON",
        "COLON",
        "DOLLAR",
        "COMMA",
        "ANCESTOR",
        "ANCESTOR_OR_SELF",
        "ATTRIBUTE",
        "CHILD",
        "DESCENDANT",
        "DESCENDANT_OR_SELF",
        "FOLLOWING",
        "FOLLOWING_SIBLING",
        "NAMESPACE",
        "PARENT",
        "PRECEDING",
        "PRECEDING_SIBLING",
        "SELF",
        "AT",
        "LBRACKET",
        "RBRACKET",
        "PROCESSING_INSTRUCTION",
        "LPAREN",
        "RPAREN",
        "COMMENT",
        "TEXT",
        "NODE",
        "DQSTRING",
        "SQSTRING",
        "NUMBER",
        "NAME"
    };

    const char* GetTokenName(int tokenId)
    {
        return tokenName[tokenId];
    }

    const char* tokenInfo[] =
    {
        "end of file",
        "'or'",
        "'and'",
        "'='",
        "'!='",
        "'<='",
        "'>='",
        "'<'",
        "'>'",
        "'+'",
        "'-'",
        "'*",
        "'div'",
        "'mod'",
        "'|'",
        "'//'",
        "'/'",
        "'..'",
        "'.'",
        "'::'",
        "':'",
        "'$'",
        "','",
        "'ancestor'",
        "'ancestor-or-self'",
        "'attribute'",
        "'child'",
        "'descendant'",
        "'descendant-or-self'",
        "'following'",
        "'following-sibling'",
        "'namespace'",
        "'parent'",
        "'preceding'",
        "'preceding-sibling'",
        "'self'",
        "'@'",
        "'['",
        "']'",
        "'processing-instruction'",
        "'('",
        "')'",
        "'comment'",
        "'text'",
        "'node'",
        "string",
        "string",
        "number",
        "name"
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
