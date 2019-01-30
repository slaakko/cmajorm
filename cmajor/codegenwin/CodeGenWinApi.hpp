// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENWIN_CODEGENWIN_API_INCLUDED
#define CMAJOR_CODEGENWIN_CODEGENWIN_API_INCLUDED

#ifdef _WIN32

#ifdef CODEGENWIN_EXPORTS
#define CODEGENWIN_API __declspec(dllexport)
#else
#define CODEGENWIN_API __declspec(dllimport)
#endif

#else

#define CODEGENWIN_API

#endif

#endif // CMAJOR_CODEGENWIN_CODEGENWIN_API_INCLUDED
