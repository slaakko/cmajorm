// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_ANSI_INCLUDED
#define SOULNG_UTIL_ANSI_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

namespace soulng { namespace util {

UTIL_API extern const std::string fgBlack;
UTIL_API extern const std::string fgDarkRed;
UTIL_API extern const std::string fgDarkGreen;
UTIL_API extern const std::string fgDarkYellow;
UTIL_API extern const std::string fgDarkBlue;
UTIL_API extern const std::string fgDarkMagenta;
UTIL_API extern const std::string fgDarkCyan;
UTIL_API extern const std::string fgGray;
UTIL_API extern const std::string fgRed;
UTIL_API extern const std::string fgGreen;
UTIL_API extern const std::string fgYellow;
UTIL_API extern const std::string fgBlue;
UTIL_API extern const std::string fgMagenta;
UTIL_API extern const std::string fgCyan;
UTIL_API extern const std::string fgWhite;

UTIL_API extern const std::string bgBlack;
UTIL_API extern const std::string bgDarkRed;
UTIL_API extern const std::string bgDarkGreen;
UTIL_API extern const std::string bgDarkYellow;
UTIL_API extern const std::string bgDarkBlue;
UTIL_API extern const std::string bgDarkMagenta;
UTIL_API extern const std::string bgDarkCyan;
UTIL_API extern const std::string bgGray;
UTIL_API extern const std::string bgRed;
UTIL_API extern const std::string bgGreen;
UTIL_API extern const std::string bgYellow;
UTIL_API extern const std::string bgBlue;
UTIL_API extern const std::string bgMagenta;
UTIL_API extern const std::string bgCyan;
UTIL_API extern const std::string bgWhite;

UTIL_API extern const std::string reset;

#ifdef _WIN32

UTIL_API std::string AnsiIntercept(int handle, const std::string& text);

#endif

UTIL_API void AnsiInit();
UTIL_API void AnsiDone();

} } // namespace soulng::util

#endif // SOULNG_UTIL_ANSI_INCLUDED
