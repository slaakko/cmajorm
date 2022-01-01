// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPM_API_INCLUDED
#define CMAJOR_CMPM_API_INCLUDED

#ifdef _WIN32

#ifdef CMPM_EXPORTS
#define CMPM_API __declspec(dllexport)
#else
#define CMPM_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmpm.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmpmt.lib")
#else
#pragma comment(lib, "cmpmd.lib")
#endif

#endif

#else

#define CMPM_API

#endif

#endif // CMAJOR_CMPM_API_INCLUDED

