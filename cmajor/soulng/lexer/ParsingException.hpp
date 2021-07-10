// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_PARSING_EXCEPTION_INCLUDED
#define SOULNG_LEXER_PARSING_EXCEPTION_INCLUDED
#include <soulng/lexer/Span.hpp>
#include <stdexcept>
#include <string>

namespace soulng { namespace lexer {

class SOULNG_LEXER_API ParsingException : public std::runtime_error
{
public:
    ParsingException(const std::string& message_, const std::string& fileName_, const Span& span_);
    ParsingException(const std::string& message_, const std::string& fileName_);
    const std::string& Project() const { return project; }
    const std::string& Message() const { return message; }
    const std::string& FileName() const { return fileName; }
    const Span& GetSpan() const { return span; }
    void SetProject(const std::string& project_);
    void* Module() const { return mod; }
    void SetModule(void* mod_) { mod = mod_; }
private:
    std::string project;
    std::string message;
    std::string fileName;
    Span span;
    void* mod;
};

} } // namespace soulng::lexer


#endif // SOULNG_LEXER_PARSING_EXCEPTION_INCLUDED
