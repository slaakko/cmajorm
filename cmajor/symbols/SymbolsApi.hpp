// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_API_INCLUDED
#define CMAJOR_SYMBOLS_API_INCLUDED

#ifdef _WIN32

#ifdef SYMBOLS_EXPORTS
#define SYMBOLS_API __declspec(dllexport)
#else
#define SYMBOLS_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "symbols.lib")
#elif defined(TRACE)
#pragma comment(lib, "symbolst.lib")
#else
#pragma comment(lib, "symbolsd.lib")
#endif

#endif

#else

#define SYMBOLS_API

#endif

#endif // CMAJOR_SYMBOLS_API_INCLUDED
