// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Unicode.hpp>
#include <string>
#include <iostream>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif 

namespace soulng { namespace util {

#ifdef _WIN32

void SetStdHandlesToUtf16Mode()
{
    _setmode(0, _O_U16TEXT);
    _setmode(1, _O_U16TEXT);
    _setmode(2, _O_U16TEXT);
}

void SetStdHandlesToNarrowMode()
{
    _setmode(0, _O_TEXT);
    _setmode(1, _O_TEXT);
    _setmode(2, _O_TEXT);
}

bool IsHandleRedirected(int handle)
{
    return !_isatty(handle);
}

struct UnicodeWriteGuard
{
    UnicodeWriteGuard()
    {
        SetStdHandlesToUtf16Mode();
    }
    ~UnicodeWriteGuard()
    {
        SetStdHandlesToNarrowMode();
    }
};

void WriteUtf16StrToStdOutOrStdErr(const std::u16string& str, FILE* file)
{
    //  precondition: file must be stdout or stderr
    if (file != stdout && file != stderr)
    {
        throw std::runtime_error("WriteUtf16StrToStdOutOrStdErr: precondition violation: file must be stdout or stderr");
    }
    UnicodeWriteGuard unicodeWriteGuard;
    size_t result = std::fwrite(str.c_str(), sizeof(char16_t), str.length(), file);
    if (result != str.length())
    {
        throw std::runtime_error("could not write Unicode text");
    }
}

void WriteUtf8(std::ostream& s, const std::string& str)
{
    if (&s == &std::cout && !IsHandleRedirected(1))
    {
        std::u16string utf16Str = soulng::unicode::ToUtf16(str);
        WriteUtf16StrToStdOutOrStdErr(utf16Str, stdout);
    }
    else if (&s == &std::cerr && !IsHandleRedirected(2))
    {
        std::u16string utf16Str = soulng::unicode::ToUtf16(str);
        WriteUtf16StrToStdOutOrStdErr(utf16Str, stderr);
    }
    else
    {
        s << str;
    }
}

#else // !_WIN32

void WriteUtf8(std::ostream& s, const std::string& str)
{
    s << str;
}

#endif

void CodeFormatter::Write(const std::string& text)
{
    if (atBeginningOfLine)
    {
        if (indent > 0)
        {
            stream << std::string(indentSize * indent, ' ');
            atBeginningOfLine = false;
        }
    }
    WriteUtf8(stream, text);
}

void CodeFormatter::WriteLine(const std::string& text)
{
    Write(text);
    NewLine();
}

void CodeFormatter::NewLine()
{
    stream << "\n";
    atBeginningOfLine = true;
    ++line;
}

} } // namespace soulng::util
