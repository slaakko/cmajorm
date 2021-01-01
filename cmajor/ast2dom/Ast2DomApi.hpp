// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST2DOM_API_INCLUDED
#define CMAJOR_AST2DOM_API_INCLUDED

#ifdef _WIN32

#ifndef AST2DOM_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "ast2dom.lib")
#elif defined(TRACE)
#pragma comment(lib, "ast2domt.lib")
#else
#pragma comment(lib, "ast2domd.lib")
#endif

#endif

#endif

#endif // CMAJOR_AST2DOM_API_INCLUDED
