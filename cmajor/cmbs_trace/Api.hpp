// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMBS_TRACE_API_INCLUDED
#define CMAJOR_CMBS_TRACE_API_INCLUDED

#ifdef _WIN32

#ifdef CMBSTRACE_EXPORTS
#define CMBSTRACE_API __declspec(dllexport)
#else
#define CMBSTRACE_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmbs_trace.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmbs_tracet.lib")
#else
#pragma comment(lib, "cmbs_traced.lib")
#endif

#endif

#else

#define CMBSTRACE_API

#endif

#endif // CMAJOR_CMBS_TRACE_API_INCLUDED
