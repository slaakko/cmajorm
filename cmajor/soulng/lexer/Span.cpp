// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/Span.hpp>

namespace soulng { namespace lexer {

bool operator==(const Span& left, const Span& right)
{
    return left.fileIndex == right.fileIndex && left.line == right.line && left.start == right.start && left.end == right.end;
}

} } // namespace soulng::lexer
