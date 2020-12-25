// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_TOKEN_VALUE_PARSERS_INCLUDED
#define SOULNG_REX_TOKEN_VALUE_PARSERS_INCLUDED
#include <soulng/lexer/Token.hpp>
#include <soulng/rex/RexApi.hpp>

namespace soulng { namespace rex {

SOULNG_REX_API char32_t MakeEscapeValue(const std::string& fileName, const soulng::lexer::Token& token);

} } // namespace soulng::rex

#endif // SOULNG_REX_TOKEN_VALUE_PARSERS_INCLUDED
