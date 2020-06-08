#ifndef BUILDLANGOPTIONPARSER_HPP
#define BUILDLANGOPTIONPARSER_HPP
#include <cmajor/build/BuildApi.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLangOptionParser.parser' using soulng parser generator spg version 3.0.0

class BuildLangLexer;

struct BUILD_API BuildLangOptionParser
{
    static soulng::parser::Match Options(BuildLangLexer& lexer, cmajor::build::BuildOptionSetter* optionSetter);
    static soulng::parser::Match OptionsWithinBrackets(BuildLangLexer& lexer, cmajor::build::BuildOptionSetter* optionSetter);
    static soulng::parser::Match Option(BuildLangLexer& lexer, cmajor::build::BuildOptionSetter* optionSetter);
    static soulng::parser::Match Config(BuildLangLexer& lexer);
    static soulng::parser::Match ToolChain(BuildLangLexer& lexer);
};

#endif // BUILDLANGOPTIONPARSER_HPP
