// =================================
// Copyright (c) 2021 Seppo Laakko
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

} } // namespace soulng::lexer
