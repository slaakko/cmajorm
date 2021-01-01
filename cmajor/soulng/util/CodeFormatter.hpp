// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_CODE_FORMATTER_INCLUDED
#define SOULNG_UTIL_CODE_FORMATTER_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <ostream>
#include <string>
#include <stdint.h>

namespace soulng { namespace util {

class UTIL_API CodeFormatter
{
public:
    CodeFormatter(std::ostream& stream_) : stream(stream_), indent(0), indentSize(4), atBeginningOfLine(true), line(1), start(false), preserveSpace(false), contentCount(0), logging(false) {}
    int Indent() const { return indent; }
    int IndentSize() const { return indentSize; }
    void SetIndentSize(int indentSize_) { indentSize = indentSize_; }
    int CurrentIndent() const { return indentSize * indent; }
    void Write(const std::string& text);
    void WriteLine(const std::string& text);
    void NewLine();
    void WriteLine() { NewLine(); }
    void Flush();
    void IncIndent()
    {
        ++indent;
    }
    void DecIndent()
    {
        --indent;
    }
    int Line() const { return line; }
    void SetLine(int line_) { line = line_; }
    bool Start() const { return start; }
    void SetStart() { start = true; }
    void ResetStart() { start = false; }
    void SetStartText(const std::string& startText_) { startText = startText_; }
    const std::string& StartText() const { return startText; }
    bool PreserveSpace() const { return preserveSpace; }
    void SetPreserveSpace(bool preserveSpace_) { preserveSpace = preserveSpace_; }
    void SetLogging() { logging = true; }
    void BeginContent() { ++contentCount; }
    void EndContent() { --contentCount; }
    typedef std::basic_ostream<char, std::char_traits<char>> CoutType;
    typedef CoutType& (*StandardEndLine)(CoutType&);
    CodeFormatter& operator<<(StandardEndLine manip);
private:
    std::ostream& stream;
    int indent;
    int indentSize;
    bool atBeginningOfLine;
    int line;
    bool start;
    std::string startText;
    bool preserveSpace;
    int contentCount;
    bool logging;
};

UTIL_API CodeFormatter& operator<<(CodeFormatter& f, const std::string& s);
UTIL_API CodeFormatter& operator<<(CodeFormatter& f, const char* s);
UTIL_API CodeFormatter& operator<<(CodeFormatter& f, char c);
UTIL_API CodeFormatter& operator<<(CodeFormatter& f, bool b);
UTIL_API CodeFormatter& operator<<(CodeFormatter& f, int x);
UTIL_API CodeFormatter& operator<<(CodeFormatter& f, double x);
UTIL_API CodeFormatter& operator<<(CodeFormatter& f, int64_t x);
UTIL_API CodeFormatter& operator<<(CodeFormatter& f, uint64_t x);

UTIL_API void WriteUtf8(std::ostream& s, const std::string& str);
UTIL_API bool IsHandleRedirected(int handle);

} } // namespace soulng::util

#endif // SOULNG_UTIL_CODE_FORMATTER_INCLUDED
