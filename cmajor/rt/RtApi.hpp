// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_RT_API_INCLUDED
#define CMAJOR_RT_RT_API_INCLUDED

#ifdef _WIN32

#ifdef RT_EXPORTS
#define RT_API __declspec(dllexport)
#else
#define RT_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmrt330.lib")
#else
#pragma comment(lib, "cmrt330d.lib")
#endif

#endif

#else

#define RT_API

#endif

#endif // CMAJOR_RT_RT_API_INCLUDED
