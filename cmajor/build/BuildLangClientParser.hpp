#ifndef BUILDLANGCLIENTPARSER_HPP
#define BUILDLANGCLIENTPARSER_HPP
#include <cmajor/build/BuildApi.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <cmajor/build/Command.hpp>
#include <memory>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLangClientParser.parser' using soulng parser generator spg version 3.0.0

class BuildLangLexer;

struct BUILD_API BuildLangClientParser
{
    static std::unique_ptr<cmajor::build::Command> Parse(BuildLangLexer& lexer, cmajor::build::BuildOptionSetter* optionSetter);
    static soulng::parser::Match ClientSentence(BuildLangLexer& lexer, cmajor::build::BuildOptionSetter* optionSetter);
    static soulng::parser::Match ClientCommandSentence(BuildLangLexer& lexer);
    static soulng::parser::Match PushProjectSentence(BuildLangLexer& lexer);
    static soulng::parser::Match RemoveProjectSentence(BuildLangLexer& lexer);
    static soulng::parser::Match BuildProjectSentence(BuildLangLexer& lexer);
    static soulng::parser::Match DebugProjectSentence(BuildLangLexer& lexer);
    static soulng::parser::Match InstallProjectSentence(BuildLangLexer& lexer);
    static soulng::parser::Match ProjectFilePath(BuildLangLexer& lexer);
    static soulng::parser::Match DirectoryPath(BuildLangLexer& lexer);
    static soulng::parser::Match ServerName(BuildLangLexer& lexer);
};

#endif // BUILDLANGCLIENTPARSER_HPP
