// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_EH_API_INCLUDED
#define CMAJOR_EH_API_INCLUDED

#ifdef _WIN32

#ifndef EH_EXPORTS

#ifdef NDEBUG
#pragma comment(lib, "eh.lib")
#else
#pragma comment(lib, "ehd.lib")
#endif

#endif

#endif

#endif // CMAJOR_EH_API_INCLUDED
