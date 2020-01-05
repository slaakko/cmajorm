// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_API_INCLUDED
#define CMAJOR_CMDOCLIB_API_INCLUDED

#ifdef _WIN32

#ifdef CMDOCLIB_EXPORTS
#define CMDOCLIB_API __declspec(dllexport)
#else
#define CMDOCLIB_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmdoclib.lib")
#else
#pragma comment(lib, "cmdoclibd.lib")
#endif

#endif

#else

#define CMDOCLIB_API

#endif

#endif // CMAJOR_CMDOCLIB_API_INCLUDED
