// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMTRACE_TRACE_API_INCLUDED
#define CMAJOR_CMTRACE_TRACE_API_INCLUDED

#ifdef _WIN32

#ifdef CMTRACE_EXPORTS
#define TRACE_API __declspec(dllexport)
#else
#define TRACE_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmtrace.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmtracet.lib")
#else
#pragma comment(lib, "cmtraced.lib")
#endif

#endif

#else

#define TRACE_API

#endif

#endif // CMAJOR_CMTRACE_TRACE_API_INCLUDED
