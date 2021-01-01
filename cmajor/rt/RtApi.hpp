// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_RT_API_INCLUDED
#define CMAJOR_RT_RT_API_INCLUDED

#ifdef _WIN32

#ifdef _LIB
#define RT_API
#else

#ifdef RT_EXPORTS
#define RT_API __declspec(dllexport)
#else
#define RT_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmrt390.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmrt390t.lib")
#else
#pragma comment(lib, "cmrt390d.lib")
#endif

#endif

#endif

#else

#define RT_API

#endif

#endif // CMAJOR_RT_RT_API_INCLUDED
