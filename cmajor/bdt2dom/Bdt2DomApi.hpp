// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BDT2DOM_API_INCLUDED
#define CMAJOR_BDT2DOM_API_INCLUDED

#ifdef _WIN32

#ifndef BDT2DOM_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "bdt2dom.lib")
#else
#pragma comment(lib, "bdt2domd.lib")
#endif

#endif

#else

#define CMM_API

#endif

#endif // CMAJOR_BDT2DOM_API_INCLUDED
