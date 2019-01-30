// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_EXCEPTION_INCLUDED
#define CMAJOR_PARSING_EXCEPTION_INCLUDED
#include <cmajor/parsing/Scanner.hpp>
#include <stdexcept>

namespace cmajor { namespace parsing {

PARSING_API void SetFileInfo(const std::vector<std::string>& files_);
PARSING_API std::u32string GetErrorLines(const char32_t* start, const char32_t* end, const Span& span);
PARSING_API void GetColumns(const char32_t* start, const char32_t* end, const Span& span, int32_t& startCol, int32_t& endCol);
PARSING_API void ThrowException(const std::string& message, const Span& span);

class PARSING_API ParsingException : public std::runtime_error
{
public:
    ParsingException(const std::string& message_, const std::string& fileName_, const Span& span_, const char32_t* start_, const char32_t* end_);
    const std::string& Project() const { return project; }
    const std::string& Message() const { return message; }
    const std::string& FileName() const { return fileName; }
    const Span& GetSpan() const { return span; }
    const char32_t* Start() const { return start; }
    const char32_t* End() const { return end; }
    void SetProject(const std::string& project_);
    void* Module() const { return module; }
    void SetModule(void* module_) { module = module_; }
private:
    std::string project;
    std::string message;
    std::string fileName;
    Span span;
    const char32_t* start;
    const char32_t* end;
    void* module;
};

class PARSING_API ExpectationFailure : public ParsingException
{
public:
    ExpectationFailure(const std::u32string& info_, const std::string& fileName_, const Span& span_, const char32_t* start, const char32_t* end);
    const std::u32string& Info() const { return info; }
    void CombineInfo(const std::u32string& parentInfo);
private:
    std::u32string info;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_EXCEPTION_INCLUDED
