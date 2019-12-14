// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/ParsingLog.hpp>

namespace soulng { namespace lexer {

ParsingLog::ParsingLog() : maxLineLength(80)
{
}

ParsingLog::ParsingLog(int maxLineLength_) : maxLineLength(maxLineLength_)
{
}

ParsingLog::~ParsingLog()
{
}

int ParsingLog::MaxLineLength() const
{
    return maxLineLength;
}

} } // namespace soulng::lexer
