// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_API_INCLUDED
#define CMAJOR_CMSXI_API_INCLUDED

#ifdef _WIN32

#ifdef CMSXI_EXPORTS
#define CMSXI_API __declspec(dllexport)
#else
#define CMSXI_API __declspec(dllimport)
#endif

#else

#define CMSXI_API

#endif

#endif // CMAJOR_CMSXI_API_INCLUDED
