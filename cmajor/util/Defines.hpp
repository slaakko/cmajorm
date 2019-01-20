// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_DEFINES_INCLUDED
#define CMAJOR_UTIL_DEFINES_INCLUDED

#ifdef _WIN32
    #if defined(_WIN64)
        #define BITS_64 1
    #else 
        #define BITS_32 1
    #endif
#else
    #if defined(__x86_64__)
        #define BITS_64 1
    #else 
        #define BITS_32 1
    #endif
#endif 

#endif // CMAJOR_UTIL_DEFINES_INCLUDED
