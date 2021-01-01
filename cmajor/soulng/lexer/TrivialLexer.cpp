// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/TrivialLexer.hpp>

TrivialLexer::TrivialLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) : Lexer(content_, fileName_, fileIndex_)
{
}

TrivialLexer::TrivialLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) : Lexer(start_, end_, fileName_, fileIndex_)
{
}

int TrivialLexer::NextState(int state, char32_t c)
{
    token.id = static_cast<int>(c);
    token.match = lexeme;
    if (c == '\n')
    {
        ++line;
    }
    return -1;
}


std::string GetTrivialTokenInfo(int tokenId)
{
    if (tokenId == soulng::lexer::END_TOKEN)
    {
        return "end of file";
    }
    return std::to_string(tokenId);
}

