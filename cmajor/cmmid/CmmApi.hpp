// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MID_CMM_API_INCLUDED
#define CMAJOR_MID_CMM_API_INCLUDED

#ifdef _WIN32

#ifdef CMMID_EXPORTS
#define CMM_API __declspec(dllexport)
#else
#define CMM_API __declspec(dllimport)
#endif

#else

#define CMM_API

#endif

#endif // CMAJOR_MID_CMM_API_INCLUDED
