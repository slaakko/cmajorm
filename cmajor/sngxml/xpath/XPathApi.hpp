// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_API_INCLUDED
#define SNGXML_XPATH_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGXML_XPATH_EXPORTS
#define SNGXML_XPATH_API __declspec(dllexport)
#else
#define SNGXML_XPATH_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngxmlxpath.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngxmlxpatht.lib")
#else
#pragma comment(lib, "cmsngxmlxpathd.lib")
#endif

#endif

#else

#define SNGXML_XPATH_API

#endif

#endif // SNGXML_XPATH_API_INCLUDED
