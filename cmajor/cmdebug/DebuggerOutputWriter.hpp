// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DEBUGGER_OUTPUT_WRITER_INCLUDED
#define CMAJOR_DEBUG_DEBUGGER_OUTPUT_WRITER_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/Json.hpp>
#include <string>

namespace cmajor { namespace debug {

class Instruction;

class DEBUG_API DebuggerOutputWriter
{
public:
    virtual ~DebuggerOutputWriter();
    virtual void WriteLogMessage(const std::string& message) = 0;
    virtual void WriteJsonValue(soulng::util::JsonValue* jsonValue) = 0;
    virtual void WriteWarning(const std::string& warning) = 0;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUGGER_OUTPUT_WRITER_INCLUDED
