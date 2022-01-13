// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_GUICONL_API_INCLUDED
#define CMSX_GUICONL_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_GUICON_EXPORTS
#define CMSX_GUICON_API __declspec(dllexport)
#else

#define CMSX_GUICON_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_guicon.lib")
#else
#pragma comment(lib, "cmsx_guicond.lib")
#endif

#endif

#else

#define CMSX_GUICON_API

#endif

#endif // CMSX_GUICONL_API_INCLUDED

