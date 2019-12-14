// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/ParsingException.hpp>

namespace soulng { namespace lexer {

ParsingException::ParsingException(const std::string& message_, const std::string& fileName_, const Span& span_) :
    std::runtime_error(message_), message(message_), fileName(fileName_), span(span_), mod(nullptr)
{
}

void ParsingException::SetProject(const std::string& project_)
{
    project = project_;
}

} } // namespace soulng::lexer
