// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDEBUG_API_INCLUDED
#define CMAJOR_CMDEBUG_API_INCLUDED

#ifdef _WIN32

#ifdef CMDEBUG_EXPORTS
#define DEBUG_API __declspec(dllexport)
#else
#define DEBUG_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmdebug.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmdebugt.lib")
#else
#pragma comment(lib, "cmdebugd.lib")
#endif

#endif

#else

#define DEBUG_API

#endif

#endif // CMAJOR_CMDEBUG_API_INCLUDED
