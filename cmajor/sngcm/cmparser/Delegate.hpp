#ifndef DELEGATE_HPP
#define DELEGATE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Delegate.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmparser/Delegate.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API DelegateParser
{
    static soulng::parser::Match Delegate(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ClassDelegate(CmajorLexer& lexer, ParsingContext* ctx);
};

#endif // DELEGATE_HPP
