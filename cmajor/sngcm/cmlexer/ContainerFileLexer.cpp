
// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/ContainerFileLexer.hpp>
#include <sngcm/cmlexer/ContainerFileKeywords.hpp>
#include <soulng/lexer/Token.hpp>
#include <sngcm/cmlexer/ContainerFileTokens.hpp>
#include <sngcm/cmlexer/ContainerFileClassMap.hpp>

using namespace soulng::lexer;
using namespace ContainerFileTokens;

ContainerFileLexer::ContainerFileLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_),
    filePath()
{
    SetKeywordMap(ContainerFileKeywords::GetKeywordMap());
}

ContainerFileLexer::ContainerFileLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_),
    filePath()
{
    SetKeywordMap(ContainerFileKeywords::GetKeywordMap());
}

int ContainerFileLexer::NextState(int state, char32_t c)
{
    int i = ContainerFileClassMap::GetClass(c);
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
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 2;
                }
                case 4:
                {
                    return 3;
                }
                case 9:
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
        case 6:
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
        case 5:
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
        case 4:
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
                case 7:
                case 8:
                case 9:
                case 11:
                case 12:
                case 13:
                {
                    return 8;
                }
                case 10:
                {
                    return 9;
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
        case 8:
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
                case 7:
                case 8:
                case 9:
                case 11:
                case 12:
                case 13:
                {
                    return 8;
                }
                case 10:
                {
                    return 9;
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
                case 4:
                case 5:
                {
                    return 10;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 10:
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
                case 4:
                case 5:
                {
                    return 10;
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
                case 0:
                {
                    return 1;
                }
                case 2:
                case 3:
                case 6:
                case 7:
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
            switch (i)
            {
                case 0:
                {
                    return 11;
                }
                case 1:
                {
                    return 12;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 12:
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
                {
                    return 13;
                }
                case 1:
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
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 15;
                }
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
                {
                    return 16;
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
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 2:
                case 3:
                case 6:
                case 7:
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 18;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 17:
        {
            switch (i)
            {
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
                {
                    return 13;
                }
                case 0:
                {
                    return 19;
                }
                case 1:
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
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 15;
                }
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
                {
                    return 16;
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
                case 0:
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
                {
                    return 21;
                }
                case 1:
                {
                    return 22;
                }
                case 2:
                {
                    return 23;
                }
                case 3:
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 3:
                case 6:
                case 7:
                {
                    return 18;
                }
                case 2:
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 18;
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 18;
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
                case 1:
                {
                    return 22;
                }
                case 0:
                {
                    return 26;
                }
                case 2:
                {
                    return 27;
                }
                case 3:
                {
                    return 28;
                }
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
                {
                    return 21;
                }
                case 1:
                {
                    return 22;
                }
                case 2:
                {
                    return 23;
                }
                case 3:
                {
                    return 24;
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 3:
                case 6:
                case 7:
                {
                    return 18;
                }
                case 2:
                {
                    return 25;
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 18;
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 21;
                }
                case 1:
                {
                    return 22;
                }
                case 0:
                {
                    return 30;
                }
                case 2:
                {
                    return 31;
                }
                case 3:
                {
                    return 32;
                }
                case 6:
                case 7:
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 21;
                }
                case 1:
                {
                    return 22;
                }
                case 0:
                {
                    return 30;
                }
                case 2:
                {
                    return 31;
                }
                case 3:
                {
                    return 32;
                }
                case 6:
                case 7:
                {
                    return 33;
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 3:
                case 6:
                case 7:
                {
                    return 18;
                }
                case 2:
                {
                    return 25;
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
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                case 0:
                {
                    return 17;
                }
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 18;
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
                {
                    return 19;
                }
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
                {
                    return 21;
                }
                case 2:
                {
                    return 23;
                }
                case 3:
                {
                    return 24;
                }
                case 1:
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
                case 1:
                {
                    return 22;
                }
                case 0:
                {
                    return 26;
                }
                case 2:
                {
                    return 27;
                }
                case 3:
                {
                    return 28;
                }
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
                {
                    return 29;
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
                case 0:
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
                {
                    return 21;
                }
                case 1:
                {
                    return 22;
                }
                case 2:
                {
                    return 23;
                }
                case 3:
                {
                    return 24;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 13:
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
                {
                    return 13;
                }
                case 1:
                {
                    return 14;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 11:
        {
            switch (i)
            {
                case 0:
                case 1:
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
                {
                    return 35;
                }
                case 2:
                {
                    return 36;
                }
                case 3:
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
                {
                    return 1;
                }
                case 3:
                case 6:
                case 7:
                {
                    return 2;
                }
                case 2:
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
                {
                    return 1;
                }
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 2;
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
                {
                    return 1;
                }
                case 2:
                case 3:
                case 6:
                case 7:
                {
                    return 2;
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
                case 0:
                case 1:
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
                {
                    return 35;
                }
                case 2:
                {
                    return 36;
                }
                case 3:
                {
                    return 37;
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

int ContainerFileLexer::GetTokenId(int statementIndex)
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
            filePath = MakeFilePath(token.match);
            return FILEPATH;
            break;
        }
        case 3:
        {
            Retract();
            return ASSIGN;
            break;
        }
        case 4:
        {
            Retract();
            return SEMICOLON;
            break;
        }
        case 5:
        {
            Retract();
            return DOT;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
