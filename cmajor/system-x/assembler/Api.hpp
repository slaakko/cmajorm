// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_API_INCLUDED
#define CMSX_ASSEMBLER_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_ASSEMBLER_EXPORTS
#define CMSX_ASSEMBLER_API __declspec(dllexport)
#else

#define CMSX_ASSEMBLER_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_assembler.lib")
#else
#pragma comment(lib, "cmsx_assemblerd.lib")
#endif

#endif

#else

#define CMSX_ASSEMBLER_API

#endif

#endif // CMSX_ASSEMBLER_API_INCLUDED
