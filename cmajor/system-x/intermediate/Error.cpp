// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/Context.hpp>
#include <cmajor/symbols/Error.hpp>
#include <soulng/util/Unicode.hpp>
#include <stdexcept>

namespace cmsx::intermediate {

using namespace soulng::unicode;

void Error(const std::string& message, const SourcePos& sourcePos, Context* context)
{
    throw cmajor::symbols::Error(message + " at\n'" + context->FilePath() + "':" + std::to_string(sourcePos.line) + ":\n" + 
        ToUtf8(context->ErrorLines(sourcePos)));
}

void Error(const std::string& message, const SourcePos& sourcePos, Context* context, const SourcePos& refSourcePos)
{
    throw cmajor::symbols::Error(message + " at\n'" + context->FilePath() + "':" + std::to_string(sourcePos.line) + ":\n" +
        ToUtf8(context->ErrorLines(sourcePos)) + ": see reference line " + std::to_string(refSourcePos.line) + ":\n" +
        ToUtf8(context->ErrorLines(refSourcePos)));
}

} // cmsx::intermediate
