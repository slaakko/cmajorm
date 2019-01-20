// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Scanner.hpp>
#include <cmajor/parsing/Parser.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <algorithm>
#include <cctype>

namespace cmajor { namespace parsing {

std::u32string NarrowString(const char32_t* start, const char32_t* end)
{
    std::u32string s;
    s.reserve(end - start);
    while (start != end)
    {
        if (*start != '\r')
        {
            s.append(1, *start);
        }
        ++start;
    }
    return s;
}

bool countSourceLines = false;
int numParsedSourceLines = 0;

void SetCountSourceLines(bool count)
{
    countSourceLines = count;
}

int GetParsedSourceLines()
{
    return numParsedSourceLines;
}

Scanner::Scanner(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_, Parser* skipper_):
    start(start_), end(end_), skipper(skipper_), skipping(false), tokenCounter(0), fileName(fileName_), span(fileIndex_),
    log(nullptr), atBeginningOfLine(true), parsingData(nullptr)
{
    if (countSourceLines)
    {
        numParsedSourceLines = 0;
    }
}

void Scanner::operator++()
{
    char32_t c = GetChar();
    if (countSourceLines)
    {
        if (!skipping && atBeginningOfLine && !std::isspace(c))
        {
            ++numParsedSourceLines;
            atBeginningOfLine = false;
        }
    }
    ++span;
    if (c == '\n')
    {
        atBeginningOfLine = true;
        span.IncLineNumber();
    }
}

void Scanner::Skip()
{
    if (tokenCounter == 0 && skipper)
    {
        Span save = span;
        BeginToken();
        skipping = true;
        ObjectStack stack;
        Match match = skipper->Parse(*this, stack, parsingData);
        skipping = false;
        EndToken();
        if (!match.Hit())
        {
            span = save;
        }
    }
}

int Scanner::LineEndIndex(int maxLineLength)
{
    int lineEndIndex = span.Start();
    int contentLength = static_cast<int>(end - start);
    int end = contentLength;
    if (maxLineLength != 0)
    {
        end = std::min(contentLength, span.Start() + maxLineLength);
    }
    while (lineEndIndex < end && (start[lineEndIndex] != '\r' && start[lineEndIndex] != '\n'))
    {
        ++lineEndIndex;
    }
    return lineEndIndex;
}

std::string Scanner::RestOfLine(int maxLineLength)
{
    std::string restOfLine(start + span.Start(), start + LineEndIndex(maxLineLength));
    return restOfLine;
}

} } // namespace cmajor::parsing
