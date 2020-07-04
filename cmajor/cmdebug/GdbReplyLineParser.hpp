#ifndef GDBREPLYLINEPARSER_HPP
#define GDBREPLYLINEPARSER_HPP
#include <cmajor/cmdebug/DebugApi.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <memory>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/GdbReplyLineParser.parser' using soulng parser generator spg version 3.0.0

class GdbReplyLexer;

struct DEBUG_API GdbReplyLineParser
{
    static std::unique_ptr<cmajor::debug::GdbReplyRecord> Parse(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbReplyRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbResultRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbDoneRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbRunningRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbConnectedRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbErrorRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbExitRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbAsyncRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbExecAsyncRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbExecRunningRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbExecStoppedRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbNotifyAsyncRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbStreamRecord(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbPrompt(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbResults(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbResult(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbVariable(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbValue(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbStringValue(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbTupleValue(GdbReplyLexer& lexer);
    static soulng::parser::Match GdbListValue(GdbReplyLexer& lexer);
};

#endif // GDBREPLYLINEPARSER_HPP
