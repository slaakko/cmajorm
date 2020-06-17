// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_PARSER_API_INCLUDED
#define SNGCM_PARSER_API_INCLUDED
 
#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGCM_PARSER_EXPORTS
#define SNGCM_PARSER_API __declspec(dllexport)
#else

#define SNGCM_PARSER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngcmparser.lib")
#else
#pragma comment(lib, "cmsngcmparserd.lib")
#endif

#endif

#else

#define SNGCM_PARSER_API

#endif

#endif // SNGCM_PARSER_API_INCLUDED
