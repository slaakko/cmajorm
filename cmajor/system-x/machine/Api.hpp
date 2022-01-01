// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_API_INCLUDED
#define CMSX_MACHINE_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_MACHINE_EXPORTS
#define CMSX_MACHINE_API __declspec(dllexport)
#else

#define CMSX_MACHINE_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_machine.lib")
#else
#pragma comment(lib, "cmsx_machined.lib")
#endif

#endif

#else

#define CMSX_MACHINE_API

#endif

#endif // CMSX_MACHINE_API_INCLUDED
