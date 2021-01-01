// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_PARSER_API_INCLUDED
#define SOULNG_PARSER_API_INCLUDED

#ifndef NDEBUG
#define SOULNG_PARSER_DEBUG_SUPPORT
#endif

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SOULNG_PARSER_EXPORTS
#define SOULNG_PARSER_API __declspec(dllexport)
#else

#define SOULNG_PARSER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngparser.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngparsert.lib")
#else
#pragma comment(lib, "cmsngparserd.lib")
#endif

#endif

#else

#define SOULNG_PARSER_API

#endif

#endif // SOULNG_PARSER_API_INCLUDED
