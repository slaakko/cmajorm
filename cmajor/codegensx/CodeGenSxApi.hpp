// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENSX_API_INCLUDED
#define CMAJOR_CODEGENSX_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGENSX_EXPORTS
#define CODEGENSX_API __declspec(dllexport)
#else
#define CODEGENSX_API __declspec(dllimport)
#endif

#else

#define CODEGENSX_API

#endif

#endif // CMAJOR_CODEGENSX_API_INCLUDED
