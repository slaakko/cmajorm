// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_ERROR_INCLUDED
#define SOULNG_UTIL_ERROR_INCLUDED
#include <stdexcept>

namespace soulng { namespace util {

#ifdef NDEBUG

#define Assert(expression, message) ((void)0)

#else

#define Assert(expression, message) if (!(expression)) throw std::runtime_error(std::string("assertion failed: ") + message)

#endif

} } // namespace soulng::util

#endif // SOULNG_UTIL_ERROR_INCLUDED
