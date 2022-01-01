// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_IR_API_INCLUDED
#define CMAJOR_IR_API_INCLUDED

#ifdef _WIN32

#ifdef IR_EXPORTS
#define IR_API __declspec(dllexport)
#else
#define IR_API __declspec(dllimport)

#ifdef NDEBUG
#pragma comment(lib, "ir.lib")
#elif defined(TRACE)
#pragma comment(lib, "irt.lib")
#else
#pragma comment(lib, "ird.lib")
#endif

#endif

#else

#define IR_API

#endif

#endif // CMAJOR_IR_API_INCLUDED
