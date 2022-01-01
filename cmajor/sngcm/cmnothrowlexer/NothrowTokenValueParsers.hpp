// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef TOKEN_VALUE_PARSERS_HPP 
#define TOKEN_VALUE_PARSERS_HPP
#include <sngcm/cmnothrowlexer/CmajorNothrowLexerApi.hpp>
#include <soulng/lexer/Token.hpp>
#include <string>
#include <stdint.h>

using namespace soulng::lexer;

const int noPrefix = 0;
const int utf16Prefix = 1;
const int utf32Prefix = 2;

SNGCM_NOTHROW_LEXER_API bool ParseFloatingLiteralNothrow(const std::string& fileName, const Token& token, double& floatingLit, bool& floatingLitFloat);
SNGCM_NOTHROW_LEXER_API bool ParseIntegerLiteralNothrow(const std::string& fileName, const Token& token, uint64_t& intLit, bool& intLitUnsigned);
SNGCM_NOTHROW_LEXER_API bool ParseCharacterLiteralNothrow(const std::string& fileName, const Token& token, char32_t& charLit, int& charLitPrefix);
SNGCM_NOTHROW_LEXER_API bool ParseStringLiteralNothrow(const std::string& fileName, const Token& token, std::u32string& stringLit, int& stringLitPrefix);

#endif // TOKEN_VALUE_PARSERS_HPP
