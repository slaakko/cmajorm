// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_DOM_API_INCLUDED
#define SNGXML_DOM_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGXML_DOM_EXPORTS
#define SNGXML_DOM_API __declspec(dllexport)
#else
#define SNGXML_DOM_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngxmldom.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngxmldomt.lib")
#else
#pragma comment(lib, "cmsngxmldomd.lib")
#endif

#endif

#else

#define SNGXML_DOM_API

#endif

#endif // SNGXML_DOM_API_INCLUDED
