// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_LEXER_API_INCLUDED
#define SNGCM_LEXER_API_INCLUDED

#ifdef _WIN32

#ifdef SNGCM_LEXER_EXPORTS
#define SNGCM_LEXER_API __declspec(dllexport)
#else

#define SNGCM_LEXER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngcmlexer.lib")
#else
#pragma comment(lib, "cmsngcmlexerd.lib")
#endif

#endif

#else

#define SNGCM_LEXER_API

#endif

#endif // SNGCM_LEXER_API_INCLUDED

