#ifndef DEBUGGERCOMMANDPARSER_HPP
#define DEBUGGERCOMMANDPARSER_HPP
#include <cmajor/cmdebug/DebugApi.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <memory>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebuggerCommandParser.parser' using soulng parser generator spg version 3.0.0

class DebuggerCommandLexer;

struct DEBUG_API DebuggerCommandParser
{
    static std::unique_ptr<cmajor::debug::DebuggerCommand> Parse(DebuggerCommandLexer& lexer, std::string* currentSourceFilePath);
    static soulng::parser::Match DebuggerCommand(DebuggerCommandLexer& lexer, std::string* currentSourceFilePath);
    static soulng::parser::Match DebuggerExitCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerHelpCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerNextCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerStepCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerContinueCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerFinishCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerUntilCommand(DebuggerCommandLexer& lexer, std::string* currentSourceFilePath);
    static soulng::parser::Match DebuggerBreakCommand(DebuggerCommandLexer& lexer, std::string* currentSourceFilePath);
    static soulng::parser::Match DebuggerDeleteCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerDepthCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerFramesCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerShowBreakpointCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerShowBreakpointsCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerListCommand(DebuggerCommandLexer& lexer, std::string* currentSourceFilePath);
    static soulng::parser::Match DebuggerPrintCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match DebuggerRepeatLatestCommand(DebuggerCommandLexer& lexer);
    static soulng::parser::Match SourceLocation(DebuggerCommandLexer& lexer, std::string* currentSourceFilePath);
    static soulng::parser::Match Line(DebuggerCommandLexer& lexer);
};

#endif // DEBUGGERCOMMANDPARSER_HPP
