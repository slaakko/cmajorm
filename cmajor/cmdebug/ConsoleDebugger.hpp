// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CONSOLE_DEBUGGER_INCLUDED
#define CMAJOR_DEBUG_CONSOLE_DEBUGGER_INCLUDED
#include <cmajor/cmdebug/Debugger.hpp>
#include <mutex>

namespace cmajor { namespace debug {

class Console;

class DEBUG_API ConsoleDebuggerOutputWriter : public DebuggerOutputWriter
{
public:
    ConsoleDebuggerOutputWriter(CodeFormatter& formatter_);
    void WriteLogMessage(const std::string& message) override;
    void WriteJsonValue(JsonValue* jsonValue) override;
    void WriteWarning(const std::string& warning) override;
private:
    CodeFormatter& formatter;
};

class DEBUG_API ConsoleDebugger : public Debugger
{
public:
    ConsoleDebugger(const std::string& executable, const std::vector<std::string>& args, bool verbose_, CodeFormatter& formatter_, Console& console_, bool breakOnThrow_);
    ~ConsoleDebugger();
    void Prompt() override;
    void WriteTargetOuput(int handle, const std::string& s) override;
    void TargetInputPrompt() override;
    void Error(const std::string& msg) override;
    void StartProgram(bool breakOnThrow_) override;
    void WriteResult(JsonValue* result, Instruction* stoppedInstruction) override;
    void WriteSourceFile(SourceFile& sourceFile, int line, Instruction* stoppedInstruction) override;
    std::string GetTargetInputBytes() override;
    void ProcessConsoleOutput(GdbConsoleOutputRecord* record) override;
    void ProcessTargetOutput(GdbTargetOutputRecord* record) override;
    void ProcessLogOutput(GdbLogOutputRecord* record) override;
    void Proceed() override;
    void ResetConsole() override;
    void SetConsoleActive() override;
    void SetTargetRunning() override;
private:
    CodeFormatter& formatter;
    CodeFormatter outFormatter;
    CodeFormatter errorFormatter;
    Console& console;
    std::recursive_mutex outputMutex;
};

DEBUG_API void RunDebuggerInteractive(const std::string& executable, const std::vector<std::string>& args, bool verbose, bool breakOnThrow, const std::string& version);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CONSOLE_DEBUGGER_INCLUDED
