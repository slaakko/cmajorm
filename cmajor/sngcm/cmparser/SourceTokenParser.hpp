// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_PARSER_SOURCE_TOKEN_PARSER_INCLUDED
#define SNGCM_PARSER_SOURCE_TOKEN_PARSER_INCLUDED
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/SourceToken.hpp>

namespace sngcm { namespace parser {

SNGCM_PARSER_API void ParseSourceLine(const std::u32string& sourceLine, sngcm::ast::SourceTokenFormatter* formatter, bool& inBlockComment);

} } // namespace sngcm::parser

#endif // SNGCM_PARSER_SOURCE_TOKEN_PARSER_INCLUDED
