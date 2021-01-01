// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMRES_API_INCLUDED
#define CMAJOR_CMRES_API_INCLUDED

#ifdef _WIN32

#ifdef CMRES_EXPORTS
#define CMRES_API __declspec(dllexport)
#else
#define CMRES_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmres.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmrest.lib")
#else
#pragma comment(lib, "cmresd.lib")
#endif

#endif

#else

#define CMRES_API

#endif

#endif // CMAJOR_CMRES_API_INCLUDED
