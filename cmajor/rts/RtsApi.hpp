// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_RT_API_INCLUDED
#define CMAJOR_RT_RT_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef _LIB
#define RT_API
#else

#ifdef RT_EXPORTS
#define RT_API __declspec(dllexport)
#else
#define RT_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmrt360.lib")
#else
#pragma comment(lib, "cmrt360d.lib")
#endif

#endif

#endif

#else

#define RT_API

#endif

#endif // CMAJOR_RT_RT_API_INCLUDED
