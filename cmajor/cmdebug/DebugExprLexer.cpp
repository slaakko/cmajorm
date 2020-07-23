
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebugExpr.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugExprLexer.hpp>
#include <cmajor/cmdebug/DebugExprKeywords.hpp>
#include <soulng/lexer/Token.hpp>
#include <cmajor/cmdebug/DebugExprTokens.hpp>
#include <cmajor/cmdebug/DebugExprClassMap.hpp>

using namespace soulng::lexer;
using namespace DebugExprTokens;

DebugExprLexer::DebugExprLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_)
{
    SetKeywordMap(DebugExprKeywords::GetKeywordMap());
}

DebugExprLexer::DebugExprLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_)
{
    SetKeywordMap(DebugExprKeywords::GetKeywordMap());
}

int DebugExprLexer::NextState(int state, char32_t c)
{
    int i = DebugExprClassMap::GetClass(c);
    switch (state)
    {
        case 0:
        {
            switch (i)
            {
                case 0:
                case 4:
                case 5:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 1;
                }
                case 2:
                case 3:
                {
                    return 2;
                }
                case 14:
                case 15:
                {
                    return 3;
                }
                case 16:
                {
                    return 4;
                }
                case 17:
                {
                    return 5;
                }
                case 27:
                {
                    return 6;
                }
                case 28:
                {
                    return 7;
                }
                case 29:
                {
                    return 8;
                }
                case 30:
                {
                    return 9;
                }
                case 31:
                {
                    return 10;
                }
                case 32:
                {
                    return 11;
                }
                case 33:
                {
                    return 12;
                }
                case 34:
                {
                    return 13;
                }
                case 35:
                {
                    return 14;
                }
                case 36:
                {
                    return 15;
                }
                case 37:
                {
                    return 16;
                }
                case 38:
                {
                    return 17;
                }
                case 39:
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
        case 17:
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
            return -1;
        }
        case 16:
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
        case 15:
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
        case 14:
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
            return -1;
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
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
                case 4:
                {
                    return 22;
                }
                case 5:
                {
                    return 23;
                }
                case 9:
                {
                    return 24;
                }
                case 10:
                {
                    return 25;
                }
                case 12:
                {
                    return 26;
                }
                case 13:
                {
                    return 27;
                }
                case 15:
                case 16:
                {
                    return 28;
                }
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
                {
                    return 21;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 28:
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
                {
                    return 21;
                }
                case 15:
                case 16:
                {
                    return 28;
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
                case 14:
                case 15:
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
                {
                    return 21;
                }
                case 14:
                case 15:
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
        case 26:
        {
            switch (i)
            {
                case 14:
                case 15:
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
        case 25:
        {
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
                {
                    return 21;
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 39;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 39:
        {
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 40;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 40:
        {
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 41;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 41:
        {
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 42;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 42:
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
                {
                    return 21;
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 43;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 43:
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
                {
                    return 21;
                }
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 43;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 22:
        {
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 43;
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
        case 19:
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                {
                    return 19;
                }
                case 17:
                {
                    return 20;
                }
                case 18:
                {
                    return 21;
                }
                default:
                {
                    return -1;
                }
            }
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
            switch (i)
            {
                case 14:
                case 15:
                case 16:
                {
                    return 3;
                }
                case 4:
                {
                    return 44;
                }
                case 5:
                {
                    return 45;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 45:
        {
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
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
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 46;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 44:
        {
            switch (i)
            {
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                {
                    return 46;
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
            switch (i)
            {
                case 14:
                case 15:
                case 16:
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
                {
                    return 2;
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
                case 0:
                case 1:
                case 4:
                case 5:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
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
                case 0:
                case 1:
                case 4:
                case 5:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 47;
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

int DebugExprLexer::GetTokenId(int statementIndex)
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
            int kw = GetKeywordToken(token.match);
            if (kw == INVALID_TOKEN) return ID;
            else return kw;
            break;
        }
        case 2:
        {
            Retract();
            return ADDRESS;
            break;
        }
        case 3:
        {
            Retract();
            return INTEGER;
            break;
        }
        case 4:
        {
            Retract();
            return STRING;
            break;
        }
        case 5:
        {
            Retract();
            return AST;
            break;
        }
        case 6:
        {
            Retract();
            return AMP;
            break;
        }
        case 7:
        {
            Retract();
            return DOLLAR;
            break;
        }
        case 8:
        {
            Retract();
            return LPAREN;
            break;
        }
        case 9:
        {
            Retract();
            return RPAREN;
            break;
        }
        case 10:
        {
            Retract();
            return LANGLE;
            break;
        }
        case 11:
        {
            Retract();
            return RANGLE;
            break;
        }
        case 12:
        {
            Retract();
            return LBRACKET;
            break;
        }
        case 13:
        {
            Retract();
            return RBRACKET;
            break;
        }
        case 14:
        {
            Retract();
            return DOT;
            break;
        }
        case 15:
        {
            Retract();
            return COMMA;
            break;
        }
        case 16:
        {
            Retract();
            return PLUS;
            break;
        }
        case 17:
        {
            Retract();
            return MINUS;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
