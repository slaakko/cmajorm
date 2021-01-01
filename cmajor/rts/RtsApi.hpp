// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_RT_API_INCLUDED
#define CMAJOR_RTS_RT_API_INCLUDED

#ifdef SHARED

#ifdef RT_EXPORTS
#define RT_API __declspec(dllexport)
#else
#define RT_API __declspec(dllimport)
#endif

#else

#define RT_API

#endif

#endif // CMAJOR_RTS_RT_API_INCLUDED
