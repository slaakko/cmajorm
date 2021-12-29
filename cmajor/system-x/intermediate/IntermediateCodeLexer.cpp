
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/intermediate/IntermediateCodeLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/intermediate/IntermediateCodeLexer.hpp>
#include <system-x/intermediate/IntermediateCodeKeywords.hpp>
#include <soulng/lexer/Token.hpp>
#include <system-x/intermediate/IntermediateCodeTokens.hpp>
#include <system-x/intermediate/IntermediateCodeClassMap.hpp>

using namespace soulng::lexer;
using namespace IntermediateCodeTokens;

IntermediateCodeLexer::IntermediateCodeLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_)
{
    SetKeywordMap(IntermediateCodeKeywords::GetKeywordMap());
}

IntermediateCodeLexer::IntermediateCodeLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_)
{
    SetKeywordMap(IntermediateCodeKeywords::GetKeywordMap());
}

int IntermediateCodeLexer::NextState(int state, char32_t c)
{
    int i = IntermediateCodeClassMap::GetClass(c);
    switch (state)
    {
        case 0:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 5:
                case 6:
                {
                    return 1;
                }
                case 2:
                {
                    return 2;
                }
                case 3:
                case 7:
                case 8:
                case 9:
                case 15:
                {
                    return 3;
                }
                case 10:
                {
                    return 4;
                }
                case 11:
                {
                    return 5;
                }
                case 12:
                {
                    return 6;
                }
                case 13:
                {
                    return 7;
                }
                case 14:
                {
                    return 8;
                }
                case 17:
                {
                    return 9;
                }
                case 18:
                case 19:
                {
                    return 10;
                }
                case 20:
                case 22:
                case 23:
                case 24:
                {
                    return 11;
                }
                case 21:
                {
                    return 12;
                }
                case 25:
                {
                    return 13;
                }
                case 26:
                {
                    return 14;
                }
                case 27:
                {
                    return 15;
                }
                case 28:
                {
                    return 16;
                }
                case 29:
                {
                    return 17;
                }
                case 30:
                {
                    return 18;
                }
                case 31:
                {
                    return 19;
                }
                case 32:
                {
                    return 20;
                }
                case 33:
                {
                    return 21;
                }
                case 34:
                {
                    return 22;
                }
                case 35:
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
            return -1;
        }
        case 22:
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
        case 21:
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
        case 20:
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
        case 19:
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
        case 18:
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
        case 17:
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
        case 16:
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
        case 15:
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
        case 14:
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
        case 13:
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
            switch (i)
            {
                case 25:
                {
                    return 13;
                }
                case 17:
                {
                    return 24;
                }
                case 20:
                case 21:
                case 22:
                {
                    return 25;
                }
                case 23:
                case 24:
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
            switch (i)
            {
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                {
                    return 25;
                }
                case 18:
                case 19:
                {
                    return 27;
                }
                case 25:
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
            switch (i)
            {
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                {
                    return 25;
                }
                case 25:
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
                case 25:
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
            switch (i)
            {
                case 25:
                {
                    return 29;
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
            switch (i)
            {
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
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
        case 24:
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
            switch (i)
            {
                case 23:
                case 24:
                {
                    return 30;
                }
                case 25:
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
            switch (i)
            {
                case 23:
                case 24:
                {
                    return 30;
                }
                case 25:
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
            switch (i)
            {
                case 18:
                case 19:
                {
                    return 27;
                }
                case 25:
                {
                    return 29;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 12:
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
                case 3:
                case 4:
                case 8:
                case 9:
                case 15:
                {
                    return 32;
                }
                case 7:
                {
                    return 33;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 36;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 35:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
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
                case 3:
                case 4:
                case 7:
                case 9:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                case 8:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                case 9:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                case 20:
                case 21:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                case 22:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                case 10:
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
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
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
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 41;
                }
                case 11:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 11:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 36;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 10:
        {
            switch (i)
            {
                case 17:
                {
                    return 9;
                }
                case 25:
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
            switch (i)
            {
                case 17:
                {
                    return 24;
                }
                case 23:
                case 24:
                {
                    return 30;
                }
                case 25:
                {
                    return 43;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 9:
        {
            switch (i)
            {
                case 25:
                {
                    return 31;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 8:
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
            switch (i)
            {
                case 15:
                {
                    return 44;
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
                case 25:
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
                case 25:
                {
                    return 45;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 7:
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
            return -1;
        }
        case 6:
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
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 5:
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
        case 4:
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
            switch (i)
            {
                case 3:
                case 4:
                case 7:
                case 8:
                case 9:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                {
                    return 32;
                }
                case 12:
                {
                    return 34;
                }
                case 13:
                {
                    return 35;
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
                case 2:
                {
                    return 46;
                }
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
                case 34:
                case 35:
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
            switch (i)
            {
                case 2:
                {
                    return 46;
                }
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
                case 34:
                case 35:
                {
                    return 47;
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
                case 0:
                case 1:
                case 5:
                case 6:
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

int IntermediateCodeLexer::GetTokenId(int statementIndex)
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
            return CLSID;
            break;
        }
        case 2:
        {
            Retract();
            int kw = GetKeywordToken(token.match);
            if (kw == INVALID_TOKEN) return ID;
            else return kw;
            break;
        }
        case 3:
        {
            Retract();
            return TYPEID;
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
            return NUMBER;
            break;
        }
        case 6:
        {
            Retract();
            return HEXNUM;
            break;
        }
        case 7:
        {
            Retract();
            return LPAREN;
            break;
        }
        case 8:
        {
            Retract();
            return RPAREN;
            break;
        }
        case 9:
        {
            Retract();
            return COMMA;
            break;
        }
        case 10:
        {
            Retract();
            return LBRACE;
            break;
        }
        case 11:
        {
            Retract();
            return RBRACE;
            break;
        }
        case 12:
        {
            Retract();
            return AST;
            break;
        }
        case 13:
        {
            Retract();
            return COLON;
            break;
        }
        case 14:
        {
            Retract();
            return SEMICOLON;
            break;
        }
        case 15:
        {
            Retract();
            return LBRACKET;
            break;
        }
        case 16:
        {
            Retract();
            return RBRACKET;
            break;
        }
        case 17:
        {
            Retract();
            return ASSIGN;
            break;
        }
        case 18:
        {
            Retract();
            return AT;
            break;
        }
        case 19:
        {
            Retract();
            return DOLLAR;
            break;
        }
        case 20:
        {
            Retract();
            return EXCLAMATION;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
