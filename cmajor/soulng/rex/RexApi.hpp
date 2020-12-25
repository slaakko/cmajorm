// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_REX_API_INCLUDED
#define SOULNG_REX_API_INCLUDED

#ifdef _WIN32

#ifdef SOULNG_REX_EXPORTS
#define SOULNG_REX_API __declspec(dllexport)
#else
#define SOULNG_REX_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsngrex.lib")
#else
#pragma comment(lib, "cmsngrexd.lib")
#endif

#endif

#else

#define SOULNG_REX_API

#endif

#endif // SOULNG_REX_API_INCLUDED
