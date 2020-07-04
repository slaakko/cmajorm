
// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebuggerCommand.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebuggerCommandLexer.hpp>
#include <cmajor/cmdebug/DebuggerCommandKeywords.hpp>
#include <soulng/lexer/Token.hpp>
#include <cmajor/cmdebug/DebuggerCommandTokens.hpp>
#include <cmajor/cmdebug/DebuggerCommandClassMap.hpp>

using namespace soulng::lexer;
using namespace DebuggerCommandTokens;

DebuggerCommandLexer::DebuggerCommandLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(content_, fileName_, fileIndex_)
{
    SetKeywordMap(DebuggerCommandKeywords::GetKeywordMap());
}

DebuggerCommandLexer::DebuggerCommandLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : soulng::lexer::Lexer(start_, end_, fileName_, fileIndex_)
{
    SetKeywordMap(DebuggerCommandKeywords::GetKeywordMap());
}

int DebuggerCommandLexer::NextState(int state, char32_t c)
{
    int i = DebuggerCommandClassMap::GetClass(c);
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
                {
                    return 6;
                }
                case 9:
                {
                    return 7;
                }
                case 10:
                {
                    return 8;
                }
                case 11:
                case 12:
                {
                    return 9;
                }
                case 13:
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
                case 13:
                {
                    return 10;
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
                case 7:
                {
                    return 11;
                }
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
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
                case 7:
                {
                    return 11;
                }
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    return 12;
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
                case 9:
                case 10:
                case 11:
                case 12:
                {
                    return 9;
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
                case 7:
                {
                    return 11;
                }
                case 8:
                case 11:
                case 12:
                {
                    return 12;
                }
                case 0:
                case 1:
                {
                    return 13;
                }
                case 9:
                case 10:
                case 13:
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
                case 7:
                {
                    return 11;
                }
                case 8:
                case 11:
                case 12:
                {
                    return 12;
                }
                case 0:
                case 1:
                {
                    return 13;
                }
                case 9:
                case 10:
                case 13:
                {
                    return 14;
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
                case 0:
                case 1:
                case 9:
                case 10:
                case 13:
                {
                    return 13;
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
                case 7:
                {
                    return 11;
                }
                case 8:
                case 11:
                case 12:
                {
                    return 12;
                }
                case 0:
                case 1:
                {
                    return 13;
                }
                case 9:
                case 10:
                case 13:
                {
                    return 14;
                }
                case 6:
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
                case 7:
                {
                    return 6;
                }
                case 9:
                case 10:
                case 11:
                case 12:
                {
                    return 9;
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
                case 9:
                case 10:
                case 11:
                case 12:
                {
                    return 9;
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
            return -1;
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
                case 5:
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
                case 0:
                case 1:
                case 9:
                case 10:
                case 13:
                {
                    return 13;
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

int DebuggerCommandLexer::GetTokenId(int statementIndex)
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
            return NL;
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
            return PATH;
            break;
        }
        case 4:
        {
            Retract();
            return INTEGER;
            break;
        }
        case 5:
        {
            Retract();
            return COLON;
            break;
        }
    }
    return CONTINUE_TOKEN;
}
