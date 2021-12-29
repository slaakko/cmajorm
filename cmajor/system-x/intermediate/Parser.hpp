// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_PARSER_INCLUDED
#define CMSX_INTERMEDIATE_PARSER_INCLUDED
#include <system-x/intermediate/Context.hpp>

namespace cmsx::intermediate {

CMSX_INTERMEDIATE_API void Parse(const std::string& filePath, Context& context);

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_PARSER_INCLUDED
