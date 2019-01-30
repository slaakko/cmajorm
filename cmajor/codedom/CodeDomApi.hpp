// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_API_INCLUDED
#define CMAJOR_CODEDOM_API_INCLUDED

#ifdef _WIN32

#ifdef CODEDOM_EXPORTS
#define CODEDOM_API __declspec(dllexport)
#else
#define CODEDOM_API __declspec(dllimport)
#endif

#else

#define CODEDOM_API

#endif

#endif // CMAJOR_CODEDOM_API_INCLUDED
