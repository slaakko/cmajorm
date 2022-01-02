// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_API_INCLUDED
#define CMSX_KERNEL_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_KERNEL_EXPORTS
#define CMSX_KERNEL_API __declspec(dllexport)
#else

#define CMSX_KERNEL_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_kernel.lib")
#else
#pragma comment(lib, "cmsx_kerneld.lib")
#endif

#endif

#else

#define CMSX_KERNEL_API

#endif

#endif // CMSX_KERNEL_API_INCLUDED
