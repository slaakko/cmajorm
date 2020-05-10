// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENCPP_API_INCLUDED
#define CMAJOR_CODEGENCPP_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGENCPP_EXPORTS
#define CODEGENCPP_API __declspec(dllexport)
#else
#define CODEGENCPP_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "codegencpp.lib")
#else
#pragma comment(lib, "codegencppd.lib")
#endif

#endif

#else

#define CODEGENCPP_API

#endif

#endif // CMAJOR_CODEGENCPP_API_INCLUDED
