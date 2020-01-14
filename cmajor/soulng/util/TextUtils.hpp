// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_TEXTUTILS_INCLUDED
#define SOULNG_UTIL_TEXTUTILS_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>
#include <vector>
#include <stdint.h>

namespace soulng { namespace util {

enum class FormatWidth
{
    exact, min
};

enum class FormatJustify
{
    left, right
};

UTIL_API std::string Trim(const std::string& s);
UTIL_API std::string TrimAll(const std::string& s);
UTIL_API std::u32string Trim(const std::u32string& s);
UTIL_API std::u32string TrimAll(const std::u32string& s);
UTIL_API std::vector<std::string> Split(const std::string& s, char c);
UTIL_API std::string Replace(const std::string& s, char oldChar, char newChar);
UTIL_API std::string Replace(const std::string& s, const std::string& oldString, const std::string& newString);
UTIL_API std::string HexEscape(char c);
UTIL_API std::u32string HexEscape(uint32_t c);
UTIL_API std::string CharStr(char c);
UTIL_API std::u32string CharStr(char32_t c);
UTIL_API std::string StringStr(const std::string& s);
UTIL_API std::u32string StringStr(const std::u32string& s);
UTIL_API std::string QuotedPath(const std::string& path);
UTIL_API bool LastComponentsEqual(const std::string& s0, const std::string& s1, char componentSeparator);
UTIL_API bool StartsWith(const std::string& s, const std::string& prefix);
UTIL_API bool EndsWith(const std::string& s, const std::string& suffix);
UTIL_API std::string NarrowString(const char* str, int length);
UTIL_API std::string ToUpper(const std::string& s);
UTIL_API std::string ToLower(const std::string& s);
UTIL_API std::string ToString(double x);
UTIL_API std::string ToString(double x, int maxNumDecimals);
UTIL_API std::string ToString(double x, int minNumDecimals, int maxNumDecimals);
UTIL_API std::string ToHexString(uint8_t x);
UTIL_API std::string ToHexString(uint16_t x);
UTIL_API std::string ToHexString(uint32_t x);
UTIL_API std::string ToHexString(uint64_t x);
UTIL_API int Log10(int n);
UTIL_API std::u32string FormatNumber(int n, int numDigits);
UTIL_API std::string CurrentThreadIdStr();
UTIL_API std::string Format(const std::string& s, int width);
UTIL_API std::string Format(const std::string& s, int width, FormatJustify justify);
UTIL_API std::string Format(const std::string& s, int width, FormatWidth fw);
UTIL_API std::string Format(const std::string& s, int width, FormatWidth fw, FormatJustify justify);
UTIL_API std::string Format(const std::string& s, int width, FormatWidth fw, FormatJustify justify, char fillChar);

} } // namespace soulng::util

#endif // SOULNG_UTIL_TEXTUTILS_INCLUDED
