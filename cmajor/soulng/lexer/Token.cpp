// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/Token.hpp>

namespace soulng { namespace lexer {

bool NoWhiteSpaceBetweenTokens(const Token& first, const Token& second)
{
    if (first.match.end == second.match.begin) return true;
    return false;
}

std::string GetEndTokenInfo()
{
    return "end of file";
}

int TokenLine::TokenIndex(int columnNumber) const
{
    int col = 1;
    int index = 0;
    for (const Token& token : tokens)
    {
        int len = token.match.end - token.match.begin;
        if (columnNumber >= col && columnNumber < col + len)
        {
            return index;
        }
        col += len;
        ++index;
    }
    return -1;
}

} } // namespace soulng::lexer
