// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_LEXER_API_INCLUDED
#define SNGCM_LEXER_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGCM_LEXER_EXPORTS
#define SNGCM_LEXER_API __declspec(dllexport)
#else

#define SNGCM_LEXER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngcmlexer.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngcmlexert.lib")
#else
#pragma comment(lib, "cmsngcmlexerd.lib")
#endif

#endif

#else

#define SNGCM_LEXER_API

#endif

#endif // SNGCM_LEXER_API_INCLUDED

