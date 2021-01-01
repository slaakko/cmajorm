// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_API_INCLUDED
#define SNGXML_XML_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef SNGXML_XML_EXPORTS
#define SNGXML_XML_API __declspec(dllexport)
#else
#define SNGXML_XML_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngxmlxml.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsngxmlxmlt.lib")
#else
#pragma comment(lib, "cmsngxmlxmld.lib")
#endif

#endif

#else

#define SNGXML_XML_API

#endif

#endif // SNGXML_XML_API_INCLUDED
