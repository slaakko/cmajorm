// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_SCANNER_INCLUDED
#define CMAJOR_PARSING_SCANNER_INCLUDED
#include <cmajor/parsing/ParsingData.hpp>
#include <memory>
#include <string>
#include <stdint.h>
#include <vector>
#include <stack>

namespace cmajor { namespace parsing {

class Rule;

PARSING_API void SetCountSourceLines(bool count);
PARSING_API int GetParsedSourceLines();

class PARSING_API Span
{
public:
    Span(): fileIndex(-1), lineNumber(-1), start(-1), end(-1) {}
    Span(int32_t fileIndex_): fileIndex(fileIndex_), lineNumber(1), start(0), end(0) {}
    Span(int32_t fileIndex_, int32_t lineNumber_, int32_t start_): fileIndex(fileIndex_), lineNumber(lineNumber_), start(start_), end(start + 1) {}
    Span(int32_t fileIndex_, int32_t lineNumber_, int32_t start_, int32_t end_): fileIndex(fileIndex_), lineNumber(lineNumber_), start(start_), end(end_) {}
    int32_t FileIndex() const { return fileIndex; }
    void SetFileIndex(int32_t fileIndex_) { fileIndex = fileIndex_; }
    int32_t LineNumber() const { return lineNumber; }
    int32_t Start() const { return start; }
    int32_t End() const { return end; }
    bool Valid() const { return start != -1; }
    void IncLineNumber() { ++lineNumber; }
    void operator++()
    {
        ++start;
        ++end;
    }
    void operator--()
    {
        --start;
        --end;
    }
    void SetStart(int32_t start_) { start = start_; }
    void SetEnd(int32_t end_) { end = end_; }
    bool IsNull() const { return fileIndex == 0 && lineNumber == 0 && start == 0 && end == 0; }
private:
    int32_t fileIndex;
    int32_t lineNumber;
    int32_t start;
    int32_t end;
};

inline bool operator==(const Span& left, const Span& right)
{
    return left.FileIndex() == right.FileIndex() && left.LineNumber() == right.LineNumber() && left.Start() == right.Start() && left.End() == right.End();
}

PARSING_API std::u32string NarrowString(const char32_t* start, const char32_t* end);

class Parser;
class XmlLog;
class ExpectationFailure;

class PARSING_API Scanner
{
public:
    Scanner(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_, Parser* skipper_);
    const char32_t* Start() const { return start; }
    const char32_t* End() const { return end; }
    char32_t GetChar() const { return start[span.Start()]; }
    void operator++();
    bool AtEnd() const { return start + span.Start() >= end; }
    void BeginToken() { ++tokenCounter; }
    void EndToken() { --tokenCounter; }
    void Skip();
    const std::string& FileName() const { return fileName; }
    const Span& GetSpan() const { return span; }
    void SetSpan(const Span& span_) { span = span_; }
    bool Skipping() const { return skipping; }
    XmlLog* Log() const { return log; }
    void SetLog(XmlLog* log_) { log = log_; }
    int LineEndIndex(int maxLineLength);
    std::string RestOfLine(int maxLineLength);
    void SetParsingData(ParsingData* parsingData_) { parsingData = parsingData_; }
private:
    const char32_t* start;
    const char32_t* end;
    Parser* skipper;
    bool skipping;
    int tokenCounter;
    std::string fileName;
    Span span;
    XmlLog* log;
    bool atBeginningOfLine;
    ParsingData* parsingData;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_SCANNER_INCLUDED
