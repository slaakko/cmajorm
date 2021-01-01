// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_API_INCLUDED
#define SNGCM_AST_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGCM_AST_EXPORTS
#define SNGCM_AST_API __declspec(dllexport)
#else
#define SNGCM_AST_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngcmast.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngcmastt.lib")
#else
#pragma comment(lib, "cmsngcmastd.lib")
#endif

#endif

#else

#define SNGCM_AST_API

#endif

#endif // SNGCM_AST_API_INCLUDED
