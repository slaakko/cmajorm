// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_API_INCLUDED
#define SOULNG_LEXER_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SOULNG_LEXER_EXPORTS
#define SOULNG_LEXER_API __declspec(dllexport)
#else
#define SOULNG_LEXER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsnglexer.lib")
#else
#pragma comment(lib, "cmsnglexerd.lib")
#endif

#endif

#else

#define SOULNG_LEXER_API

#endif

#endif // SOULNG_LEXER_API_INCLUDED
