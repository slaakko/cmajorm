// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENBASE_CODEGENBASE_API_INCLUDED
#define CMAJOR_CODEGENBASE_CODEGENBASE_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGENBASE_EXPORTS
#define CODEGENBASE_API __declspec(dllexport)
#else
#define CODEGENBASE_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "codegenbase.lib")
#elif defined(TRACE)
#pragma comment(lib, "codegenbaset.lib")
#else
#pragma comment(lib, "codegenbased.lib")
#endif

#endif

#else

#define CODEGENBASE_API

#endif

#endif // CMAJOR_CODEGENBASE_CODEGENBASE_API_INCLUDED
