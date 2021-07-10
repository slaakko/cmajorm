#include "Constant.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowparser/Specifier.hpp>
#include <sngcm/cmnothrowparser/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/Identifier.hpp>
#include <sngcm/cmnothrowparser/Expression.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Constant.parser' using soulng parser generator spg version 4.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowConstantParser::Constant(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
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
    soulng::lexer::RuleGuard ruleGuard(lexer, 65);
    Span s = Span();
    Span v = Span();
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
                                soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
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
                            soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                            type.reset(static_cast<sngcm::ast::Node*>(match.value));
                            *parentMatch8 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch9 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch10 = &match;
                        {
                            int64_t pos = lexer.GetPos();
                            soulng::lexer::Span span = lexer.GetSpan();
                            soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                            id.reset(static_cast<IdentifierNode*>(match.value));
                            if (match.hit)
                            {
                                v = span;
                            }
                            *parentMatch10 = match;
                        }
                        *parentMatch9 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch11 = &match;
                {
                    soulng::parser::Match match(false);
                    if (*lexer == ASSIGN)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch11 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch12 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch13 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                    expr.reset(static_cast<Node*>(match.value));
                    if (match.hit)
                    {
                        e.reset(expr.release());
                    }
                    *parentMatch13 = match;
                }
                *parentMatch12 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch14 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch15 = &match;
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
                    v.end = e->GetSpan().end;
                    s.end = e->GetSpan().end;
                    ConstantNode * value = new ConstantNode(s, *moduleId, specifiers->value, type.release(), id.release(), e.release());
                    value->SetLexerFlags(lexer.Flags());
                    value->SetStrValue(lexer.GetMatch(v));
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Constant"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, value);
                    }
                }
                *parentMatch15 = match;
            }
            *parentMatch14 = match;
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
