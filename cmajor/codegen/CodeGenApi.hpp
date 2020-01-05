// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGEN_CODEGEN_API_INCLUDED
#define CMAJOR_CODEGEN_CODEGEN_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGEN_EXPORTS
#define CODEGEN_API __declspec(dllexport)
#else
#define CODEGEN_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "codegen.lib")
#else
#pragma comment(lib, "codegend.lib")
#endif

#endif

#else

#define CODEGEN_API

#endif

#endif // CMAJOR_CODEGEN_CODEGEN_API_INCLUDED
