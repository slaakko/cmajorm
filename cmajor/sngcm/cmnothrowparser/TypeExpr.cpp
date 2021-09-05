#include "TypeExpr.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowparser/BasicType.hpp>
#include <sngcm/cmnothrowparser/Expression.hpp>
#include <sngcm/cmnothrowparser/Template.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/TypeExpr.parser' using soulng parser generator spg version 4.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

std::unique_ptr<sngcm::ast::Node> NothrowTypeExprParser::Parse(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    std::unique_ptr<sngcm::ast::Node> value;
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (lexer.Log())
    {
        lexer.Log()->WriteBeginRule(soulng::unicode::ToUtf32("parse"));
        lexer.Log()->IncIndent();
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    ++lexer;
    soulng::lexer::Span span = lexer.GetSpan();
    soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
    value.reset(static_cast<sngcm::ast::Node*>(match.value));
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (lexer.Log())
    {
        lexer.Log()->DecIndent();
        lexer.Log()->WriteEndRule(soulng::unicode::ToUtf32("parse"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (match.hit)
    {
        if (*lexer == soulng::lexer::END_TOKEN)
        {
            return value;
        }
        else
        {
            lexer.AddError(lexer.GetSpan(), ToUtf32(soulng::lexer::GetEndTokenInfo()));
            return value;
        }
    }
    else
    {
        lexer.AddError(span, U"type expression");
        return value;
    }
    return value;
}

soulng::parser::Match NothrowTypeExprParser::TypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("TypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 169);
    std::unique_ptr<sngcm::ast::Node> prefixTypeExpr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match = NothrowTypeExprParser::PrefixTypeExpr(lexer, moduleId, ctx);
        prefixTypeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("TypeExpr"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, prefixTypeExpr.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("TypeExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("TypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowTypeExprParser::PrefixTypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PrefixTypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 170);
    Span s = Span();
    std::unique_ptr<sngcm::ast::Node> constTypeExpr;
    std::unique_ptr<sngcm::ast::Node> postfixTypeExpr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(false);
                if (*lexer == CONST)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    s = span;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match = NothrowTypeExprParser::PostfixTypeExpr(lexer, moduleId, ctx);
                    constTypeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
                    if (match.hit)
                    {
                        s.end = span.end;
                        ConstNode * node = new ConstNode(s, *moduleId, constTypeExpr.release());
                        node->SetLexerFlags(lexer.Flags());
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrefixTypeExpr"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, node);
                        }
                    }
                    *parentMatch4 = match;
                }
                *parentMatch3 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch5 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch6 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::parser::Match match = NothrowTypeExprParser::PostfixTypeExpr(lexer, moduleId, ctx);
                    postfixTypeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrefixTypeExpr"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, postfixTypeExpr.release());
                        }
                    }
                    *parentMatch6 = match;
                }
                *parentMatch5 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrefixTypeExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PrefixTypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowTypeExprParser::PostfixTypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PostfixTypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 171);
    std::unique_ptr<Node> typeExpr = std::unique_ptr<Node>();
    Span s = Span();
    std::unique_ptr<sngcm::ast::Node> primaryTypeExpr;
    std::unique_ptr<Node> size;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match = NothrowTypeExprParser::PrimaryTypeExpr(lexer, moduleId, ctx);
                    primaryTypeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
                    if (match.hit)
                    {
                        s = span;
                        typeExpr.reset(primaryTypeExpr.release());
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        while (true)
                        {
                            int64_t save = lexer.GetPos();
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch7 = &match;
                                    {
                                        int64_t save = lexer.GetPos();
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch8 = &match;
                                        {
                                            int64_t save = lexer.GetPos();
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch9 = &match;
                                            {
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch10 = &match;
                                                {
                                                    int64_t save = lexer.GetPos();
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch11 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == DOT)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        *parentMatch11 = match;
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
                                                                soulng::lexer::Span span = lexer.GetSpan();
                                                                soulng::parser::Match match(false);
                                                                if (*lexer == ID)
                                                                {
                                                                    ++lexer;
                                                                    match.hit = true;
                                                                }
                                                                if (match.hit)
                                                                {
                                                                    s.end = span.end;
                                                                    typeExpr.reset(new DotNode(s, *moduleId, typeExpr.release(), new IdentifierNode(span, *moduleId, lexer.GetToken(pos))));
                                                                }
                                                                *parentMatch13 = match;
                                                            }
                                                            *parentMatch12 = match;
                                                        }
                                                        *parentMatch11 = match;
                                                    }
                                                    *parentMatch10 = match;
                                                    if (!match.hit)
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch14 = &match;
                                                        lexer.SetPos(save);
                                                        {
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch15 = &match;
                                                            {
                                                                int64_t pos = lexer.GetPos();
                                                                soulng::lexer::Span span = lexer.GetSpan();
                                                                soulng::parser::Match match(false);
                                                                if (*lexer == STAR)
                                                                {
                                                                    ++lexer;
                                                                    match.hit = true;
                                                                }
                                                                if (match.hit)
                                                                {
                                                                    s.end = span.end;
                                                                    typeExpr.reset(new PointerNode(s, *moduleId, typeExpr.release()));
                                                                }
                                                                *parentMatch15 = match;
                                                            }
                                                            *parentMatch14 = match;
                                                        }
                                                        *parentMatch10 = match;
                                                    }
                                                }
                                                *parentMatch9 = match;
                                                if (!match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch16 = &match;
                                                    lexer.SetPos(save);
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch17 = &match;
                                                        {
                                                            int64_t pos = lexer.GetPos();
                                                            soulng::lexer::Span span = lexer.GetSpan();
                                                            soulng::parser::Match match(false);
                                                            if (*lexer == AMPAMP)
                                                            {
                                                                ++lexer;
                                                                match.hit = true;
                                                            }
                                                            if (match.hit)
                                                            {
                                                                s.end = span.end;
                                                                typeExpr.reset(new RValueRefNode(s, *moduleId, typeExpr.release()));
                                                            }
                                                            *parentMatch17 = match;
                                                        }
                                                        *parentMatch16 = match;
                                                    }
                                                    *parentMatch9 = match;
                                                }
                                            }
                                            *parentMatch8 = match;
                                            if (!match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch18 = &match;
                                                lexer.SetPos(save);
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch19 = &match;
                                                    {
                                                        int64_t pos = lexer.GetPos();
                                                        soulng::lexer::Span span = lexer.GetSpan();
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == AMP)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        if (match.hit)
                                                        {
                                                            s.end = span.end;
                                                            typeExpr.reset(new LValueRefNode(s, *moduleId, typeExpr.release()));
                                                        }
                                                        *parentMatch19 = match;
                                                    }
                                                    *parentMatch18 = match;
                                                }
                                                *parentMatch8 = match;
                                            }
                                        }
                                        *parentMatch7 = match;
                                        if (!match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch20 = &match;
                                            lexer.SetPos(save);
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch21 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch22 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == LBRACKET)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        *parentMatch22 = match;
                                                    }
                                                    if (match.hit)
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch23 = &match;
                                                        {
                                                            soulng::parser::Match match(true);
                                                            int64_t save = lexer.GetPos();
                                                            soulng::parser::Match* parentMatch24 = &match;
                                                            {
                                                                soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                                                                size.reset(static_cast<Node*>(match.value));
                                                                if (match.hit)
                                                                {
                                                                    *parentMatch24 = match;
                                                                }
                                                                else
                                                                {
                                                                    lexer.SetPos(save);
                                                                }
                                                            }
                                                            *parentMatch23 = match;
                                                        }
                                                        *parentMatch22 = match;
                                                    }
                                                    *parentMatch21 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch25 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch26 = &match;
                                                        {
                                                            int64_t pos = lexer.GetPos();
                                                            soulng::lexer::Span span = lexer.GetSpan();
                                                            soulng::parser::Match match(true);
                                                            soulng::parser::Match* parentMatch27 = &match;
                                                            {
                                                                soulng::lexer::Span span = lexer.GetSpan();
                                                                soulng::parser::Match match(false);
                                                                if (*lexer == RBRACKET)
                                                                {
                                                                    ++lexer;
                                                                    match.hit = true;
                                                                }
                                                                if (match.hit)
                                                                {
                                                                    lexer.ResetRecovered();
                                                                    *parentMatch27 = match;
                                                                }
                                                                else
                                                                {
                                                                    match.hit = true;
                                                                    lexer.SetRecovered();
                                                                    *parentMatch27 = match;
                                                                    lexer.AddError(span, ToUtf32(GetTokenInfo(RBRACKET)));
                                                                }
                                                            }
                                                            if (match.hit)
                                                            {
                                                                s.end = span.end;
                                                                typeExpr.reset(new ArrayNode(s, *moduleId, typeExpr.release(), size.release()));
                                                            }
                                                            *parentMatch26 = match;
                                                        }
                                                        *parentMatch25 = match;
                                                    }
                                                    *parentMatch21 = match;
                                                }
                                                *parentMatch20 = match;
                                            }
                                            *parentMatch7 = match;
                                        }
                                    }
                                    *parentMatch6 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch5 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                    break;
                                }
                            }
                        }
                    }
                    *parentMatch4 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            typeExpr->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PostfixTypeExpr"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, typeExpr.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PostfixTypeExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PostfixTypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowTypeExprParser::PrimaryTypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PrimaryTypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 172);
    std::unique_ptr<Node> basicType;
    std::unique_ptr<TemplateIdNode> templateId;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t save = lexer.GetPos();
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::parser::Match match = NothrowBasicTypeParser::BasicType(lexer, moduleId);
                basicType.reset(static_cast<Node*>(match.value));
                if (match.hit)
                {
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrimaryTypeExpr"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, basicType.release());
                    }
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::parser::Match match = NothrowTemplateParser::TemplateId(lexer, moduleId, ctx);
                        templateId.reset(static_cast<TemplateIdNode*>(match.value));
                        if (match.hit)
                        {
                            {
                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrimaryTypeExpr"));
                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                return soulng::parser::Match(true, templateId.release());
                            }
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch5 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch6 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(false);
                    if (*lexer == ID)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    if (match.hit)
                    {
                        IdentifierNode * node = new IdentifierNode(span, *moduleId, lexer.GetToken(pos));
                        node->SetLexerFlags(lexer.Flags());
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrimaryTypeExpr"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, node);
                        }
                    }
                    *parentMatch6 = match;
                }
                *parentMatch5 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrimaryTypeExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PrimaryTypeExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
