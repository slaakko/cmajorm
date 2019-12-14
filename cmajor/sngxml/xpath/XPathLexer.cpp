
// this file has been automatically generated from 'D:/work/soulng-project/sngxml/xpath/XPathLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngxml/xpath/XPathLexer.hpp>
#include <sngxml/xpath/XPathKeywords.hpp>
#include <soulng/lexer/Token.hpp>
#include <sngxml/xpath/XPathTokens.hpp>
#include <sngxml/xpath/XPathClassMap.hpp>

using namespace soulng::lexer;
using namespace XPathTokens;

XPathLexer::XPathLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_)
{
    SetKeywordMap(XPathKeywords::GetKeywordMap());
}

XPathLexer::XPathLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_)
{
    SetKeywordMap(XPathKeywords::GetKeywordMap());
}

int XPathLexer::NextState(int state, char32_t c)
{
    int i = XPathClassMap::GetClass(c);
    switch (state)
    {
        case 0:
        {
            switch (i)
            {
                case 0:
                {
                    return 1;
                }
                case 1:
                {
                    return 2;
                }
                case 4:
                case 5:
                case 6:
                case 7:
                {
                    return 3;
                }
                case 8:
                case 9:
                case 10:
                case 11:
                {
                    return 4;
                }
                case 13:
                {
                    return 5;
                }
                case 14:
                {
                    return 6;
                }
                case 16:
                {
                    return 7;
                }
                case 18:
                {
                    return 8;
                }
                case 19:
                {
                    return 9;
                }
                case 20:
                {
                    return 10;
                }
                case 21:
                {
                    return 11;
                }
                case 22:
                {
                    return 12;
                }
                case 23:
                {
                    return 13;
                }
                case 24:
                {
                    return 14;
                }
                case 25:
                {
                    return 15;
                }
                case 26:
                {
                    return 16;
                }
                case 27:
                {
                    return 17;
                }
                case 28:
                {
                    return 18;
                }
                case 29:
                {
                    return 19;
                }
                case 30:
                {
                    return 20;
                }
                case 31:
                {
                    return 21;
                }
                case 32:
                {
                    return 22;
                }
                case 33:
                {
                    return 23;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 23:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(27);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 22:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(26);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 21:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(25);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 20:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(24);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 19:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(23);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 18:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(22);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 17:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(21);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 16:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(20);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 26:
                {
                    return 24;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 24:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(19);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 15:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(16);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 25:
                {
                    return 25;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 25:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(15);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 14:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(14);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 13:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(13);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 12:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(11);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 11:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(10);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 18:
                {
                    return 26;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 26:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(8);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 10:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(9);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 18:
                {
                    return 27;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 27:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(7);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 9:
        {
            switch (i)
            {
                case 18:
                {
                    return 28;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 28:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(6);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 8:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(5);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 7:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(2);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 16:
                {
                    return 7;
                }
                case 14:
                {
                    return 29;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 29:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(2);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 16:
                {
                    return 30;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 30:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(2);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 16:
                {
                    return 30;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 6:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(18);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 14:
                {
                    return 31;
                }
                case 16:
                {
                    return 32;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 32:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(2);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 16:
                {
                    return 32;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 31:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(17);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 5:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(12);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 4:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(1);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 8:
                case 9:
                case 10:
                case 11:
                {
                    return 33;
                }
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                {
                    return 34;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 34:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(1);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 8:
                case 9:
                case 10:
                case 11:
                {
                    return 33;
                }
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                {
                    return 34;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 33:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(1);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 8:
                case 9:
                case 10:
                case 11:
                {
                    return 33;
                }
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                {
                    return 34;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 3:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(0);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            switch (i)
            {
                case 4:
                case 5:
                case 6:
                case 7:
                {
                    return 3;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 2:
        {
            switch (i)
            {
                case 0:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 32:
                case 33:
                {
                    return 35;
                }
                case 1:
                {
                    return 36;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 36:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(3);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
        case 35:
        {
            switch (i)
            {
                case 0:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 32:
                case 33:
                {
                    return 35;
                }
                case 1:
                {
                    return 36;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 1:
        {
            switch (i)
            {
                case 0:
                {
                    return 37;
                }
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 32:
                case 33:
                {
                    return 38;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 38:
        {
            switch (i)
            {
                case 0:
                {
                    return 37;
                }
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 32:
                case 33:
                {
                    return 38;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 37:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(4);
            if (tokenId == CONTINUE_TOKEN)
            {
                token.id = tokenId;
                return -1;
            }
            else if (tokenId != INVALID_TOKEN)
            {
                token.id = tokenId;
            }
            else
            {
                token.match = prevMatch;
            }
            return -1;
        }
    }
    return -1;
}

int XPathLexer::GetTokenId(int statementIndex)
{
    switch (statementIndex)
    {
        case 0:
        {
            Retract();
            break;
        }
        case 1:
        {
            Retract();
            return NAME;
            break;
        }
        case 2:
        {
            Retract();
            return NUMBER;
            break;
        }
        case 3:
        {
            Retract();
            return DQSTRING;
            break;
        }
        case 4:
        {
            Retract();
            return SQSTRING;
            break;
        }
        case 5:
        {
            Retract();
            return EQ;
            break;
        }
        case 6:
        {
            Retract();
            return NEQ;
            break;
        }
        case 7:
        {
            Retract();
            return LEQ;
            break;
        }
        case 8:
        {
            Retract();
            return GEQ;
            break;
        }
        case 9:
        {
            Retract();
            return LESS;
            break;
        }
        case 10:
        {
            Retract();
            return GREATER;
            break;
        }
        case 11:
        {
            Retract();
            return PLUS;
            break;
        }
        case 12:
        {
            Retract();
            return MINUS;
            break;
        }
        case 13:
        {
            Retract();
            return STAR;
            break;
        }
        case 14:
        {
            Retract();
            return UNION;
            break;
        }
        case 15:
        {
            Retract();
            return SLASHSLASH;
            break;
        }
        case 16:
        {
            Retract();
            return SLASH;
            break;
        }
        case 17:
        {
            Retract();
            return DOTDOT;
            break;
        }
        case 18:
        {
            Retract();
            return DOT;
            break;
        }
        case 19:
        {
            Retract();
            return COLONCOLON;
            break;
        }
        case 20:
        {
            Retract();
            return COLON;
            break;
        }
        case 21:
        {
            Retract();
            return DOLLAR;
            break;
        }
        case 22:
        {
            Retract();
            return COMMA;
            break;
        }
        case 23:
        {
            Retract();
            return AT;
            break;
        }
        case 24:
        {
            Retract();
            return LBRACKET;
            break;
        }
        case 25:
        {
            Retract();
            return RBRACKET;
            break;
        }
        case 26:
        {
            Retract();
            return LPAREN;
            break;
        }
        case 27:
        {
            Retract();
            return RPAREN;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
