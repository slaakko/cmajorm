// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_COMMON_INCLUDED
#define CMAJOR_COMMON_INCLUDED
#include <soulng/lexer/Span.hpp>

namespace cmajor { namespace common {

using soulng::lexer::Span;

class ColumnSpanProvider
{
public:
    virtual int GetColumn(const Span& span) const = 0;
};

} } // cmajor::common

#endif // CMAJOR_COMMON_INCLUDED
