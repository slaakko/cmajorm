// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENLINUX_CODEGENLINUX_API_INCLUDED
#define CMAJOR_CODEGENLINUX_CODEGENLINUX_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGENLINUX_EXPORTS
#define CODEGENLINUX_API __declspec(dllexport)
#else
#define CODEGENLINUX_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "codegenwlinux.lib")
#else
#pragma comment(lib, "codegenlinuxd.lib")
#endif

#endif

#else

#define CODEGENLINUX_API

#endif

#endif // CMAJOR_CODEGENLINUX_CODEGENLINUX_API_INCLUDED