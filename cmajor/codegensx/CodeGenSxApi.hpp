// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENSX_API_INCLUDED
#define CMAJOR_CODEGENSX_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGENSX_EXPORTS
#define CODEGENSX_API __declspec(dllexport)
#else
#define CODEGENSX_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "codegensx.lib")
#elif defined(TRACE)
#pragma comment(lib, "codegensxt.lib")
#else
#pragma comment(lib, "codegensxd.lib")
#endif

#endif

#else

#define CODEGENSX_API

#endif

#endif // CMAJOR_CODEGENSX_API_INCLUDED
