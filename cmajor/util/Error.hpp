// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_ERROR_INCLUDED
#define CMAJOR_UTIL_ERROR_INCLUDED
#include <stdexcept>

namespace cmajor { namespace util {

#ifdef NDEBUG

#define Assert(expression, message) ((void)0)

#else

#define Assert(expression, message) if (!(expression)) throw std::runtime_error(std::string("assertion failed: ") + message)

#endif

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_ERROR_INCLUDED
