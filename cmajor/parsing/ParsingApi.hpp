// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_API_INCLUDED
#define CMAJOR_PARSING_API_INCLUDED

#ifdef _WIN32

#ifdef PARSING_EXPORTS
#define PARSING_API __declspec(dllexport)
#else
#define PARSING_API __declspec(dllimport)
#endif

#else

#define PARSING_API

#endif

#endif // CMAJOR_PARSING_API_INCLUDED
