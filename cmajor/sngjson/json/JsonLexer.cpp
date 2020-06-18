
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngjson/json/JsonLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonLexer.hpp>
#include <soulng/lexer/Token.hpp>
#include <sngjson/json/JsonTokens.hpp>
#include <sngjson/json/JsonClassMap.hpp>

using namespace soulng::lexer;
using namespace JsonTokens;

JsonLexer::JsonLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_)
{
}

JsonLexer::JsonLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_)
{
}

int JsonLexer::NextState(int state, char32_t c)
{
    int i = JsonClassMap::GetClass(c);
    switch (state)
    {
        case 0:
        {
            switch (i)
            {
                case 2:
                case 3:
                case 4:
                case 5:
                {
                    return 1;
                }
                case 7:
                {
                    return 2;
                }
                case 10:
                {
                    return 3;
                }
                case 12:
                {
                    return 4;
                }
                case 16:
                {
                    return 5;
                }
                case 20:
                {
                    return 6;
                }
                case 22:
                {
                    return 7;
                }
                case 23:
                {
                    return 8;
                }
                case 24:
                {
                    return 9;
                }
                case 25:
                {
                    return 10;
                }
                case 26:
                {
                    return 11;
                }
                case 27:
                {
                    return 12;
                }
                case 28:
                {
                    return 13;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 13:
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
        case 12:
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
        case 11:
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
        case 10:
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
        case 9:
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
        case 8:
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
                case 22:
                {
                    return 7;
                }
                case 17:
                {
                    return 14;
                }
                case 18:
                case 19:
                {
                    return 15;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 15:
        {
            switch (i)
            {
                case 20:
                case 21:
                {
                    return 16;
                }
                case 22:
                {
                    return 17;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 17:
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
                case 22:
                {
                    return 17;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 16:
        {
            switch (i)
            {
                case 22:
                {
                    return 17;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 14:
        {
            switch (i)
            {
                case 22:
                {
                    return 18;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 18:
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
                case 18:
                case 19:
                {
                    return 15;
                }
                case 22:
                {
                    return 18;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 6:
        {
            switch (i)
            {
                case 22:
                {
                    return 7;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 5:
        {
            switch (i)
            {
                case 29:
                {
                    return 19;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 19:
        {
            switch (i)
            {
                case 30:
                {
                    return 20;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 20:
        {
            switch (i)
            {
                case 31:
                {
                    return 21;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 21:
        {
            switch (i)
            {
                case 18:
                {
                    return 22;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 22:
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
            return -1;
        }
        case 4:
        {
            switch (i)
            {
                case 11:
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
            switch (i)
            {
                case 13:
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
            switch (i)
            {
                case 18:
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
            return -1;
        }
        case 3:
        {
            switch (i)
            {
                case 13:
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
            switch (i)
            {
                case 30:
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
            switch (i)
            {
                case 30:
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
        case 2:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
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
                {
                    return 29;
                }
                case 7:
                {
                    return 30;
                }
                case 8:
                {
                    return 31;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 31:
        {
            switch (i)
            {
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 15:
                case 16:
                {
                    return 32;
                }
                case 13:
                {
                    return 33;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 33:
        {
            switch (i)
            {
                case 14:
                case 15:
                case 16:
                case 18:
                case 19:
                case 22:
                case 29:
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
            switch (i)
            {
                case 14:
                case 15:
                case 16:
                case 18:
                case 19:
                case 22:
                case 29:
                {
                    return 35;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 35:
        {
            switch (i)
            {
                case 14:
                case 15:
                case 16:
                case 18:
                case 19:
                case 22:
                case 29:
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
            switch (i)
            {
                case 14:
                case 15:
                case 16:
                case 18:
                case 19:
                case 22:
                case 29:
                {
                    return 37;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 37:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
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
                {
                    return 29;
                }
                case 7:
                {
                    return 30;
                }
                case 8:
                {
                    return 31;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 32:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
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
                {
                    return 29;
                }
                case 7:
                {
                    return 30;
                }
                case 8:
                {
                    return 31;
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
            return -1;
        }
        case 29:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
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
                {
                    return 29;
                }
                case 7:
                {
                    return 30;
                }
                case 8:
                {
                    return 31;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 1:
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
                case 2:
                case 3:
                case 4:
                case 5:
                {
                    return 1;
                }
                default:
                {
                    return -1;
                }
            }
        }
    }
    return -1;
}

int JsonLexer::GetTokenId(int statementIndex)
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
            return STRING;
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
            return LBRACKET;
            break;
        }
        case 4:
        {
            Retract();
            return RBRACKET;
            break;
        }
        case 5:
        {
            Retract();
            return LBRACE;
            break;
        }
        case 6:
        {
            Retract();
            return RBRACE;
            break;
        }
        case 7:
        {
            Retract();
            return COMMA;
            break;
        }
        case 8:
        {
            Retract();
            return COLON;
            break;
        }
        case 9:
        {
            Retract();
            return TRUE;
            break;
        }
        case 10:
        {
            Retract();
            return FALSE;
            break;
        }
        case 11:
        {
            Retract();
            return NULLTOKEN;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
