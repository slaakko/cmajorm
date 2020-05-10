// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_API_INCLUDED
#define CMAJOR_CMCPPI_API_INCLUDED

#ifdef _WIN32

#ifdef CMCPPI_EXPORTS
#define CMCPPI_API __declspec(dllexport)
#else
#define CMCPPI_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "cmcppi.lib")
#else
#pragma comment(lib, "cmcppid.lib")
#endif

#endif

#else

#define CMCPPI_API

#endif

#endif // CMAJOR_CMCPPI_API_INCLUDED
