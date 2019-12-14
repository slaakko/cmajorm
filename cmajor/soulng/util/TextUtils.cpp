// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Util.hpp>
#include <soulng/util/Unicode.hpp>
#include <cctype>
#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <thread>

namespace soulng { namespace util {

using namespace soulng::unicode;

std::string Trim(const std::string& s)
{
    int b = 0;
    while (b < int(s.length()) && std::isspace(s[b])) ++b;
    int e = int(s.length()) - 1;
    while (e >= b && std::isspace(s[e])) --e;
    return s.substr(b, e - b + 1);
}

std::string TrimAll(const std::string& s)
{
    std::string result;
    result.reserve(s.length());
    int state = 0;
    std::string::const_iterator e = s.cend();
    for (std::string::const_iterator i = s.cbegin(); i != e; ++i)
    {
        char c = *i;
        switch (state)
        {
            case 0:
            {
                if (!std::isspace(c))
                {
                    result.append(1, c);
                    state = 1;
                }
                break;
            }
            case 1:
            {
                if (std::isspace(c))
                {
                    state = 2;
                }
                else
                {
                    result.append(1, c);
                }
                break;
            }
            case 2:
            {
                if (!std::isspace(c))
                {
                    result.append(1, ' ');
                    result.append(1, c);
                    state = 1;
                }
                break;
            }
        }
    }
    return result;
}

std::u32string Trim(const std::u32string& s)
{
    int b = 0;
    while (b < int(s.length()) && IsWhiteSpace(s[b])) ++b;
    int e = int(s.length()) - 1;
    while (e >= b && IsWhiteSpace(s[e])) --e;
    return s.substr(b, e - b + 1);
}

std::u32string TrimAll(const std::u32string& s)
{
    std::u32string result;
    result.reserve(s.length());
    int state = 0;
    std::u32string::const_iterator e = s.cend();
    for (std::u32string::const_iterator i = s.cbegin(); i != e; ++i)
    {
        char32_t c = *i;
        switch (state)
        {
            case 0:
            {
                if (!IsWhiteSpace(c))
                {
                    result.append(1, c);
                    state = 1;
                }
                break;
            }
            case 1:
            {
                if (IsWhiteSpace(c))
                {
                    state = 2;
                }
                else
                {
                    result.append(1, c);
                }
                break;
            }
            case 2:
            {
                if (!IsWhiteSpace(c))
                {
                    result.append(1, ' ');
                    result.append(1, c);
                    state = 1;
                }
                break;
            }
        }
    }
    return result;
}

std::vector<std::string> Split(const std::string& s, char c)
{
    std::vector<std::string> v;
    int start = 0;
    int n = int(s.length());
    for (int i = 0; i < n; ++i)
    {
        if (s[i] == c)
        {
            v.push_back(s.substr(start, i - start));
            start = i + 1;
        }
    }
    if (start < n)
    {
        v.push_back(s.substr(start, n - start));
    }
    return v;
}

std::string Replace(const std::string& s, char oldChar, char newChar)
{
    std::string t(s);
    std::string::iterator e = t.end();
    for (std::string::iterator i = t.begin(); i != e; ++i)
    {
        if (*i == oldChar)
        {
            *i = newChar;
        }
    }
    return t;
}

std::string Replace(const std::string& s, const std::string& oldString, const std::string& newString)
{
    std::string r;
    std::string::size_type start = 0;
    std::string::size_type pos = s.find(oldString.c_str(), start);
    while (pos != std::string::npos)
    {
        r.append(s.substr(start, pos - start));
        r.append(newString);
        start = pos + oldString.length();
        pos = s.find(oldString.c_str(), start);
    }
    r.append(s.substr(start, s.length() - start));
    return r;
}

std::string HexEscape(char c)
{
    std::stringstream s;
    s << "\\x" << std::hex << int(static_cast<unsigned char>(c));
    return s.str();
}

std::u32string HexEscape(uint32_t c)
{
    std::stringstream s;
    s << "\\x" << std::hex << c;
    return ToUtf32(s.str());
}

std::string CharStr(char c)
{
    switch (c)
    {
        case '\'': return "\\'";
        case '\"': return "\\\"";
        case '\\': return "\\\\";
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        case '\0': return "\\0";
        default:
        {
            if (c >= 32 && c <= 126)
            {
                return std::string(1, c);
            }
            else
            {
                return HexEscape(c);
            }
        }
    }
}

std::u32string CharStr(char32_t c)
{
    switch (c)
    {
        case '\'': return U"\\'";
        case '\"': return U"\\\"";
        case '\\': return U"\\\\";
        case '\a': return U"\\a";
        case '\b': return U"\\b";
        case '\f': return U"\\f";
        case '\n': return U"\\n";
        case '\r': return U"\\r";
        case '\t': return U"\\t";
        case '\v': return U"\\v";
        case '\0': return U"\\0";
        default:
        {
            if (c >= 32 && c <= 126)
            {
                return std::u32string(1, c);
            }
            else
            {
                return HexEscape(c);
            }
        }
    }
}

std::string StringStr(const std::string& s)
{
    std::string r;
    int n = int(s.length());
    for (int i = 0; i < n; ++i)
    {
        r.append(CharStr(s[i]));
    }
    return r;
}

std::u32string StringStr(const std::u32string& s)
{
    std::u32string r;
    int n = int(s.length());
    for (int i = 0; i < n; ++i)
    {
        r.append(CharStr(s[i]));
    }
    return r;
}

std::string QuotedPath(const std::string& path)
{
    if (path.find(' ') != std::string::npos)
    {
        return std::string("\"") + path + "\"";
    }
    return path;
}

bool LastComponentsEqual(const std::string& s0, const std::string& s1, char componentSeparator)
{
    std::vector<std::string> c0 = Split(s0, componentSeparator);
    std::vector<std::string> c1 = Split(s1, componentSeparator);
    int n0 = int(c0.size());
    int n1 = int(c1.size());
    int n = std::min(n0, n1);
    for (int i = 0; i < n; ++i)
    {
        if (c0[n0 - i - 1] != c1[n1 - i - 1]) return false;
    }
    return true;
}

bool StartsWith(const std::string& s, const std::string& prefix)
{
    int n = int(prefix.length());    
    return int(s.length()) >= n && s.substr(0, n) == prefix;
}

bool EndsWith(const std::string& s, const std::string& suffix)
{
    int n = int(suffix.length());
    int m = int(s.length());
    return m >= n && s.substr(m - n, n) == suffix;
}

std::string NarrowString(const char* str, int length)
{
#if defined(__linux) || defined(__posix) || defined(__unix)
    return std::string(str, length);
#elif defined(_WIN32)
    std::string narrow;
    narrow.reserve(length);
    int state = 0;
    for (int i = 0; i < length; ++i)
    {
        char c = str[i];
        switch (state)
        {
            case 0:
            {
                if (c == '\r') state = 1; else narrow.append(1, c);
                break;
            }
            case 1:
            {
                if (c == '\n') narrow.append(1, '\n'); else narrow.append(1, '\r').append(1, c);
                state = 0;
                break;
            }
        }
    }
    return narrow;
#else
    #error unknown platform
#endif
}

std::string ToUpper(const std::string& s)
{
    std::string result;
    int n = int(s.size());
    result.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        result.append(1, std::toupper(s[i]));
    }
    return result;
}

std::string ToLower(const std::string& s)
{
    std::string result;
    int n = int(s.size());
    result.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        result.append(1, std::tolower(s[i]));
    }
    return result;
}

