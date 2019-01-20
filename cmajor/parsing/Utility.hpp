// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_UTILITY_INCLUDED
#define CMAJOR_PARSING_UTILITY_INCLUDED
#include <stdint.h>
#include <string>
#include <vector>

namespace cmajor { namespace parsing {

std::string HexEscape(char c);
std::string XmlCharStr(char c);
std::string XmlEscape(const std::string& s);

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_UTILITY_INCLUDED
