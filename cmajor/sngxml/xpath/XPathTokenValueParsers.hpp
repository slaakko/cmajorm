// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_XPATH_TOKEN_VALUE_PARSERS
#define SNGXML_XPATH_XPATH_TOKEN_VALUE_PARSERS
#include <sngxml/xpath/XPathApi.hpp>
#include <soulng/lexer/Token.hpp>

namespace sngxml { namespace xpath {

SNGXML_XPATH_API std::u32string ParseDQString(const std::string& fileName, const soulng::lexer::Token& token);
SNGXML_XPATH_API std::u32string ParseSQString(const std::string& fileName, const soulng::lexer::Token& token);

} } // namespace sngxml::xpath

#endif // SNGXML_XPATH_XPATH_TOKEN_VALUE_PARSERS
