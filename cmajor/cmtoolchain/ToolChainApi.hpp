// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMTOOLCHAIN_API_INCLUDED
#define CMAJOR_CMTOOLCHAIN_API_INCLUDED

#ifdef _WIN32

#ifdef CMTOOLCHAIN_EXPORTS
#define CMTOOLCHAIN_API __declspec(dllexport)
#else
#define CMTOOLCHAIN_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmtoolchain.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmtoolchaint.lib")
#else
#pragma comment(lib, "cmtoolchaind.lib")
#endif

#endif

#else

#define CMTOOLCHAIN_API

#endif

#endif // CMAJOR_CMTOOLCHAIN_API_INCLUDED
