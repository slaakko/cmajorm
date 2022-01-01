// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_NOTHROW_LEXER_API_INCLUDED
#define SNGCM_NOTHROW_LEXER_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGCM_NOTHROW_LEXER_EXPORTS
#define SNGCM_NOTHROW_LEXER_API __declspec(dllexport)
#else

#define SNGCM_NOTHROW_LEXER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngcmnothrowlexer.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngcmnothrowlexert.lib")
#else
#pragma comment(lib, "cmsngcmnothrowlexerd.lib")
#endif

#endif

#else

#define SNGCM_NOTHROW_LEXER_API

#endif

#endif // SNGCM_NOTHROW_LEXER_API_INCLUDED

