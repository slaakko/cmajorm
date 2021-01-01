// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <rt/Ascii.hpp>
#include <cctype>

extern "C" RT_API char RtAsciiToUpper(char c)
{
    return std::toupper(static_cast<unsigned char>(c));
}

extern "C" RT_API char RtAsciiToLower(char c)
{
    return std::tolower(static_cast<unsigned char>(c));
}
