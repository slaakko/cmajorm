// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_TOKEN_VALUE_PARSERS_INCLUDED
#define CMAJOR_DEBUG_TOKEN_VALUE_PARSERS_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/lexer/Token.hpp>
#include <string>

namespace cmajor { namespace debug {

std::string ParseString(const soulng::lexer::Token& token);
int ParseInt(const std::string& str);
int64_t ParseLong(const std::string& str);
std::string ParseVmtVariableName(const std::string& str);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_TOKEN_VALUE_PARSERS_INCLUDED
