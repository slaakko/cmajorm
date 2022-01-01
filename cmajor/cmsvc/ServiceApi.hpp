// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_API_INCLUDED
#define CMAJOR_SERVICE_API_INCLUDED

#ifdef _WIN32

#ifdef CMSVC_EXPORTS
#define CMSVC_API __declspec(dllexport)
#else
#define CMSVC_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsvc.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmsvct.lib")
#else
#pragma comment(lib, "cmsvcd.lib")
#endif

#endif

#else

#define CMSVC_API

#endif

#endif // CMAJOR_SERVICE_API_INCLUDED
