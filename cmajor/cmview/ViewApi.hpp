// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_VIEW_API_INCLUDED
#define CMAJOR_VIEW_VIEW_API_INCLUDED

#ifdef _WIN32

#ifdef CMVIEW_EXPORTS
#define CMVIEW_API __declspec(dllexport)
#else
#define CMVIEW_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmview.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmviewt.lib")
#else
#pragma comment(lib, "cmviewd.lib")
#endif

#endif

#else

#define CMVIEW_API

#endif

#endif // CMAJOR_VIEW_VIEW_API_INCLUDED
