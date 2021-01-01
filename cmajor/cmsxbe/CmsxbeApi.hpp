// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXBE_API_INCLUDED
#define CMAJOR_CMSXBE_API_INCLUDED

#ifdef _WIN32

#ifdef CMSXBE_EXPORTS
#define CMSXBE_API __declspec(dllexport)
#else
#define CMSXBE_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsxbe.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsxbet.lib")
#else
#pragma comment(lib, "cmsxbed.lib")
#endif

#endif

#else

#define CMSXBE_API

#endif

#endif // CMAJOR_CMSXBE_API_INCLUDEDs
