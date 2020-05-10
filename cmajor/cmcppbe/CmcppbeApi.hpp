// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPBE_API_INCLUDED
#define CMAJOR_CMCPPBE_API_INCLUDED

#ifdef _WIN32

#ifdef CMCPPBE_EXPORTS
#define CMCPPBE_API __declspec(dllexport)
#else
#define CMCPPBE_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmcppbe.lib")
#else
#pragma comment(lib, "cmcppbed.lib")
#endif

#endif

#else

#define CMCPPBE_API

#endif

#endif // CMAJOR_CMCPPBE_API_INCLUDED
