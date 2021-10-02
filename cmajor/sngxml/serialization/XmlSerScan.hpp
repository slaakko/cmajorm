// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XMLSER_SCAN_INCLUDED
#define SNGXML_XMLSER_SCAN_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>
#include <soulng/lexer/Lexer.hpp>

namespace sngxml { namespace xmlser {

SNGXML_SERIALIZATION_API std::string ScanEndCpp(soulng::lexer::Lexer& lexer);

} } // namespace sngxml::xmlser

#endif // SNGXML_XMLSER_SCAN_INCLUDED
