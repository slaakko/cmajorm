// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENLLVM_CODEGENLLVM_API_INCLUDED
#define CMAJOR_CODEGENLLVM_CODEGENLLVM_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGENLLVM_EXPORTS
#define CODEGENLLVM_API __declspec(dllexport)
#else
#define CODEGENLLVM_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "codegenllvm.lib")
#else
#pragma comment(lib, "codegenllvmd.lib")
#endif

#endif

#else

#define CODEGENLLVM_API

#endif

#endif // CMAJOR_CODEGENLLVM_CODEGENLLVM_API_INCLUDED
