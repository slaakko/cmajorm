#ifndef COMMANDLINE_HPP
#define COMMANDLINE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <vector>
#include <string>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmparser/CommandLine.parser' using soulng parser generator spg version 3.0.0

class TrivialLexer;

struct SNGCM_PARSER_API CommandLineParser
{
    static std::vector<std::string> Parse(TrivialLexer& lexer);
    static soulng::parser::Match CommandLine(TrivialLexer& lexer);
    static soulng::parser::Match Spaces(TrivialLexer& lexer);
    static soulng::parser::Match Argument(TrivialLexer& lexer);
    static soulng::parser::Match ArgElement(TrivialLexer& lexer);
    static soulng::parser::Match OddBackslashesAndLiteralQuotationMark(TrivialLexer& lexer);
    static soulng::parser::Match EvenBackslashesAndQuotationMark(TrivialLexer& lexer);
    static soulng::parser::Match StringChar(TrivialLexer& lexer);
};

#endif // COMMANDLINE_HPP
