#ifndef BUILDLANGSERVERPARSER_HPP
#define BUILDLANGSERVERPARSER_HPP
#include <cmajor/build/BuildApi.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <memory>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLangServerParser.parser' using soulng parser generator spg version 3.0.0

class BuildLangLexer;

struct BUILD_API BuildLangServerParser
{
    static void Parse(BuildLangLexer& lexer, cmajor::build::BuildOptionSetter* optionSetter);
    static soulng::parser::Match ServerSentence(BuildLangLexer& lexer, cmajor::build::BuildOptionSetter* optionSetter);
    static soulng::parser::Match ServerCommandSentence(BuildLangLexer& lexer);
    static soulng::parser::Match AddServerSentence(BuildLangLexer& lexer);
    static soulng::parser::Match RemoveServerSentence(BuildLangLexer& lexer);
    static soulng::parser::Match RunServerSentence(BuildLangLexer& lexer);
    static soulng::parser::Match ToolChain(BuildLangLexer& lexer);
    static soulng::parser::Match ServerId(BuildLangLexer& lexer);
};

#endif // BUILDLANGSERVERPARSER_HPP
