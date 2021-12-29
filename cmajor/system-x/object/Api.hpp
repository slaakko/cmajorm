// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_API_INCLUDED
#define CMSX_OBJECT_API_INCLUDED

#ifdef _WIN32

#ifdef CMSX_OBJECT_EXPORTS
#define CMSX_OBJECT_API __declspec(dllexport)
#else

#define CMSX_OBJECT_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmsx_object.lib")
#else
#pragma comment(lib, "cmsx_objectd.lib")
#endif

#endif

#else

#define CMSX_OBJECT_API

#endif

#endif // CMSX_OBJECT_API_INCLUDED
