// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_NOTHROW_PARSER_API_INCLUDED
#define SNGCM_NOTHROW_PARSER_API_INCLUDED
 
#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGCM_NOTHROW_PARSER_EXPORTS
#define SNGCM_NOTHROW_PARSER_API __declspec(dllexport)
#else

#define SNGCM_NOTHROW_PARSER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngcmnothrowparser.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngcmnothrowparsert.lib")
#else
#pragma comment(lib, "cmsngcmnothrowparserd.lib")
#endif

#endif

#else

#define SNGCM_NOTHROW_PARSER_API

#endif

#endif // SNGCM_NOTHROW_PARSER_API_INCLUDED
