// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef TOKEN_VALUE_PARSERS_HPP 
#define TOKEN_VALUE_PARSERS_HPP
#include <sngcm/cmlexer/CmajorLexerApi.hpp>
#include <soulng/lexer/Token.hpp>
#include <string>
#include <stdint.h>

using namespace soulng::lexer;

const int noPrefix = 0;
const int utf16Prefix = 1;
const int utf32Prefix = 2;

SNGCM_LEXER_API void ParseFloatingLiteral(const std::string& fileName, const Token& token, double& floatingLit, bool& floatingLitFloat);
SNGCM_LEXER_API void ParseIntegerLiteral(const std::string& fileName, const Token& token, uint64_t& intLit, bool& intLitUnsigned);
SNGCM_LEXER_API void ParseCharacterLiteral(const std::string& fileName, const Token& token, char32_t& charLit, int& charLitPrefix);
SNGCM_LEXER_API void ParseStringLiteral(const std::string& fileName, const Token& token, std::u32string& stringLit, int& stringLitPrefix);
SNGCM_LEXER_API std::string MakeFilePath(const Lexeme& lexeme);

#endif // TOKEN_VALUE_PARSERS_HPP
