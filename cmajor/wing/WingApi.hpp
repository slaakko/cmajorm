// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_API_INCLUDED
#define CMAJOR_WING_API_INCLUDED

#if defined(_WIN32) && !defined(__MINGW32__)

#ifdef WING_EXPORTS
#define WING_API __declspec(dllexport)
#else
#define WING_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmwing.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmwingt.lib")
#else
#pragma comment(lib, "cmwingd.lib")
#endif

#endif

#else

#define WING_API

#endif

#endif // CMAJOR_WING_API_INCLUDED
