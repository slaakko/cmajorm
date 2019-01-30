// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_LLVM_CMLLVM_API_INCLUDED
#define CMAJOR_LLVM_CMLLVM_API_INCLUDED

#ifdef _WIN32

#ifdef CMLLVM_EXPORTS
#define CMLLVM_API __declspec(dllexport)
#else
#define CMLLVM_API __declspec(dllimport)
#endif

#else

#define CMLLVM

#endif

#endif // CMAJOR_LLVM_CMLLVM_API_INCLUDED
