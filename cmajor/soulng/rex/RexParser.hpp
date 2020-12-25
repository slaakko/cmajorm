#ifndef REXPARSER_HPP
#define REXPARSER_HPP
#include <soulng/rex/Context.hpp>
#include <soulng/rex/Nfa.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/soulng/rex/RexParser.parser' using soulng parser generator spg version 3.0.0

class RexLexer;

struct SOULNG_REX_API RexParser
{
    static soulng::rex::Nfa Parse(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match RegularExpression(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match Alternative(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match Catenation(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match Repetition(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match Primary(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match Class(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match Range(RexLexer& lexer, soulng::rex::Context* context);
    static soulng::parser::Match Char(RexLexer& lexer);
};

#endif // REXPARSER_HPP
