// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Exception.hpp>
#include <cmajor/util/MappedInputFile.hpp>
#include <cmajor/util/Unicode.hpp>
#include <algorithm>

namespace cmajor { namespace parsing {

using namespace cmajor::util;
using namespace cmajor::unicode;

std::vector<std::string> files;

void SetFileInfo(const std::vector<std::string>& files_)
{
    files = files_;
}

const char32_t* LineStart(const char32_t* start, const char32_t* pos)
{
    while (pos > start && *pos != '\n')
    {
        --pos;
    }
    if (*pos == '\n')
    {
        ++pos;
    }
    return pos;
}

const char32_t* LineEnd(const char32_t* end, const char32_t* pos)
{
    while (pos < end && *pos != '\n')
    {
        ++pos;
    }
    return pos;
}

std::u32string GetErrorLines(const char32_t* start, const char32_t* end, const Span& span)
{
    const char32_t* startPos = start + span.Start();
    if (startPos < start || startPos >= end)
    {
        return std::u32string();
    }
    const char32_t* lineStart = LineStart(start, startPos);
    int cols = static_cast<int>(startPos - lineStart);
    if (cols < 0)
    {
        cols = 0;
    }
    const char32_t* lineEnd = LineEnd(end, startPos);
    if (lineEnd < lineStart)
    {
        lineEnd = lineStart;
    }
    int lineLength = static_cast<int>(lineEnd - lineStart);
    std::u32string lines(NarrowString(lineStart, lineEnd));
    int spanCols = std::max(1, std::min(span.End() -  span.Start(), lineLength - cols));
    lines.append(1, '\n').append(std::u32string(cols, ' ')).append(spanCols, '^');
    return lines;
}

void GetColumns(const char32_t* start, const char32_t* end, const Span& span, int32_t& startCol, int32_t& endCol)
{
    startCol = 0;
    endCol = 0;
    const char32_t* startPos = start + span.Start();
    if (startPos < start || startPos >= end)
    {
        return;
    }
    const char32_t* lineStart = LineStart(start, startPos);
    int cols = static_cast<int>(startPos - lineStart);
    if (cols < 0)
    {
        cols = 0;
    }
    startCol = cols + 1;
    const char32_t* lineEnd = LineEnd(end, startPos);
    if (lineEnd < lineStart)
    {
        lineEnd = lineStart;
    }
    int lineLength = static_cast<int>(lineEnd - lineStart);
    int spanCols = std::max(1, std::min(span.End() - span.Start(), lineLength - cols));
    endCol = startCol + spanCols;
}

void ThrowException(const std::string& message, const Span& span)
{
    if (span.FileIndex() >= 0 && span.FileIndex() < int(files.size()))
    {
        const std::string& filePath = files[span.FileIndex()];
        cmajor::util::MappedInputFile file(filePath);
        std::string s(file.Begin(), file.End());
        std::u32string t(ToUtf32(s));
        throw ParsingException(message, filePath, span, &t[0], &t[0] + t.length());
    }
    throw std::runtime_error(message);
}

ParsingException::ParsingException(const std::string& message_, const std::string& fileName_, const Span& span_, const char32_t* start_, const char32_t* end_):
    std::runtime_error(message_ + " in file " + fileName_ + " at line " + std::to_string(span_.LineNumber()) + ":\n" + ToUtf8(GetErrorLines(start_, end_, span_))), 
    message(message_), fileName(fileName_), span(span_), start(start_), end(end_), module(nullptr)
{
}

void ParsingException::SetProject(const std::string& project_)
{
    project = project_;
}

ExpectationFailure::ExpectationFailure(const std::u32string& info_, const std::string& fileName_, const Span& span_, const char32_t* start, const char32_t* end):
    ParsingException("parsing failed (" +  ToUtf8(info_) + " expected)", fileName_, span_, start, end), info(info_)
{
}

void ExpectationFailure::CombineInfo(const std::u32string& parentInfo)
{
    info = parentInfo + info;
}

} } // namespace cmajor::parsing
