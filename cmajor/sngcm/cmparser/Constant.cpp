#include "Constant.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmparser/Specifier.hpp>
#include <sngcm/cmparser/TypeExpr.hpp>
#include <sngcm/cmparser/Identifier.hpp>
#include <sngcm/cmparser/Expression.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmlexer/CmajorTokens.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmparser/Constant.parser' using soulng parser generator spg version 3.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorTokens;

soulng::parser::Match ConstantParser::Constant(CmajorLexer& lexer, ParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Constant"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<Node> e = std::unique_ptr<Node>();
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<sngcm::ast::Node> type;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> expr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match = SpecifierParser::Specifiers(lexer);
                                specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                if (match.hit)
                                {
                                    s = span;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == CONST)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch8 = &match;
                        {
                            soulng::parser::Match match(true);
                            soulng::parser::Match* parentMatch9 = &match;
                            {
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match = TypeExprParser::TypeExpr(lexer, ctx);
                                type.reset(static_cast<sngcm::ast::Node*>(match.value));
                                if (match.hit)
                                {
                                    *parentMatch9 = match;
                                }
                                else
                                {
                                    lexer.ThrowExpectationFailure(span, U"type expression");
                                }
                            }
                            *parentMatch8 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch10 = &match;
                    {
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch11 = &match;
                        {
                            soulng::lexer::Span span = lexer.GetSpan();
                            soulng::parser::Match match = IdentifierParser::Identifier(lexer);
                            id.reset(static_cast<IdentifierNode*>(match.value));
                            if (match.hit)
                            {
                                *parentMatch11 = match;
                            }
                            else
                            {
                                lexer.ThrowExpectationFailure(span, U"identifier");
                            }
                        }
                        *parentMatch10 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch12 = &match;
                {
                    soulng::parser::Match match(false);
                    if (*lexer == ASSIGN)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch12 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch13 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch14 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::parser::Match match = ExpressionParser::Expression(lexer, ctx);
                    expr.reset(static_cast<Node*>(match.value));
                    if (match.hit)
                    {
                        e.reset(expr.release());
                    }
                    *parentMatch14 = match;
                }
                *parentMatch13 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch15 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch16 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::parser::Match match(false);
                if (*lexer == SEMICOLON)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    e->SetFullSpan();
                    s.end = e->GetSpan().end;
                    ConstantNode * value = new ConstantNode(s, specifiers->value, type.release(), id.release(), e.release());
                    value->SetStrValue(lexer.GetMatch(s));
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Constant"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, value);
                    }
                }
                *parentMatch16 = match;
            }
            *parentMatch15 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Constant"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Constant"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
