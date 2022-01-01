// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCM_API_INCLUDED
#define CMAJOR_CMCM_API_INCLUDED

#ifdef _WIN32

#ifdef CMCM_EXPORTS
#define CMCM_API __declspec(dllexport)
#else
#define CMCM_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmcm.lib")
#elif defined(TRACE)
#pragma comment(lib, "cmcmt.lib")
#else
#pragma comment(lib, "cmcmd.lib")
#endif

#endif

#else

#define CMCM_API

#endif

#endif // CMAJOR_CMCM_API_INCLUDED


