// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_API_INCLUDED
#define CMAJOR_UTIL_API_INCLUDED

#ifdef _WIN32

#ifdef UTIL_EXPORTS
#define UTIL_API __declspec(dllexport)
#else
#define UTIL_API __declspec(dllimport)
#endif

#else

#define UTIL_API

#endif

#endif // CMAJOR_UTIL_API_INCLUDED
