// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_MATCH_INCLUDED
#define SOULNG_REX_MATCH_INCLUDED
#include <soulng/rex/RexApi.hpp>
#include <string>

namespace soulng { namespace rex {

class Nfa;
class Context;

SOULNG_REX_API bool FilePatternMatch(const std::u32string& filePath, const std::u32string& filePattern);
SOULNG_REX_API bool PatternMatch(const std::u32string& str, const std::u32string& regularExpressionPattern);
SOULNG_REX_API bool PatternMatch(const std::u32string& str, Nfa& nfa);
SOULNG_REX_API Nfa CompileRegularExpressionPattern(Context& context, const std::u32string& regularExpressionPattern);
SOULNG_REX_API Nfa CompileFilePattern(Context& context, const std::u32string& filePattern);

} } // namespace soulng::rex

#endif // SOULNG_REX_MATCH_INCLUDED
