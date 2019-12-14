
// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/CmajorLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmlexer/CmajorKeywords.hpp>
#include <soulng/lexer/Token.hpp>
#include <sngcm/cmlexer/CmajorTokens.hpp>
#include <sngcm/cmlexer/CmajorClassMap.hpp>

using namespace soulng::lexer;
using namespace CmajorTokens;

CmajorLexer::CmajorLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_),
    floatingLit(), floatingLitFloat(), intLit(), intLitUnsigned(), chrLit(), chrLitPrefix(), strLit(), strLitPrefix(), leftAngleCount(), parsingOperatorFunctionGroupId()
{
    SetKeywordMap(CmajorKeywords::GetKeywordMap());
}

CmajorLexer::CmajorLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_),
    floatingLit(), floatingLitFloat(), intLit(), intLitUnsigned(), chrLit(), chrLitPrefix(), strLit(), strLitPrefix(), leftAngleCount(), parsingOperatorFunctionGroupId()
{
    SetKeywordMap(CmajorKeywords::GetKeywordMap());
}

int CmajorLexer::NextState(int state, char32_t c)
{
    int i = CmajorClassMap::GetClass(c);
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
                case 2:
                case 9:
                case 10:
                {
                    return 2;
                }
                case 4:
                {
                    return 3;
                }
                case 5:
                {
                    return 4;
                }
                case 6:
                {
                    return 5;
                }
                case 7:
                case 12:
                case 13:
                case 15:
                case 20:
                case 21:
                case 22:
                case 23:
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
                    return 6;
                }
                case 14:
                {
                    return 7;
                }
                case 16:
                {
                    return 8;
                }
                case 17:
                {
                    return 9;
                }
                case 18:
                {
                    return 10;
                }
                case 19:
                {
                    return 11;
                }
                case 24:
                {
                    return 12;
                }
                case 34:
                case 36:
                {
                    return 13;
                }
                case 35:
                {
                    return 14;
                }
                case 37:
                {
                    return 15;
                }
                case 38:
                {
                    return 16;
                }
                case 39:
                {
                    return 17;
                }
                case 40:
                {
                    return 18;
                }
                case 41:
                {
                    return 19;
                }
                case 42:
                {
                    return 20;
                }
                case 43:
                {
                    return 21;
                }
                case 44:
                {
                    return 22;
                }
                case 45:
                {
                    return 23;
                }
                case 46:
                {
                    return 24;
                }
                case 47:
                {
                    return 25;
                }
                case 48:
                {
                    return 26;
                }
                case 49:
                {
                    return 27;
                }
                case 50:
                {
                    return 28;
                }
                case 51:
                {
                    return 29;
                }
                case 52:
                {
                    return 30;
                }
                case 53:
                {
                    return 31;
                }
                case 54:
                {
                    return 32;
                }
                case 55:
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
            int tokenId = GetTokenId(42);
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
            int tokenId = GetTokenId(41);
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
        case 31:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(40);
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
        case 30:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(38);
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
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(37);
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
            int tokenId = GetTokenId(36);
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
            int tokenId = GetTokenId(35);
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
            int tokenId = GetTokenId(34);
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
            int tokenId = GetTokenId(33);
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
            int tokenId = GetTokenId(32);
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
            int tokenId = GetTokenId(29);
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
        case 21:
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
                case 38:
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
        case 20:
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
        case 19:
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
            switch (i)
            {
                case 41:
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
        case 18:
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
                case 40:
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
            return -1;
        }
        case 17:
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
                case 38:
                {
                    return 37;
                }
                case 39:
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
        case 37:
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
            int tokenId = GetTokenId(39);
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
                case 38:
                {
                    return 39;
                }
                case 39:
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
        case 39:
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
        case 15:
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
                case 37:
                {
                    return 41;
                }
                case 38:
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
            switch (i)
            {
                case 39:
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
        case 41:
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
                case 12:
                {
                    return 44;
                }
                case 13:
                {
                    return 45;
                }
                case 14:
                {
                    return 46;
                }
                case 15:
                {
                    return 47;
                }
                case 16:
                {
                    return 48;
                }
                case 26:
                {
                    return 49;
                }
                case 27:
                {
                    return 50;
                }
                case 34:
                {
                    return 51;
                }
                case 35:
                case 36:
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
                case 14:
                {
                    return 46;
                }
                case 15:
                {
                    return 47;
                }
                case 16:
                {
                    return 48;
                }
                case 26:
                {
                    return 49;
                }
                case 27:
                {
                    return 50;
                }
                case 34:
                {
                    return 51;
                }
                case 35:
                case 36:
                {
                    return 52;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 51:
        {
            switch (i)
            {
                case 16:
                {
                    return 48;
                }
                case 26:
                {
                    return 49;
                }
                case 27:
                {
                    return 50;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 51;
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
                case 17:
                {
                    return 53;
                }
                case 18:
                {
                    return 54;
                }
                case 34:
                case 35:
                case 36:
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
                case 34:
                case 35:
                case 36:
                {
                    return 55;
                }
                case 28:
                {
                    return 56;
                }
                case 29:
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
        case 56:
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
        case 54:
        {
            switch (i)
            {
                case 34:
                case 35:
                case 36:
                {
                    return 55;
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
                case 34:
                case 35:
                case 36:
                {
                    return 55;
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
                case 17:
                {
                    return 53;
                }
                case 18:
                {
                    return 54;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 55;
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
                case 28:
                {
                    return 56;
                }
                case 29:
                {
                    return 57;
                }
                case 26:
                {
                    return 58;
                }
                case 27:
                {
                    return 59;
                }
                case 34:
                case 35:
                case 36:
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
                case 28:
                {
                    return 56;
                }
                case 29:
                {
                    return 57;
                }
                case 26:
                {
                    return 58;
                }
                case 27:
                {
                    return 59;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 60;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 59:
        {
            switch (i)
            {
                case 17:
                {
                    return 61;
                }
                case 18:
                {
                    return 62;
                }
                case 34:
                case 35:
                case 36:
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
                case 28:
                {
                    return 56;
                }
                case 29:
                {
                    return 57;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 63;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 62:
        {
            switch (i)
            {
                case 34:
                case 35:
                case 36:
                {
                    return 63;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 61:
        {
            switch (i)
            {
                case 34:
                case 35:
                case 36:
                {
                    return 63;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 58:
        {
            switch (i)
            {
                case 17:
                {
                    return 61;
                }
                case 18:
                {
                    return 62;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 63;
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
            return -1;
        }
        case 45:
        {
            switch (i)
            {
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
                case 14:
                {
                    return 46;
                }
                case 15:
                {
                    return 47;
                }
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
                    return 64;
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
                    return 64;
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
                case 14:
                {
                    return 46;
                }
                case 15:
                {
                    return 47;
                }
                case 16:
                {
                    return 48;
                }
                case 26:
                {
                    return 49;
                }
                case 27:
                {
                    return 50;
                }
                case 34:
                case 35:
                case 36:
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
                case 14:
                {
                    return 46;
                }
                case 15:
                {
                    return 47;
                }
                case 16:
                {
                    return 48;
                }
                case 26:
                {
                    return 49;
                }
                case 27:
                {
                    return 50;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 65;
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
            switch (i)
            {
                case 0:
                {
                    return 67;
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
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
                case 0:
                {
                    return 67;
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
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 68;
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
        case 11:
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
                {
                    return 1;
                }
                case 4:
                {
                    return 3;
                }
                case 24:
                {
                    return 12;
                }
                case 7:
                case 8:
                case 12:
                case 13:
                case 14:
                case 15:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
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
                case 7:
                case 8:
                case 12:
                case 13:
                case 14:
                case 15:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
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
                    return 69;
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
            switch (i)
            {
                case 18:
                {
                    return 70;
                }
                case 39:
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
            int tokenId = GetTokenId(31);
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
        case 70:
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
        case 9:
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
            switch (i)
            {
                case 17:
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
        case 8:
        {
            Lexeme prevMatch = token.match;
            token.match = lexeme;
            int tokenId = GetTokenId(30);
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
                case 34:
                case 35:
                case 36:
                {
                    return 60;
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
                {
                    return 1;
                }
                case 4:
                {
                    return 3;
                }
                case 24:
                {
                    return 12;
                }
                case 7:
                case 8:
                case 12:
                case 13:
                case 14:
                case 15:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
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
                    return 69;
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
                case 7:
                case 8:
                case 12:
                case 13:
                case 14:
                case 15:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
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
                    return 69;
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
        case 4:
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
            switch (i)
            {
                case 5:
                {
                    return 73;
                }
                case 6:
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
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
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
            switch (i)
            {
                case 6:
                {
                    return 76;
                }
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 77;
                }
                case 5:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
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
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 5:
                {
                    return 81;
                }
                case 6:
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
                case 6:
                {
                    return 76;
                }
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 77;
                }
                case 5:
                {
                    return 78;
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
                case 0:
                case 3:
                case 4:
                case 5:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 83;
                }
                case 1:
                {
                    return 84;
                }
                case 2:
                {
                    return 85;
                }
                case 6:
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
            switch (i)
            {
                case 6:
                {
                    return 86;
                }
                case 0:
                case 3:
                case 4:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 87;
                }
                case 1:
                {
                    return 88;
                }
                case 2:
                {
                    return 89;
                }
                case 5:
                {
                    return 90;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 90:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 83;
                }
                case 6:
                {
                    return 86;
                }
                case 1:
                {
                    return 91;
                }
                case 2:
                {
                    return 92;
                }
                case 5:
                {
                    return 93;
                }
                case 9:
                case 10:
                {
                    return 94;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 94:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 83;
                }
                case 6:
                {
                    return 86;
                }
                case 1:
                {
                    return 91;
                }
                case 2:
                {
                    return 92;
                }
                case 5:
                {
                    return 93;
                }
                case 9:
                case 10:
                {
                    return 94;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 93:
        {
            switch (i)
            {
                case 5:
                {
                    return 81;
                }
                case 0:
                case 3:
                case 4:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 83;
                }
                case 1:
                {
                    return 84;
                }
                case 2:
                {
                    return 85;
                }
                case 6:
                {
                    return 95;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 95:
        {
            switch (i)
            {
                case 6:
                {
                    return 86;
                }
                case 0:
                case 3:
                case 4:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 87;
                }
                case 1:
                {
                    return 88;
                }
                case 2:
                {
                    return 89;
                }
                case 5:
                {
                    return 90;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 92:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
                }
                case 1:
                {
                    return 96;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 96:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 91:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 89:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
                }
                case 1:
                {
                    return 96;
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 87:
        {
            switch (i)
            {
                case 0:
                case 3:
                case 4:
                case 5:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 83;
                }
                case 1:
                {
                    return 84;
                }
                case 2:
                {
                    return 85;
                }
                case 6:
                {
                    return 86;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 85:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
                }
                case 1:
                {
                    return 96;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 84:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
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
                case 0:
                case 3:
                case 4:
                case 5:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 83;
                }
                case 1:
                {
                    return 84;
                }
                case 2:
                {
                    return 85;
                }
                case 6:
                {
                    return 86;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 79:
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
                case 3:
                case 4:
                case 7:
                case 8:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 79;
                }
                case 5:
                {
                    return 80;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 77:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 75:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 75;
                }
                case 6:
                {
                    return 76;
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
                case 0:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 97;
                }
                case 1:
                {
                    return 98;
                }
                case 2:
                {
                    return 99;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 99:
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
                case 9:
                case 10:
                {
                    return 2;
                }
                case 1:
                {
                    return 100;
                }
                case 5:
                {
                    return 101;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 101:
        {
            switch (i)
            {
                case 5:
                {
                    return 73;
                }
                case 6:
                {
                    return 74;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 100:
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
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 2;
                }
                case 5:
                {
                    return 101;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 98:
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
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 2;
                }
                case 5:
                {
                    return 101;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 97:
        {
            switch (i)
            {
                case 0:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 97;
                }
                case 1:
                {
                    return 98;
                }
                case 2:
                {
                    return 99;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 3:
        {
            switch (i)
            {
                case 0:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 102;
                }
                case 3:
                {
                    return 103;
                }
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 104:
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
        case 103:
        {
            switch (i)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 16:
                case 17:
                case 18:
                case 19:
                case 24:
                case 25:
                case 26:
                case 27:
                case 29:
                case 34:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 105;
                }
                case 4:
                {
                    return 106;
                }
                case 12:
                {
                    return 107;
                }
                case 13:
                {
                    return 108;
                }
                case 14:
                {
                    return 109;
                }
                case 15:
                {
                    return 110;
                }
                case 20:
                {
                    return 111;
                }
                case 21:
                {
                    return 112;
                }
                case 22:
                {
                    return 113;
                }
                case 23:
                {
                    return 114;
                }
                case 28:
                {
                    return 115;
                }
                case 30:
                {
                    return 116;
                }
                case 31:
                {
                    return 117;
                }
                case 32:
                {
                    return 118;
                }
                case 33:
                {
                    return 119;
                }
                case 35:
                case 36:
                {
                    return 120;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 120:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                case 35:
                case 36:
                {
                    return 121;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 121:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                case 35:
                case 36:
                {
                    return 121;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 119:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 118:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 117:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 122;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 122:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 122;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 116:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 122;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 115:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 114:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 113:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 112:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 111:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 110:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
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
                    return 123;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 123:
        {
            switch (i)
            {
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
                    return 124;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 124:
        {
            switch (i)
            {
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
                    return 125;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 125:
        {
            switch (i)
            {
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
                    return 126;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 126:
        {
            switch (i)
            {
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
                    return 127;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 127:
        {
            switch (i)
            {
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
                    return 128;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 128:
        {
            switch (i)
            {
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
                    return 129;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 129:
        {
            switch (i)
            {
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
                    return 130;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 130:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 109:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
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
                    return 131;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 131:
        {
            switch (i)
            {
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
                    return 132;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 132:
        {
            switch (i)
            {
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
                    return 133;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 133:
        {
            switch (i)
            {
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
                    return 134;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 134:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 108:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
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
                    return 135;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 135:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
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
                    return 135;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 107:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
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
                    return 135;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 106:
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
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 105:
        {
            switch (i)
            {
                case 4:
                {
                    return 104;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 102:
        {
            switch (i)
            {
                case 0:
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 102;
                }
                case 4:
                {
                    return 104;
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
                case 1:
                case 2:
                case 9:
                case 10:
                {
                    return 2;
                }
                case 5:
                {
                    return 101;
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
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 138:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 137:
        {
            switch (i)
            {
                case 3:
                {
                    return 137;
                }
                case 0:
                {
                    return 139;
                }
                case 1:
                case 2:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 16:
                case 17:
                case 18:
                case 19:
                case 24:
                case 25:
                case 26:
                case 27:
                case 29:
                case 34:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 140;
                }
                case 12:
                {
                    return 141;
                }
                case 13:
                {
                    return 142;
                }
                case 14:
                {
                    return 143;
                }
                case 15:
                {
                    return 144;
                }
                case 20:
                {
                    return 145;
                }
                case 21:
                {
                    return 146;
                }
                case 22:
                {
                    return 147;
                }
                case 23:
                {
                    return 148;
                }
                case 28:
                {
                    return 149;
                }
                case 30:
                {
                    return 150;
                }
                case 31:
                {
                    return 151;
                }
                case 32:
                {
                    return 152;
                }
                case 33:
                {
                    return 153;
                }
                case 35:
                case 36:
                {
                    return 154;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 154:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                case 35:
                case 36:
                {
                    return 155;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 155:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                case 35:
                case 36:
                {
                    return 155;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 153:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 152:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 151:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 156;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 156:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 156;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 150:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                case 34:
                case 35:
                case 36:
                {
                    return 156;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 149:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 148:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 147:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 146:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 145:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 144:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 157;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 157:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 158;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 158:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 159;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 159:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 160;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 160:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 161;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 161:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 162;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 162:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 163;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 163:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 164;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 164:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 143:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 165;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 165:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 166;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 166:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 167;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 167:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 168;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 168:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 142:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 169;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 169:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 169;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 141:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
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
                    return 169;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 140:
        {
            switch (i)
            {
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 139:
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
                case 0:
                {
                    return 136;
                }
                case 3:
                {
                    return 137;
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
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                {
                    return 138;
                }
                default:
                {
                    return -1;
                }
            }
        }
        case 136:
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
    }
    return -1;
}

int CmajorLexer::GetTokenId(int statementIndex)
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
            return INTLIT;
            break;
        }
        case 3:
        {
            Retract();
            return FLOATINGLIT;
            break;
        }
        case 4:
        {
            Retract();
            return CHARLIT;
            break;
        }
        case 5:
        {
            Retract();
            return STRINGLIT;
            break;
        }
        case 6:
        {
            Retract();
            return EQUIVALENCE;
            break;
        }
        case 7:
        {
            Retract();
            return IMPLICATION;
            break;
        }
        case 8:
        {
            Retract();
            return DISJUNCTION;
            break;
        }
        case 9:
        {
            Retract();
            return AMPAMP;
            break;
        }
        case 10:
        {
            Retract();
            return BITOR;
            break;
        }
        case 11:
        {
            Retract();
            return BITXOR;
            break;
        }
        case 12:
        {
            Retract();
            return AMP;
            break;
        }
        case 13:
        {
            Retract();
            return EQ;
            break;
        }
        case 14:
        {
            Retract();
            return NEQ;
            break;
        }
        case 15:
        {
            Retract();
            return LEQ;
            break;
        }
        case 16:
        {
            Retract();
            return GEQ;
            break;
        }
        case 17:
        {
            Retract();
            return LANGLE;
            break;
        }
        case 18:
        {
            Retract();
            return RANGLE;
            break;
        }
        case 19:
        {
            if (parsingOperatorFunctionGroupId) return INVALID_TOKEN;
            Retract();
            return SHIFTLEFT;
            break;
        }
        case 20:
        {
            if (leftAngleCount > 0) return INVALID_TOKEN;
            Retract();
            return SHIFTRIGHT;
            break;
        }
        case 21:
        {
            Retract();
            return PLUS;
            break;
        }
        case 22:
        {
            Retract();
            return MINUS;
            break;
        }
        case 23:
        {
            Retract();
            return STAR;
            break;
        }
        case 24:
        {
            Retract();
            return DIV;
            break;
        }
        case 25:
        {
            Retract();
            return REM;
            break;
        }
        case 26:
        {
            Retract();
            return PLUSPLUS;
            break;
        }
        case 27:
        {
            Retract();
            return MINUSMINUS;
            break;
        }
        case 28:
        {
            Retract();
            return EXCLAMATION;
            break;
        }
        case 29:
        {
            Retract();
            return CPL;
            break;
        }
        case 30:
        {
            Retract();
            return DOT;
            break;
        }
        case 31:
        {
            Retract();
            return ARROW;
            break;
        }
        case 32:
        {
            Retract();
            return LBRACKET;
            break;
        }
        case 33:
        {
            Retract();
            return RBRACKET;
            break;
        }
        case 34:
        {
            Retract();
            return LPAREN;
            break;
        }
        case 35:
        {
            Retract();
            return RPAREN;
            break;
        }
        case 36:
        {
            Retract();
            return LBRACE;
            break;
        }
        case 37:
        {
            Retract();
            return RBRACE;
            break;
        }
        case 38:
        {
            Retract();
            return COMMA;
            break;
        }
        case 39:
        {
            Retract();
            return ASSIGN;
            break;
        }
        case 40:
        {
            Retract();
            return COLON;
            break;
        }
        case 41:
        {
            Retract();
            return SEMICOLON;
            break;
        }
        case 42:
        {
            Retract();
            return HASH;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
