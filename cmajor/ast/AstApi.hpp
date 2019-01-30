// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_API_INCLUDED
#define CMAJOR_AST_API_INCLUDED

#ifdef _WIN32

#ifdef AST_EXPORTS
#define AST_API __declspec(dllexport)
#else
#define AST_API __declspec(dllimport)
#endif

#else

#define AST_API

#endif

#endif // CMAJOR_AST_API_INCLUDED
