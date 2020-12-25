
// this file has been automatically generated from 'D:/work/soulng-project/soulng/rex/RexLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <soulng/rex/RexLexer.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/rex/RexTokens.hpp>
#include <soulng/rex/RexClassMap.hpp>

using namespace soulng::lexer;
using namespace RexTokens;

RexLexer::RexLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_)
{
}

RexLexer::RexLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_)
{
}

int RexLexer::NextState(int state, char32_t c)
{
    int i = RexClassMap::GetClass(c);
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
                case 2:
                {
                    return 3;
                }
                case 3:
                {
                    return 4;
                }
                case 4:
                {
                    return 5;
                }
                case 5:
                {
                    return 6;
                }
                case 6:
                {
                    return 7;
                }
                case 7:
                {
                    return 8;
                }
                case 8:
                {
                    return 9;
                }
                case 9:
                {
                    return 10;
                }
                case 10:
                {
                    return 11;
                }
                case 11:
                {
                    return 12;
                }
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
                {
                    return 13;
                }
                case 31:
                {
                    return 14;
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
                case 0:
                {
                    return 1;
                }
                case 1:
                {
                    return 2;
                }
                case 2:
                {
                    return 3;
                }
                case 3:
                {
                    return 4;
                }
                case 4:
                {
                    return 5;
                }
                case 5:
                {
                    return 6;
                }
                case 6:
                {
                    return 7;
                }
                case 7:
                {
                    return 8;
                }
                case 8:
                {
                    return 9;
                }
                case 9:
                {
                    return 10;
                }
                case 10:
                {
                    return 11;
                }
                case 11:
                {
                    return 12;
                }
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
                case 0:
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
                case 18:
                case 21:
                case 30:
                case 32:
                case 33:
                case 34:
                case 35:
                {
                    return 15;
                }
                case 14:
                {
                    return 16;
                }
                case 15:
                {
                    return 17;
                }
                case 16:
                {
                    return 18;
                }
                case 17:
                {
                    return 19;
                }
                case 19:
                {
                    return 20;
                }
                case 20:
                {
                    return 21;
                }
                case 22:
                {
                    return 22;
                }
                case 23:
                {
                    return 23;
                }
                case 24:
                {
                    return 24;
                }
                case 25:
                {
                    return 25;
                }
                case 26:
                {
                    return 26;
                }
                case 27:
                {
                    return 27;
                }
                case 28:
                {
                    return 28;
                }
                case 29:
                {
                    return 29;
                }
                case 31:
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
                case 0:
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
                case 18:
                case 21:
                case 30:
                case 32:
                case 33:
                case 34:
                case 35:
                {
                    return 15;
                }
                case 14:
                {
                    return 16;
                }
                case 15:
                {
                    return 17;
                }
                case 16:
                {
                    return 18;
                }
                case 17:
                {
                    return 19;
                }
                case 19:
                {
                    return 20;
                }
                case 20:
                {
                    return 21;
                }
                case 22:
                {
                    return 22;
                }
                case 23:
                {
                    return 23;
                }
                case 24:
                {
                    return 24;
                }
                case 25:
                {
                    return 25;
                }
                case 26:
                {
                    return 26;
                }
                case 27:
                {
                    return 27;
                }
                case 28:
                {
                    return 28;
                }
                case 29:
                {
                    return 29;
                }
                case 31:
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
                case 0:
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
                case 18:
                case 21:
                case 30:
                case 32:
                case 33:
                case 34:
                case 35:
                {
                    return 15;
                }
                case 14:
                {
                    return 16;
                }
                case 15:
                {
                    return 17;
                }
                case 16:
                {
                    return 18;
                }
                case 17:
                {
                    return 19;
                }
                case 19:
                {
                    return 20;
                }
                case 20:
                {
                    return 21;
                }
                case 22:
                {
                    return 22;
                }
                case 23:
                {
                    return 23;
                }
                case 24:
                {
                    return 24;
                }
                case 25:
                {
                    return 25;
                }
                case 26:
                {
                    return 26;
                }
                case 27:
                {
                    return 27;
                }
                case 28:
                {
                    return 28;
                }
                case 29:
                {
                    return 29;
                }
                case 31:
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
                case 14:
                {
                    return 33;
                }
                case 15:
                {
                    return 34;
                }
                case 16:
                {
                    return 35;
                }
                case 19:
                {
                    return 36;
                }
                case 20:
                {
                    return 37;
                }
                case 22:
                {
                    return 38;
                }
                case 23:
                {
                    return 39;
                }
                case 24:
                {
                    return 40;
                }
                case 25:
                {
                    return 41;
                }
                case 26:
                {
                    return 42;
                }
                case 27:
                {
                    return 43;
                }
                case 28:
                {
                    return 44;
                }
                case 29:
                {
                    return 45;
                }
                case 31:
                {
                    return 46;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 46:
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
                case 14:
                {
                    return 33;
                }
                case 15:
                {
                    return 34;
                }
                case 19:
                {
                    return 36;
                }
                case 20:
                {
                    return 37;
                }
                case 22:
                {
                    return 38;
                }
                case 23:
                {
                    return 39;
                }
                case 24:
                {
                    return 40;
                }
                case 25:
                {
                    return 41;
                }
                case 26:
                {
                    return 42;
                }
                case 27:
                {
                    return 43;
                }
                case 28:
                {
                    return 44;
                }
                case 31:
                {
                    return 47;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 47:
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
                case 14:
                {
                    return 33;
                }
                case 15:
                {
                    return 34;
                }
                case 19:
                {
                    return 36;
                }
                case 20:
                {
                    return 37;
                }
                case 22:
                {
                    return 38;
                }
                case 23:
                {
                    return 39;
                }
                case 24:
                {
                    return 40;
                }
                case 25:
                {
                    return 41;
                }
                case 26:
                {
                    return 42;
                }
                case 27:
                {
                    return 43;
                }
                case 31:
                {
                    return 48;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 48:
        {
            switch (i)
            {
                case 14:
                {
                    return 33;
                }
                case 15:
                {
                    return 34;
                }
                case 19:
                {
                    return 36;
                }
                case 20:
                {
                    return 37;
                }
                case 23:
                {
                    return 39;
                }
                case 24:
                {
                    return 40;
                }
                case 25:
                {
                    return 41;
                }
                case 26:
                {
                    return 42;
                }
                case 31:
                {
                    return 49;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 49:
        {
            switch (i)
            {
                case 23:
                {
                    return 39;
                }
                case 24:
                {
                    return 40;
                }
                case 25:
                {
                    return 41;
                }
                case 31:
                {
                    return 50;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 50:
        {
            switch (i)
            {
                case 23:
                {
                    return 39;
                }
                case 24:
                {
                    return 40;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 45:
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
                case 31:
                {
                    return 51;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 51:
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
                case 31:
                {
                    return 52;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 52:
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
                case 31:
                {
                    return 53;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 53:
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
        case 44:
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
                case 31:
                {
                    return 54;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 54:
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
                case 31:
                {
                    return 51;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 43:
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
                case 31:
                {
                    return 55;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 55:
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
                case 31:
                {
                    return 54;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 42:
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
                case 31:
                {
                    return 56;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 56:
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
                case 31:
                {
                    return 55;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 41:
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
                case 31:
                {
                    return 57;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 57:
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
                case 31:
                {
                    return 56;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 40:
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
                case 31:
                {
                    return 58;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 58:
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
                case 31:
                {
                    return 57;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 39:
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
                case 31:
                {
                    return 58;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 38:
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
                case 22:
                {
                    return 38;
                }
                case 31:
                {
                    return 59;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 59:
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
                case 22:
                {
                    return 38;
                }
                case 31:
                {
                    return 60;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 60:
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
                case 31:
                {
                    return 61;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 61:
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
                case 31:
                {
                    return 62;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 62:
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
                case 31:
                {
                    return 53;
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
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 64;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 64:
        {
            switch (i)
            {
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 65;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 65:
        {
            switch (i)
            {
                case 21:
                case 22:
                {
                    return 63;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 63:
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
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 66;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 66:
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
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 67;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 67:
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
                case 31:
                {
                    return 60;
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
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 64;
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
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 68;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 68:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 69;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 69:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 70;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 70:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 71;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 71:
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
                case 31:
                {
                    return 61;
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
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                case 31:
                {
                    return 73;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 73:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                case 31:
                {
                    return 74;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 74:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 72:
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
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                case 31:
                {
                    return 75;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 75:
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
                case 31:
                {
                    return 67;
                }
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
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
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                case 31:
                {
                    return 73;
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
                case 31:
                {
                    return 51;
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
                case 31:
                {
                    return 54;
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
                case 31:
                {
                    return 55;
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
                case 31:
                {
                    return 56;
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
            switch (i)
            {
                case 31:
                {
                    return 57;
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
                case 31:
                {
                    return 58;
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
                case 31:
                {
                    return 58;
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
                case 22:
                {
                    return 38;
                }
                case 31:
                {
                    return 76;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 76:
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
                case 22:
                {
                    return 38;
                }
                case 31:
                {
                    return 60;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 21:
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
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 77;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 77:
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
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 78;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 78:
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
                case 21:
                case 22:
                {
                    return 63;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 20:
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
                case 21:
                case 22:
                {
                    return 63;
                }
                case 31:
                {
                    return 77;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 19:
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
                case 31:
                {
                    return 52;
                }
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 79;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 79:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 80;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 80:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 81;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 81:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 82;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 82:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 83;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 83:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 84;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 84:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 85;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 85:
        {
            switch (i)
            {
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 86;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 86:
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
                case 31:
                {
                    return 62;
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
                case 31:
                {
                    return 52;
                }
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 68;
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
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                case 31:
                {
                    return 87;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 87:
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
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                case 31:
                {
                    return 88;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 88:
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
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 16:
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
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 72;
                }
                case 31:
                {
                    return 87;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 15:
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
                case 31:
                {
                    return 52;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 9:
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
        case 8:
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
        case 7:
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
        case 6:
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
        case 5:
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
        case 4:
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
        case 3:
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
            return -1;
        }
        case 2:
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
            return -1;
        }
    }
    return -1;
}

int RexLexer::GetTokenId(int statementIndex)
{
    switch (statementIndex)
    {
        case 0:
        {
            Retract();
            return LPAREN;
            break;
        }
        case 1:
        {
            Retract();
            return RPAREN;
            break;
        }
        case 2:
        {
            Retract();
            return LBRACKET;
            break;
        }
        case 3:
        {
            Retract();
            return RBRACKET;
            break;
        }
        case 4:
        {
            Retract();
            return ALT;
            break;
        }
        case 5:
        {
            Retract();
            return STAR;
            break;
        }
        case 6:
        {
            Retract();
            return PLUS;
            break;
        }
        case 7:
        {
            Retract();
            return QUEST;
            break;
        }
        case 8:
        {
            Retract();
            return DOT;
            break;
        }
        case 9:
        {
            Retract();
            return ESCAPE;
            break;
        }
        case 10:
        {
            Retract();
            return INVERSE;
            break;
        }
        case 11:
        {
            Retract();
            return MINUS;
            break;
        }
        case 12:
        {
            Retract();
            return CHAR;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
