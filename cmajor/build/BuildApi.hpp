// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_API_INCLUDED
#define CMAJOR_BUILD_API_INCLUDED

#ifdef _WIN32

#ifdef BUILD_EXPORTS
#define BUILD_API __declspec(dllexport)
#else
#define BUILD_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "build.lib")
#elif defined(TRACE)
#pragma comment(lib, "buildt.lib")
#else
#pragma comment(lib, "buildd.lib")
#endif

#endif

#else

#define BUILD_API

#endif

#endif // CMAJOR_BUILD_API_INCLUDED