std::string ToString(double x)
{
    return ToString(x, 15);
}

std::string ToString(double x, int maxNumDecimals)
{
    return ToString(x, 0, maxNumDecimals);
}

std::string ToString(double x, int minNumDecimals, int maxNumDecimals)
{
    std::string result;
    if (x < 0)
    {
        x = -x;
        result.append(1, '-');
    }
    result.append(std::to_string(static_cast<int>(x)));
    double d = x - static_cast<int>(x);
    if (d > 0 || minNumDecimals > 0)
    {
        result.append(1, '.');
        for (int i = 0; (d > 0 || i < minNumDecimals) && i < maxNumDecimals; ++i)
        {
            d = 10 * d;
            int digit = static_cast<int>(d) % 10;
            result.append(1, static_cast<char>(static_cast<int>('0') + digit));
            d = d - static_cast<int>(d);
        }
    }
    return result;
}

inline char HexNibble(uint8_t n)
{
    static const char* h = "0123456789ABCDEF";
    return h[n];
}

std::string ToHexString(uint8_t x)
{
    std::string s;
    s.append(1, HexNibble(x >> 4)).append(1, HexNibble(x & 0x0F));
    return s;
}

std::string ToHexString(uint16_t x)
{
    std::string s;
    s.append(ToHexString(uint8_t((x >> 8) & 0xFF)));
    s.append(ToHexString(uint8_t((x & 0xFF))));
    return s;
}

std::string ToHexString(uint32_t x)
{
    std::string s;
    s.append(ToHexString(uint8_t((x >> 24) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 16) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 8) & 0xFF)));
    s.append(ToHexString(uint8_t((x & 0xFF))));
    return s;
}

std::string ToHexString(uint64_t x)
{
    std::string s;
    s.append(ToHexString(uint8_t((x >> 56) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 48) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 40) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 32) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 24) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 16) & 0xFF)));
    s.append(ToHexString(uint8_t((x >> 8) & 0xFF)));
    s.append(ToHexString(uint8_t((x & 0xFF))));
    return s;
}

int Log10(int n)
{
    int log10 = 1;
    int m = n / 10;
    while (m > 0)
    {
        ++log10;
        m = m / 10;
    }
    return log10;
}

std::u32string FormatNumber(int n, int numDigits)
{
    std::u32string s(numDigits, ' ');
    int k = numDigits - 1;
    while (n > 0)
    {
        if (k < 0)
        {
            throw std::runtime_error("invalid number of digits");
        }
        int digit = n % 10;
        s[k] = digit + '0';
        --k;
        n = n / 10;
    }
    return s;
}

std::string CurrentThreadIdStr()
{
    std::stringstream s;
    s << std::this_thread::get_id();
    return s.str();
}

std::string Format(const std::string& s, int width)
{
    return Format(s, width, FormatWidth::exact);
}

std::string Format(const std::string& s, int width, FormatWidth fw)
{
    if (fw == FormatWidth::min)
    {
        width = std::max(width, int(s.length()));
    }
    std::string result(width, ' ');
    int n = std::min(int(s.length()), width);
    for (int i = 0; i < n; ++i)
    {
        result[i] = s[i];
    }
    return result;
}

} } // namespace soulng::util
