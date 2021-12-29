// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_API_INCLUDED
#define CMSX_INTERMEDIATE_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_INTERMEDIATE_EXPORTS
#define CMSX_INTERMEDIATE_API __declspec(dllexport)
#else

#define CMSX_INTERMEDIATE_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_intermediate.lib")
#else
#pragma comment(lib, "cmsx_intermediated.lib")
#endif

#endif

#else

#define CMSX_INTERMEDIATE_API

#endif

#endif // CMSX_INTERMEDIATE_API_INCLUDED
