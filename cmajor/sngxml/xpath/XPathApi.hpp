// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_API_INCLUDED
#define SNGXML_XPATH_API_INCLUDED

#ifdef _WIN32

#ifdef SNGXML_XPATH_EXPORTS
#define SNGXML_XPATH_API __declspec(dllexport)
#else
#define SNGXML_XPATH_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngxmlxpath.lib")
#else
#pragma comment(lib, "cmsngxmlxpathd.lib")
#endif

#endif

#else

#define SNGXML_XPATH_API

#endif

#endif // SNGXML_XPATH_API_INCLUDED
