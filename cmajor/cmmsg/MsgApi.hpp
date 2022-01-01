// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MSG_API_INCLUDED
#define CMAJOR_MSG_API_INCLUDED

#ifdef _WIN32

#ifdef CMMSG_EXPORTS
#define MSG_API __declspec(dllexport)
#else
#define MSG_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmmsg.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmmsgt.lib")
#else
#pragma comment(lib, "cmmsgd.lib")
#endif

#endif

#else

#define MSG_API

#endif

#endif // CMAJOR_MSG_API_INCLUDED
