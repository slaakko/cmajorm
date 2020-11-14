// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMBS_ERROR_INCLUDED
#define CMBS_ERROR_INCLUDED
#include <soulng/lexer/ParsingException.hpp>
#include <cmajor/cmbs/BuildServerMessage.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <vector>

namespace cmbs {

CompileError ParsingExceptionToError(const soulng::lexer::ParsingException& ex);

std::vector<CompileError> SymbolsExceptionToErrors(const cmajor::symbols::Exception& ex);

} // namespace cmbs

#endif // CMBS_ERROR_INCLUDED
