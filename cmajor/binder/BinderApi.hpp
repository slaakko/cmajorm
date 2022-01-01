// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_API_INCLUDED
#define CMAJOR_BINDER_API_INCLUDED

#ifdef _WIN32

#ifdef BINDER_EXPORTS
#define BINDER_API __declspec(dllexport)
#else
#define BINDER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "binder.lib")
#elif defined(TRACE)
#pragma comment(lib, "bindert.lib")
#else
#pragma comment(lib, "binderd.lib")
#endif

#endif

#else

#define BINDER_API

#endif

#endif // CMAJOR_BINDER_API_INCLUDED
