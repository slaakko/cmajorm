// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_COMMON_INCLUDED
#define CMAJOR_COMMON_INCLUDED
#include <cmajor/parsing/Scanner.hpp>

namespace cmajor { namespace common {

using cmajor::parsing::Span;

class ColumnSpanProvider
{
public:
    virtual int GetColumn(const Span& span) const = 0;
};

} } // cmajor::common

#endif // CMAJOR_COMMON_INCLUDED
