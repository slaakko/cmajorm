// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_ERROR_INCLUDED
#define CMSX_INTERMEDIATE_ERROR_INCLUDED
#include <system-x/intermediate/Api.hpp>
#include <soulng/lexer/SourcePos.hpp>

namespace cmsx::intermediate {

class Context;
using SourcePos = soulng::lexer::SourcePos;

CMSX_INTERMEDIATE_API void Error(const std::string& message, const SourcePos& sourcePos, Context* context);
CMSX_INTERMEDIATE_API void Error(const std::string& message, const SourcePos& sourcePos, Context* context, const SourcePos& refSourcePos);

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_ERROR_INCLUDED
