// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OS_API_INCLUDED
#define CMSX_OS_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_OS_EXPORTS
#define CMSX_OS_API __declspec(dllexport)
#else

#define CMSX_OS_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_os.lib")
#else
#pragma comment(lib, "cmsx_osd.lib")
#endif

#endif

#else

#define CMSX_OS_API

#endif

#endif // CMSX_OS_API_INCLUDED
