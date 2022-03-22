// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_GUITERM_API_INCLUDED
#define CMSX_GUITERM_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_GUITERM_EXPORTS
#define CMSX_GUITERM_API __declspec(dllexport)
#else

#define CMSX_GUITERM_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_guiterm.lib")
#else
#pragma comment(lib, "cmsx_guitermd.lib")
#endif

#endif

#else

#define CMSX_GUITERM_API

#endif

#endif // CMSX_GUITERM_API_INCLUDED
