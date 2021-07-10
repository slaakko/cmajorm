#ifndef DELEGATE_HPP
#define DELEGATE_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Delegate.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Delegate.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowDelegateParser
{
    static soulng::parser::Match Delegate(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match DefiningDelegateId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match ClassDelegate(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match DefiningClassDelegateId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
};

#endif // DELEGATE_HPP
