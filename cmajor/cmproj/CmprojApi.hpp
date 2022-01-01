// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMPROJ_API_INCLUDED
#define CMAJOR_CMPROJ_API_INCLUDED

#ifdef _WIN32

#ifndef CMPROJ_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "cmproj.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmprojt.lib")
#else
#pragma comment(lib, "cmprojd.lib")
#endif

#endif

#endif

#endif // CMAJOR_CMPROJ_API_INCLUDED
