
// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/assembler/AssemblyLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/assembler/AssemblyLexer.hpp>
#include <soulng/lexer/Token.hpp>
#include <system-x/assembler/AssemblyTokens.hpp>
#include <system-x/assembler/AssemblyClassMap.hpp>

using namespace soulng::lexer;
using namespace AssemblyTokens;

AssemblyLexer::AssemblyLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_)
{
}

AssemblyLexer::AssemblyLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_)
{
}

int AssemblyLexer::NextState(int state, char32_t c)
{
    int i = AssemblyClassMap::GetClass(c);
    switch (state)
    {
        case 0:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(28);
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
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                {
                    return 5;
                }
                case 5:
                case 9:
                {
                    return 6;
                }
                case 6:
                case 7:
                {
                    return 7;
                }
                case 8:
                {
                    return 8;
                }
                case 10:
                {
                    return 9;
                }
                case 14:
                {
                    return 10;
                }
                case 15:
                {
                    return 11;
                }
                case 19:
                {
                    return 12;
                }
                case 20:
                {
                    return 13;
                }
                case 21:
                {
                    return 14;
                }
                case 22:
                {
                    return 15;
                }
                case 23:
                {
                    return 16;
                }
                case 24:
                {
                    return 17;
                }
                case 25:
                {
                    return 18;
                }
                case 26:
                {
                    return 19;
                }
                case 27:
                {
                    return 20;
                }
                case 28:
                {
                    return 21;
                }
                case 29:
                {
                    return 22;
                }
                case 30:
                {
                    return 23;
                }
                case 31:
                {
                    return 24;
                }
                case 32:
                {
                    return 25;
                }
                case 33:
                {
                    return 26;
                }
                case 34:
                {
                    return 27;
                }
                case 35:
                {
                    return 28;
                }
                case 36:
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
        case 28:
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
        case 27:
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
        case 26:
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
        case 25:
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
        case 24:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(28);
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
        case 23:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(28);
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
                case 30:
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
        case 22:
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
        case 21:
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
            switch (i)
            {
                case 28:
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
        case 17:
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
        case 16:
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
            switch (i)
            {
                case 4:
                case 5:
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 33;
                }
                case 20:
                {
                    return 34;
                }
                case 22:
                {
                    return 35;
                }
                case 23:
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
            switch (i)
            {
                case 4:
                case 5:
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 33;
                }
                case 20:
                {
                    return 34;
                }
                case 22:
                {
                    return 35;
                }
                case 23:
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
            switch (i)
            {
                case 4:
                case 5:
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 33;
                }
                case 20:
                {
                    return 34;
                }
                case 22:
                {
                    return 35;
                }
                case 23:
                {
                    return 36;
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
            switch (i)
            {
                case 4:
                case 5:
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 33;
                }
                case 20:
                {
                    return 34;
                }
                case 22:
                {
                    return 35;
                }
                case 23:
                {
                    return 36;
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
            switch (i)
            {
                case 4:
                case 5:
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 33;
                }
                case 20:
                {
                    return 34;
                }
                case 22:
                {
                    return 35;
                }
                case 23:
                {
                    return 36;
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
            switch (i)
            {
                case 4:
                case 5:
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 33;
                }
                case 20:
                {
                    return 34;
                }
                case 22:
                {
                    return 35;
                }
                case 23:
                {
                    return 36;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 14:
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
        case 13:
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
            switch (i)
            {
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
        case 12:
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
        case 11:
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
        case 10:
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
        case 9:
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
            switch (i)
            {
                case 17:
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
                case 12:
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
                case 13:
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
                case 18:
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
                case 14:
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
                case 15:
                {
                    return 45;
                }
                case 16:
                case 17:
                case 18:
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
            switch (i)
            {
                case 15:
                {
                    return 45;
                }
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 46;
                }
                case 19:
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
        case 45:
        {
            switch (i)
            {
                case 15:
                {
                    return 45;
                }
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 46;
                }
                case 19:
                {
                    return 47;
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
            int tokenId = GetTokenId(28);
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
                case 17:
                case 18:
                case 21:
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
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 48;
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
                case 6:
                case 7:
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
                case 6:
                case 7:
                {
                    return 49;
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
            int tokenId = GetTokenId(28);
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
            switch (i)
            {
                case 4:
                case 5:
                case 11:
                case 12:
                case 13:
                case 16:
                case 17:
                case 18:
                case 21:
                {
                    return 33;
                }
                case 20:
                {
                    return 34;
                }
                case 22:
                {
                    return 35;
                }
                case 23:
                {
                    return 36;
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
            int tokenId = GetTokenId(28);
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
                case 36:
                {
                    return 50;
                }
                case 3:
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
        case 50:
        {
            switch (i)
            {
                case 3:
                {
                    return 51;
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
            int tokenId = GetTokenId(28);
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
                {
                    return 52;
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
                case 36:
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
            switch (i)
            {
                case 2:
                {
                    return 52;
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
                case 36:
                {
                    return 53;
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
                case 1:
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
    }
    return -1;
}

int AssemblyLexer::GetTokenId(int statementIndex)
{
    switch (statementIndex)
    {
        case 0:
        {
            Retract();
            return SPACE;
            break;
        }
        case 1:
        {
            Retract();
            return NEWLINE;
            break;
        }
        case 2:
        {
            Retract();
            return DECIMAL_CONSTANT;
            break;
        }
        case 3:
        {
            Retract();
            return HEX_CONSTANT;
            break;
        }
        case 4:
        {
            Retract();
            return CHAR_CONSTANT;
            break;
        }
        case 5:
        {
            Retract();
            return STRING_CONSTANT;
            break;
        }
        case 6:
        {
            Retract();
            return CLSID_CONSTANT;
            break;
        }
        case 7:
        {
            Retract();
            return LOCAL_SYMBOL;
            break;
        }
        case 8:
        {
            Retract();
            return SYMBOL;
            break;
        }
        case 9:
        {
            Retract();
            return AT;
            break;
        }
        case 10:
        {
            Retract();
            return PLUS;
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
            return TILDE;
            break;
        }
        case 13:
        {
            Retract();
            return DOLLAR;
            break;
        }
        case 14:
        {
            Retract();
            return AMP;
            break;
        }
        case 15:
        {
            Retract();
            return AST;
            break;
        }
        case 16:
        {
            Retract();
            return SLASHSLASH;
            break;
        }
        case 17:
        {
            Retract();
            return SLASH;
            break;
        }
        case 18:
        {
            Retract();
            return PERCENT;
            break;
        }
        case 19:
        {
            Retract();
            return SHIFT_LEFT;
            break;
        }
        case 20:
        {
            Retract();
            return SHIFT_RIGHT;
            break;
        }
        case 21:
        {
            Retract();
            return BAR;
            break;
        }
        case 22:
        {
            Retract();
            return CARET;
            break;
        }
        case 23:
        {
            Retract();
            return DOT;
            break;
        }
        case 24:
        {
            Retract();
            return LPAREN;
            break;
        }
        case 25:
        {
            Retract();
            return RPAREN;
            break;
        }
        case 26:
        {
            Retract();
            return SEMICOLON;
            break;
        }
        case 27:
        {
            Retract();
            return COMMA;
            break;
        }
        case 28:
        {
            Retract();
            return CHAR;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
