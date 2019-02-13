// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSYSTEMX_API_INCLUDED
#define CMAJOR_CMSYSTEMX_API_INCLUDED

#ifdef _WIN32

#ifdef CMSYSTEMX_EXPORTS
#define CMSYSTEMX_API __declspec(dllexport)
#else
#define CMSYSTEMX_API __declspec(dllimport)
#endif

#else

#define CMSYSTEMX_API

#endif

#endif // CMAJOR_CMSYSTEMX_API_INCLUDED
