// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGJSON_JSON_TOKEN_VALUE_PARSERS_INCLUDED
#define SNGJSON_JSON_TOKEN_VALUE_PARSERS_INCLUDED
#include <soulng/lexer/Token.hpp>

namespace sngjson { namespace json {

double ParseNumber(const std::string& fileName, const soulng::lexer::Token& token);
std::u32string ParseStringLiteral(const std::string& fileName, const soulng::lexer::Token& token);

} } // namespace sngjson::json

#endif // SNGJSON_JSON_TOKEN_VALUE_PARSERS_INCLUDED
